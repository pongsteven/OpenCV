package com.iview.opencv.application;

import android.app.Application;

/**
 * Created by pengqin on 2016/8/16.
 */
public class MyApplication extends Application {
    @Override
    public void onCreate() {
        super.onCreate();
        CrashHandler crashHandler = CrashHandler.getInstance();
        crashHandler.init(getApplicationContext());
    }
}
