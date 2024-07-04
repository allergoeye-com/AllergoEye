package com.allergoscope.app;

import android.Manifest;
import android.app.Application;
import android.content.pm.PackageManager;
import android.os.Environment;
import android.util.Log;

import androidx.core.content.ContextCompat;

import com.allergoscope.app.card.CurrentPath;
import com.allergoscope.app.timer.CAlarm;

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.StringTokenizer;

import lib.colorpicker.CSliderLum;
import lib.colorpicker.ColorPlane;
import lib.common.CTimer;
import lib.utils.AGlobals;
import lib.utils.CDateTime;
import lib.utils.FileUtils;

import static lib.utils.FileUtils.AddToLog;

public class App extends Application {
    static public int WM_DESTROY = 0x0002;
    static public int WM_USER = 0x0400;
    static App instance = null;
    static public String defaultPath = null;
    static public CurrentPath cardPath = new CurrentPath();
    static public App GetInstance()
    {
        return instance;
    }
    static public SimpleDateFormat formatDate;


    public App()
    {
        if (instance == null)
        {

            instance = this;
            System.loadLibrary("maindll");
            System.loadLibrary("ViewDoc");

        }
    }
    private Thread.UncaughtExceptionHandler uncaughtExceptionHandler = new Thread.UncaughtExceptionHandler() {
        private Thread.UncaughtExceptionHandler originalUncaughtExceptionHandler = Thread.getDefaultUncaughtExceptionHandler();

        @Override
        public void uncaughtException(Thread thread, Throwable throwable) {
            StringWriter errors = new StringWriter();
            throwable.printStackTrace(new PrintWriter(errors));
            String s = errors.toString();
            throwable.printStackTrace();
            AddToLog("<---------Global Exception HANDLER------------------>");
            AddToLog(s);

            originalUncaughtExceptionHandler.uncaughtException(thread, throwable);
        }
    };
    CAlarm alarm = null;
    @Override
    public final void onCreate()
    {

        File [] f = getExternalMediaDirs();
        defaultPath = f[0].getAbsolutePath();
    	AGlobals instance = new AGlobals(this);
        super.onCreate();
        //defaultPath = getApplicationContext().getFilesDir().getAbsolutePath(); // Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM).getAbsoluteFile() + "/" + "com.allergoscope.app";
    
        alarm = new CAlarm();
        formatDate = CDateTime.formatDate;
        registerActivityLifecycleCallbacks(new AGlobals.CActivityCallback());
        Thread.setDefaultUncaughtExceptionHandler(uncaughtExceptionHandler);

    }
    public void OnTimeButton()
    {
       alarm.OnTimer();
    }

}
