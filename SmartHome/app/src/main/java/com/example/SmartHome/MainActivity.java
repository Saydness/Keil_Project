package com.example.SmartHome;
import android.app.Notification;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

//import com.example.SmartHome.databinding.ActivityMainBinding;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttCallbackExtended;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.nio.charset.StandardCharsets;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.example.SmartHome.R;
import com.github.anastr.speedviewlib.SpeedView;
import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;
import com.github.mikephil.charting.utils.ColorTemplate;
import com.google.android.material.materialswitch.MaterialSwitch;
import com.google.android.material.slider.Slider;

public class MainActivity extends AppCompatActivity {
    private String TAG = "Log";
//    private ActivityMainBinding binding;
    private MqttAndroidClient mqttAndroidClient;
    private MqttConnectOptions mqttConnectOptions;
    private String mqttUrl = "tcp://114.96.79.191:1883";
    private String clientId = "android";
    private String topic = "STM32";
    private SpeedView tempGauge;
    private SpeedView  humidityGauge;
    private TextView tvRecord;
    private TextView tvdeal;
    private LineChart lineChart;
    private ArrayList<Entry> temperatureEntries = new ArrayList<>(); // 存储温度数据点
    private ArrayList<Entry> humidityEntries = new ArrayList<>(); // 存储湿度数据点
    boolean DeviceOnline=false;
    private static final long DEBOUNCE_DELAY = 200;
    private Handler debounceHandler = new Handler(Looper.getMainLooper());
    private Runnable debounceRunnable;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
//        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(R.layout.activity_main);
        Intent intent=getIntent();
        String ipAddress=intent.getStringExtra("IPAddress");
        String deviceName=intent.getStringExtra("deviceName");
          initMqttClient();

        
//        tvRecord=findViewById(R.id.tv_record);
//        tvdeal=findViewById(R.id.tv_deal);

//        initMqttClient();
//         connectIP=findViewById(R.id.ip_input);

        //设置仪表盘属性
        tempGauge = findViewById(R.id.tempGauge);
        humidityGauge = findViewById(R.id.humidityGauge);

        humidityGauge.setUnit("%");
        humidityGauge.invalidate();
        tempGauge.setUnit("℃");
        tempGauge.invalidate();
        updateGaugeValues(0,0);
        lineChart = findViewById(R.id.chart);




        MaterialSwitch materialSwitch=findViewById(R.id.led_switch);
        materialSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean isCheck) {
                if(isCheck){
                    publish("Android","{\"cmd\":\"set\",\"Led\":1}");

                }else
                {
                    publish("Android","{\"cmd\":\"set\",\"Led\":0}");
                }
            }
        });

        Slider slider=findViewById(R.id.efan_set);
        slider.addOnChangeListener(new Slider.OnChangeListener() {
            @Override
            public void onValueChange(@NonNull Slider slider, float value, boolean fromUser) {
                if (fromUser) {
                    // 每次滑动都取消之前的任务
                    if (debounceRunnable != null) {
                        debounceHandler.removeCallbacks(debounceRunnable);
                    }

                    // 记录当前值
                    int speedValue = (int) value;

                    // 创建新的延迟任务
                    debounceRunnable = new Runnable() {
                        @Override
                        public void run() {
                            switch (speedValue) {
                                case 0:
                                    publish("Android", "{\"cmd\":\"set\",\"MotorSpeed\":0}");
                                    break;
                                case 1:
                                    publish("Android", "{\"cmd\":\"set\",\"MotorSpeed\":25}");
                                    break;
                                case 2:
                                    publish("Android", "{\"cmd\":\"set\",\"MotorSpeed\":50}");
                                    break;
                                case 3:
                                    publish("Android", "{\"cmd\":\"set\",\"MotorSpeed\":100}");
                                    break;
                            }
                        }
                    };

                    // 启动延迟
                    debounceHandler.postDelayed(debounceRunnable, DEBOUNCE_DELAY);
                }
            }
        });

        Button btn_angle = findViewById(R.id.btn_angle_OK);
        Slider slider_angle=findViewById(R.id.servo_angle_slider);
        btn_angle.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                int value=(int)slider_angle.getValue();
                publish("Android","{\"cmd\":\"set\",\"DjAngle\":"+value+"}");

            }
        });

        Button clearchart=findViewById(R.id.btn1);
        clearchart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                clearChartData();
            }
        });

        Button chart_switch=findViewById(R.id.btn2);
        chart_switch.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(chart_switch.getText().equals("获取数据")) {
                    chart_switch.setText("停止获取");
                    publish("Android","{\"cmd\":\"get\",\"Param\":\"Weather\"}");

                }
                else if(chart_switch.getText().equals("停止获取"))
                {
                    chart_switch.setText("获取数据");
                    publish("Android","{\"cmd\":\"Stop\"}");
                }


            }
        });




