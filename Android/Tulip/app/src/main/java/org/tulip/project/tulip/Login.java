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

                EditText username = (EditText) findViewById(R.id.tx_user);
                EditText password = (EditText) findViewById(R.id.tx_password);

                user = username.getText().toString();
                pass = password.getText().toString();
                ret = ClientLogin(user, pass);

                if (ret == 0) {
                    username.setText("");
                    password.setText("");

                    TulipSession.user = user;
                    TulipSession.password = pass;
                    user = "";
                    pass = "";

                    startActivity(new Intent(Login.this, MainActivity.class));
                }

            }
        });

    }


    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native int ClientLogin(String user, String pass);
}
