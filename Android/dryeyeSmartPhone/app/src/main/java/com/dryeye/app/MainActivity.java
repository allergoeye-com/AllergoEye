package com.dryeye.app;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.util.DisplayMetrics;
import android.view.MotionEvent;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.appcompat.app.AppCompatActivity;


import com.dryeye.app.wifi.MdnsUtils;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Locale;
import java.util.StringTokenizer;

import lib.ExitProgramActivity;
import lib.common.AActivity;
import lib.common.ActionCallback;
import lib.utils.AGlobals;
import lib.utils.FileUtils;

public class MainActivity extends AActivity {
    public static MainActivity mainActivity = null;
    public CView view;
    Bitmap tmp_bmp = null;
    static SharedPreferences prefs = null;
    static public boolean fClearCache = false;
    static public String ReadIni(String name)
    {
        if (prefs == null)
        {
            prefs = MainActivity.mainActivity.getPreferences(Context.MODE_PRIVATE);
        }
        return prefs != null ? prefs.getString(name, null) : null;

    }
    static public void WriteIni(String name, String data)
    {
        if (prefs == null)
        {
            prefs = MainActivity.mainActivity.getPreferences(Context.MODE_PRIVATE);
        }
        if (prefs != null) {
            prefs.edit().putString(name, data).apply();
        }

    }
    static public void WriteIni(String name, int data)
    {
        if (prefs == null)
        {
            prefs = MainActivity.mainActivity.getPreferences(Context.MODE_PRIVATE);
        }
        if (prefs != null)
        {
        prefs.edit().putInt(name, data).apply();
        }

    }
    static public void WriteIni(String name, float data)
    {
        if (prefs == null)
        {
            prefs = MainActivity.mainActivity.getPreferences(Context.MODE_PRIVATE);
        }
        if (prefs != null)
        {
        prefs.edit().putFloat(name, data).apply();
        }


    }
    static public void WriteIni(String name, boolean data)
    {
        if (prefs == null)
        {
            prefs = MainActivity.mainActivity.getPreferences(Context.MODE_PRIVATE);
        }
        if (prefs != null)
            prefs.edit().putBoolean(name, data).apply();


    }
    static public Integer ReadIniInt(String name)
    {
        if (prefs == null)
        {
            prefs = MainActivity.mainActivity.getPreferences(Context.MODE_PRIVATE);
        }
        return prefs != null && prefs.contains(name)  ? prefs.getInt(name, 0) : null;

    }
    static public Float ReadIniFloat(String name)
    {
        if (prefs == null)
        {
            prefs = MainActivity.mainActivity.getPreferences(Context.MODE_PRIVATE);
        }
        return prefs != null && prefs.contains(name)  ? prefs.getFloat(name, 0.0f) : 0.0f;

    }
    static public Boolean ReadIniBool(String name)
    {
        if (prefs == null)
        {
            prefs = MainActivity.mainActivity.getPreferences(Context.MODE_PRIVATE);
        }
        return prefs != null && prefs.contains(name) ?  prefs.getBoolean(name, false) :false;

    }
    static public ArrayList<ArrayList<String> > DefTest()
    {
        ArrayList<ArrayList<String> > def_value = new ArrayList<ArrayList<String> >();

        try {
            Resources resources = AGlobals.currentActivity.getResources();
            Configuration configuration = resources.getConfiguration();
            configuration.setLocale(new Locale("en"));
            DisplayMetrics displayMetrics = resources.getDisplayMetrics();
            resources.updateConfiguration(configuration, displayMetrics);

            String str = AGlobals.currentActivity.getString(R.string.test_choise);
            ArrayList<String> def = new ArrayList<String>();
            StringTokenizer st1 = new StringTokenizer(str, "/");
            while (st1.hasMoreTokens())
            {
                String mLine = st1.nextToken();
                ArrayList<String> data = new ArrayList<String>();
                StringTokenizer st = new StringTokenizer(mLine, ";");
                while (st.hasMoreTokens())
                {
                    String s = st.nextToken();
                    data.add(s);
                }
                def_value.add(data);

            }
            ArrayList<String> ss =  def_value.get(3);
            for (int i = 0, l = ss.size(); i < l; ++i)
            {
                st1 = new StringTokenizer(ss.get(i), " ");
                String val = null;
                if (st1.hasMoreTokens())
                    val = st1.nextToken();

                String mn = null;
                if (st1.hasMoreTokens())
                    mn = st1.nextToken();
                if (mn != null && val != null)
                {
                    String val1 = val;
                    if (val.length() == 1)
                        val1 = "0" + val;
                    val1 += mn;
                    ss.set(i, val1);
                }


            }
            String res = AGlobals.ReadLang();
            configuration.setLocale(new Locale(res.toLowerCase()));
            displayMetrics = resources.getDisplayMetrics();
            resources.updateConfiguration(configuration, displayMetrics);
            configuration.locale = new Locale(res.toLowerCase());
            resources.updateConfiguration(configuration, displayMetrics);
        }
        catch(Exception e)
        {
            def_value = null;
        }

        return def_value;
    }
    //----------------------------------------------------------------
    //
    //----------------------------------------------------------------
    static public ArrayList<ArrayList<String> > CurTest()
    {
        ArrayList<ArrayList<String> > cur_value = new ArrayList<ArrayList<String> >();
        try {
            String str = AGlobals.currentActivity.getString(R.string.test_choise);
            ArrayList<String> def = new ArrayList<String>();
            StringTokenizer st1 = new StringTokenizer(str, "/");
            while (st1.hasMoreTokens())
            {
                String mLine = st1.nextToken();
                ArrayList<String> data = new ArrayList<String>();
                StringTokenizer st = new StringTokenizer(mLine, ";");
                while (st.hasMoreTokens())
                {
                    String s = st.nextToken();
                    data.add(s);
                }
                cur_value.add(data);

            }

        }
        catch(Exception e)
        {
            cur_value = null;
        }
        return cur_value;
    }
    //----------------------------------------------------
    //
    //----------------------------------------------------
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        if (prefs == null)
            prefs = getPreferences(Context.MODE_PRIVATE);
        super.onCreate(savedInstanceState);