//        Spinner spinner = findViewById(R.id.spinner);
//
//// 定义选项
//        String[] items = {"选项 1", "选项 2", "选项 3", "选项 4"};
//// 创建适配器
//        ArrayAdapter<String> adapter = new ArrayAdapter<>(this, android.R.layout.simple_spinner_item, items);
//        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item); // 设置下拉选项的样式
//// 设置适配器
//        spinner.setAdapter(adapter);
//        spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
//            @Override
//            public void onItemSelected(AdapterView<?> parentView, View selectedItemView, int position, long id) {
//                // 选择某一项后，显示选中的项
//                String selectedItem = (String) parentView.getItemAtPosition(position);
//                Toast.makeText(MainActivity.this, "选中项: " + selectedItem, Toast.LENGTH_SHORT).show();
//            }
//
//            @Override
//            public void onNothingSelected(AdapterView<?> parentView) {
//                // 如果没有选择任何项，可以做一些处理
//            }
//        });


  }

    private void showAlertDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("提醒");
        builder.setMessage("这是一个弹窗提醒");

        builder.setPositiveButton("确定", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                // 处理点击确定后的逻辑
                dialog.dismiss();
            }
        });

        builder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                // 处理点击取消后的逻辑
                dialog.dismiss();
            }
        });

        AlertDialog dialog = builder.create();
        dialog.show();
    }

    private void generateChartData(float temperature, float humidity) {
        temperatureEntries.add(new Entry(temperatureEntries.size(), temperature)); // 温度数据点
        humidityEntries.add(new Entry(humidityEntries.size(), humidity)); // 湿度数据点

        // 创建温度数据集
        LineDataSet temperatureDataSet = new LineDataSet(temperatureEntries, "温度");
        temperatureDataSet.setColor(ColorTemplate.rgb("ff0000")); // 温度线条颜色
        temperatureDataSet.setValueTextColor(Color.RED); // 温度数值颜色

        // 创建湿度数据集
        LineDataSet humidityDataSet = new LineDataSet(humidityEntries, "湿度");
        humidityDataSet.setColor(ColorTemplate.rgb("0000ff")); // 湿度线条颜色
        humidityDataSet.setValueTextColor(Color.BLUE); // 湿度数值颜色

        // 创建 LineData 并设置到图表中
        LineData lineData = new LineData(temperatureDataSet, humidityDataSet);
        lineChart.setData(lineData);
        lineChart.invalidate(); // 刷新图表
    }

    private boolean isEmpty(EditText editText){
        String content = editText.getText().toString().trim();
        if(TextUtils.isEmpty(content)) {
            // EditText 内容为空
            return true;
        } else {
            // EditText 内容不为空
           return  false;
        }
    }

    private void initMqttClient() {
        mqttAndroidClient = new MqttAndroidClient(this, mqttUrl, clientId);
        mqttAndroidClient.setCallback(new MqttCallback());
        mqttConnectOptions = new MqttConnectOptions();
        mqttConnectOptions.setAutomaticReconnect(false);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationHelper notificationHelper = new NotificationHelper(this);
            Notification notification = notificationHelper.createNotification();
            mqttAndroidClient.setForegroundService(notification, 1);
        }

        if (mqttAndroidClient != null && mqttAndroidClient.isConnected()) {
            appendStatus("已连接" + mqttUrl);

        } else {

            mqttAndroidClient = new MqttAndroidClient(this, mqttUrl, clientId);
            mqttAndroidClient.setCallback(new MqttCallback());
            mqttConnectOptions = new MqttConnectOptions();
            mqttConnectOptions.setAutomaticReconnect(false);

            Log.i(TAG, "建立连接");
            try {
                mqttAndroidClient.connect(mqttConnectOptions, null, new IMqttActionListener() {
                    @Override
                    public void onSuccess(IMqttToken asyncActionToken) {
                        appendStatus("连接成功");
                        subscribe("STM32");
                        publish("Android","Connect Successfully");
                    }


                    @Override
                    public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                        appendStatus("连接失败");
                        Log.e(TAG, "connect failed: " + exception.getMessage());
                        runOnUiThread(() -> Toast.makeText(MainActivity.this,"连接失败,请检查网络配置",Toast.LENGTH_SHORT).show());
                    }
                });
            } catch (MqttException e) {
                Log.i(TAG, "connect Exceptions : " + e);
                e.printStackTrace();
            }
        }



    }

    private void updateGaugeValues(float temperature, int humidity) {
        tempGauge.speedTo(temperature);  // 温度
        humidityGauge.speedTo(humidity); // 湿度
    }


    private void connect() {


        if (mqttAndroidClient != null && mqttAndroidClient.isConnected()) {
            appendStatus("已连接" + mqttUrl);

        } else {

            mqttAndroidClient = new MqttAndroidClient(this, mqttUrl, clientId);
            mqttAndroidClient.setCallback(new MqttCallback());
            mqttConnectOptions = new MqttConnectOptions();
            mqttConnectOptions.setAutomaticReconnect(false);

            Log.i(TAG, "建立连接");
            try {
                mqttAndroidClient.connect(mqttConnectOptions, null, new IMqttActionListener() {
                    @Override
                    public void onSuccess(IMqttToken asyncActionToken) {
                        appendStatus("连接成功");
                    }


                    @Override
                    public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                        appendStatus("连接失败");
                        Log.e(TAG, "connect failed: " + exception.getMessage());
                    }
                });
            } catch (MqttException e) {
                Log.i(TAG, "connect Exceptions : " + e);
                e.printStackTrace();
            }
        }
    }




    private void subscribe(String topic) {


        if (mqttAndroidClient == null || !mqttAndroidClient.isConnected()) {
            return;
        }
        Log.i(TAG,"订阅topic");
        try {

            mqttAndroidClient.subscribe(topic, 2, null, new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    appendStatus( "订阅\""+topic+"\"成功");

                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    appendStatus(topic + "订阅失败");
                }
            });
        } catch (MqttException e) {
            Log.i(TAG, "subscribe Exceptions : " + e);
            e.printStackTrace();
        }
    }


    private void publish(String pubtopic,String word) {
        if (mqttAndroidClient == null || !mqttAndroidClient.isConnected()) {
            appendStatus("当前客户端未连接");
            return;
        }

        MqttMessage message = new MqttMessage();
        message.setQos(2);

        message.setPayload(word.getBytes());

        try {


            mqttAndroidClient.publish(pubtopic, message, null, new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    appendStatus("消息发送成功");
                    Log.i(TAG,"Send ssful:"+word);
                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    appendStatus("消息发送失败");
                    Log.i(TAG,"Fail send");
                }
            });
        } catch (MqttException e) {
            Log.i(TAG, "echo Exceptions : " + e);
            e.printStackTrace();
        }

        DeviceOnline =false;

