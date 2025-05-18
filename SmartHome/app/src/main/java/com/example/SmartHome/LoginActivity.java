package com.example.SmartHome;

import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import com.example.SmartHome.R;
import com.google.android.material.textfield.TextInputEditText;

import java.util.regex.Pattern;

public class LoginActivity extends AppCompatActivity {
    TextInputEditText deviceName;
    TextInputEditText  IPAddress;
    Button btLogin;
    private static final String IPV4_PATTERN =
            "^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\." +
                    "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\." +
                    "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\." +
                    "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_login_activity);
        deviceName=findViewById(R.id.tv_deviceName);
        IPAddress=findViewById(R.id.tv_ipAddress);
        btLogin=findViewById(R.id.bt_login);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);


            return insets;
        });




        btLogin.setOnClickListener(new View.OnClickListener() {
            @Override

            public void onClick(View view) {
                if(checkRule()){
                    Intent intent=new Intent(LoginActivity.this,MainActivity.class);
                    intent.putExtra("deviceName",deviceName.getText().toString());
                    intent.putExtra("IPAddress",IPAddress.getText().toString());
                    startActivity(intent);
                }

            }
        });
    }

    private boolean checkRule(){
        String deviceName_input=deviceName.getText().toString().trim();
        String ipAddress_input=IPAddress.getText().toString().trim();

       if (TextUtils.isEmpty(deviceName_input)) {
            deviceName.setError("设备名不能为空");
            return  false;
        }
       if (!isValidIPv4(ipAddress_input)) {
            IPAddress.setError("ip地址格式不符");
            return  false;
        }

        return  true;
    }

    private  boolean isValidIPv4(String ip) {
        return ip != null && Pattern.matches(IPV4_PATTERN, ip);
    }
}