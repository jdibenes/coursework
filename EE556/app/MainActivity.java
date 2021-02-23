package com.example.test01;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Handler;
import android.widget.TextView;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

public class MainActivity extends AppCompatActivity {
    private Long m_tsupdate = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        setTitle("Final Project");

        final android.content.Context app = this;
        final Handler mHandler = new Handler();

        final Runnable getSensorData = new Runnable() {
            @Override
            public void run() {
                RequestQueue queue = Volley.newRequestQueue(app);
                String url = "http://weather556team3.hopto.org:6112/cgi-bin/sensordataraw";
                StringRequest stringRequest = new StringRequest(Request.Method.GET, url,
                        new Response.Listener<String>() {
                            @Override
                            public void onResponse(String response) {
                                TextView tv_status = (TextView)findViewById(R.id.text_status);

                                TextView tv_temp = (TextView)findViewById(R.id.text_temp);
                                TextView tv_humi = (TextView)findViewById(R.id.text_humi);
                                TextView tv_baro = (TextView)findViewById(R.id.text_baro);
                                TextView tv_phot = (TextView)findViewById(R.id.text_phot);

                                String[] data = response.toString().trim().split(";");
                                if (data.length < 5) {
                                    tv_status.setText("Failed to retrieve data!");
                                }
                                else {
                                    tv_status.setText("Connected to server!");
                                    tv_temp.setText("Temperature: " + data[1]);
                                    tv_humi.setText("Humidity: " + data[2]);
                                    tv_baro.setText("Pressure: " + data[3]);
                                    tv_phot.setText("Light: " + data[4]);

                                    m_tsupdate = System.currentTimeMillis();
                                }
                            }
                        },

                        new Response.ErrorListener() {
                            @Override
                            public void onErrorResponse(VolleyError error) {
                                TextView tv_status = (TextView)findViewById(R.id.text_status);
                                tv_status.setText("Couldn't connect to server!");
                            }
                        }
                );
                queue.add(stringRequest);
                mHandler.postDelayed(this, 10*1000);
            }
        };

        final Runnable updateTimer = new Runnable() {
            @Override
            public void run() {
                if (m_tsupdate != null) {
                    TextView tv_timer = (TextView) findViewById(R.id.text_timer);
                    long diff = (System.currentTimeMillis() - m_tsupdate) / 1000;
                    if (diff < 1) { tv_timer.setText("Last updated: Just now"); }
                    else if (diff == 1) { tv_timer.setText("Last updated: 1 second ago"); }
                    else { tv_timer.setText("Last updated: " + diff + " seconds ago"); }
                }
                mHandler.postDelayed(this, 1000);
            }
        };

        getSensorData.run();
        updateTimer.run();
    }
}