//        new Timer().schedule(new TimerTask() {
//            @Override
//            public void run() {
//                runOnUiThread(() -> {
//                  if()
//                });
//            }
//        }, 10000); // 2000 毫秒后执行（即 2 秒）
    }

    private void disconnect() {
        if (mqttAndroidClient == null || !mqttAndroidClient.isConnected()) {
            return;
        }
        Log.i(TAG,"断开连接");
        try {
            mqttAndroidClient.disconnect();
        } catch (MqttException e) {
            Log.i(TAG, "disconnect Exceptions : " + e);
            e.printStackTrace();
        }
    }

    public class MqttCallback implements MqttCallbackExtended {

        @Override
        public void connectComplete(boolean reconnect, String serverURI) {
            if (reconnect) {
                appendStatus("重连成功");
            } else {
                appendStatus("初始化成功");
            }
        }

        @Override
        public void connectionLost(Throwable cause) {
            appendStatus("连接断开");
        }

        @Override
        public void messageArrived(String topic, MqttMessage message) {
            String payload = new String(message.getPayload(), StandardCharsets.UTF_8);
            Pattern pt = Pattern.compile("Temp:(\\d+\\.\\d) Humi:(\\d+)");
            Matcher mt=pt.matcher(payload);
            if (mt.find()) {
                // 提取温度和湿度的值
                String temperature = mt.group(1); // 第一个捕获组
                String humidity = mt.group(2); // 第二个捕获组
                 updateGaugeValues(Float.parseFloat(temperature),Integer.parseInt(humidity));
                generateChartData(tempGauge.getCurrentSpeed(),
                        humidityGauge.getCurrentSpeed());
                Log.i("Weather","update weather ssful");
            } else if (payload.equals("stm32 offline")) {
                DeviceOnline = false;
                Log.i(TAG,"DeviceOnline:"+DeviceOnline);
            }  else if (payload.equals("stm32 online")) {
                DeviceOnline = true;
                Log.i(TAG,"DeviceOnline:"+DeviceOnline);
            }
            else{
                runOnUiThread(() -> Toast.makeText(MainActivity.this, payload, Toast.LENGTH_SHORT).show());
//            appendMessage(1, new String(message.getPayload()));
//            appendStatus("消息接收成功");

            }
            Log.i(TAG,"receive:"+new String(message.getPayload()));
            Log.i("Weather",payload);
            DeviceOnline =true;
        }

        @Override
        public void deliveryComplete(IMqttDeliveryToken token) {
            try {
                MqttMessage message=token.getMessage();
                String payload = new String(message.getPayload());
                appendMessage(0, payload);
            } catch (MqttException e) {
                throw new RuntimeException(e);
            }


        }
    }

    private void appendStatus(String msg) {
        runOnUiThread(() -> {
//            binding.tvDeal.append(getCurrentTime() + " " + msg + "\n");
//            binding.scrollDeal.post(() -> binding.scrollDeal.fullScroll(View.FOCUS_DOWN));
//            binding.scrollRecord.post(() -> binding.scrollRecord.fullScroll(View.FOCUS_DOWN));
        });
    }

    //0:发送
    //1:接收
    private void appendMessage(int type, String msg) {
//        if (0 == type) {
//            binding.tvRecord.append("发送消息：" + msg + "\n");
//        } else {
//            binding.tvRecord.append("接收消息：" + msg + "\n");
//        }
//        binding.scrollDeal.post(() -> binding.scrollDeal.fullScroll(View.FOCUS_DOWN));
//        binding.scrollRecord.post(() -> binding.scrollRecord.fullScroll(View.FOCUS_DOWN));
    }

    private String getCurrentTime() {
        Date currentTime = new Date();
        SimpleDateFormat sdf = new SimpleDateFormat("MM-dd HH:mm:ss");
        return sdf.format(currentTime);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        disconnect();
    }

    private void clearChartData() {
        temperatureEntries.clear();
        humidityEntries.clear();
        lineChart.clear();
        lineChart.invalidate(); // 刷新图表
    }
}