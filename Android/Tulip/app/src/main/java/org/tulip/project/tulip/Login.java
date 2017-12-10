package org.tulip.project.tulip;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class Login extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("mbedtls");
        System.loadLibrary("mbedx509");
        System.loadLibrary("mbedcrypto");
        System.loadLibrary("tulipc");
        System.loadLibrary("native-lib");

    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        Button btn = (Button) findViewById(R.id.t_signin);

        btn.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                int ret = 0;
                String user = "";
                String pass = "";

                v.setEnabled(false);

                EditText username = (EditText) findViewById(R.id.tx_user);
                EditText password = (EditText) findViewById(R.id.tx_password);

                user = username.getText().toString();
                pass = password.getText().toString();

                if (user.length() > 3 && pass.length() >= 8) {
                    ret = ClientLogin(user, pass);

                    if (ret == 0) {
                        username.setText("");
                        password.setText("");

                        TulipSession.user = user.trim();
                        TulipSession.password = pass;
                        user = "";
                        pass = "";

                        Intent intent = new Intent(Login.this, MainActivity.class);
                        startActivity(intent);
                    }
                    else{
                        password.setText("");
                    }
                }
                v.setEnabled(true);

            }
        });

    }


    public native int ClientLogin(String user, String pass);
}