         setContentView(R.layout.activity_main);
        menuMainMenu = new HashMap<String, ActionCallback>();
        menuMainMenu.put("load", new ActionCallback(this, "OnLoadCard", MenuActivity.class, String.class));
        menuMainMenu.put("exit", new ActionCallback(this, "OnExit"));
        menuMainMenu.put("back", new ActionCallback(this, "OnBack"));
        menuMainMenu.put("menu", new ActionCallback(this, "OnLoadMenu"));
        menuMainMenu.put("newcard", new ActionCallback(this, "OnNewCard", MenuActivity.class, String.class));
        menuMainMenu.put("camera", new ActionCallback(this, "OnNewCard", Bitmap.class));
        AGlobals.onResumed = new ActionCallback(this, "OnResume", Activity.class);

    }
    public void OnResume(Activity vc)
    {
        if (tmp_bmp != null && vc instanceof ViewActivity)
        {
            ((ViewActivity)vc).LoadBitmap(tmp_bmp);
            tmp_bmp = null;
        }
    }
    public void OnNewCard(MenuActivity a, String b)
    {
    }
    public void OnNewCard(Bitmap bmp)
    {
        tmp_bmp = bmp;
        if (tmp_bmp != null && AGlobals.currentActivity != null)
        {
            if (!(AGlobals.currentActivity instanceof ViewActivity))
            {
                Intent intent = new Intent(this, ViewActivity.class);
                intent.setFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT);
               // startActivityForResult(intent, 200);
                StartView(intent);
            }
            else
                OnResume(AGlobals.currentActivity);

        }
    }
    //----------------------------------------------------
    //
    //----------------------------------------------------
    public void OnLoadMenu()
    {
        Intent intent = new Intent(this, MenuActivity.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT);
        startActivity(intent);
    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    @Override
    public boolean dispatchTouchEvent(MotionEvent ev)
    {

        return super.dispatchTouchEvent(ev);
    }
    @Override
    public void onDestroy()
    {

        super.onDestroy();

    }
    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    //------------------------------------------------------
    //
    //------------------------------------------------------
    public void OnExit()
    {
        MdnsUtils.Stop();
        String filename = FileUtils.GetLogFileName();
        AGlobals.ClearCache();
        ExitProgramActivity.exitApplication(this);
    }
    public void OnBack()
    {
        super.onBackPressed();
    }
    @Override
    public void Finish()
    {
        MdnsUtils.Stop();
        AGlobals.ClearCache();

    }


    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    @Override
    protected void onPause()
    {
        super.onPause();

    }
    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    @SuppressLint("ApplySharedPref")
    @Override
    protected void onResume()
    {
        super.onResume();
        if (prefs.getBoolean("firstrun", true))
        {
            File dir3 = null;
            dir3 = new File(AGlobals.contextApp.getApplicationInfo().dataDir);
            File dir2 = getCacheDir();
            File dir4 = getBaseContext().getCacheDir();
            File dir5 = getBaseContext().getFilesDir();

            try
            {
                if (dir2 != null)
                    FileUtils.Clear(dir2);

                if (dir3 != null)
                    FileUtils.Clear(dir3);
                if (dir4 != null)
                    FileUtils.Clear(dir4);
                if (dir5 != null)
                    FileUtils.Clear(dir4);
            } catch (Exception e)
            {
                e.printStackTrace();
            }
            prefs.edit().putBoolean("firstrun", false).commit();


        }
        if (mainActivity == null)
            mainActivity = this;
        (new Handler()).postDelayed(new Runnable() {
            @Override
            public void run() {
                OnLoadMenu();
            }
        }, 100);


    }
    void StartView(Intent intent)
    {

        ActivityResultLauncher<Intent> startActivityForResult = registerForActivityResult(
                new ActivityResultContracts.StartActivityForResult(),
                result -> {
                    if (result.getResultCode() == AppCompatActivity.RESULT_OK) {
                        OnLoadMenu();
                    }
                }
        );
        startActivityForResult.launch(intent);
    }
    public void OnLoadCard(MenuActivity ac, String res)
    {
        Intent intent = new Intent(mainActivity, ViewActivity.class);
        if (res != null)
            intent.setData(Uri.parse(res));
        intent.setFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT);
  //      startActivityForResult(intent, 200);
        StartView(intent);
//

    }


}
