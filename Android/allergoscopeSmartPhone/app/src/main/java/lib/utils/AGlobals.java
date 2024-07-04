package lib.utils;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.Application;
import android.content.ContentResolver;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.Environment;
import android.os.IBinder;

import androidx.coordinatorlayout.widget.CoordinatorLayout;

import com.google.android.material.floatingactionbutton.FloatingActionButton;

import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentTransaction;
import androidx.appcompat.app.AppCompatActivity;
import androidx.vectordrawable.graphics.drawable.VectorDrawableCompat;

import android.provider.MediaStore;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.View;
import android.view.ViewConfiguration;
import android.view.Window;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import android.widget.Spinner;
import android.widget.TextView;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.HashMap;


import lib.common.ActionCallback;
import lib.common.CHandler;
import lib.explorer.cashe.ImagesCache;
import lib.explorer.common.DirEntry;


import static android.app.AlertDialog.THEME_HOLO_DARK;
import static lib.utils.FileUtils.AddToLog;
import static lib.utils.FileUtils.DeleteDir;
import static java.lang.String.format;

/****************************************************************
 AGlobals(Application ap);
 //---------------------------------------------------
 void showFragment(Fragment fragment, int fragment_container, Bundle args)
 void dismissFragment(Fragment fragment) {

 //---------------------------------------------------
 void hideFloatingActionButton(FloatingActionButton fab);
 void showFloatingActionButton(FloatingActionButton fab);
 //---------------------------------------------------
 String CreateHashKey(String str);
 //---------------------------------------------------
 String [] MaskDesign();
 File[] DirDesign();
 //---------------------------------------------------
 String [] MaskPicture();
 File[] DirPicture();
 //---------------------------------------------------
 Object CreateInstance(Class<?> cls,  Object ... args);
 //-------------------------------------------------------
 void ClearCache()
 CasheFromExplore CreateCache(String key)
 ****************************************************************/

public class AGlobals {

    static public String cachePath = null;
    static public Application App = null;
    static public Context contextApp = null;
    static public ContentResolver contentResolver = null;
    public static Bitmap default_image = null;
    public static Bitmap error_image = null;
    public static Activity currentActivity = null;
    public static int touchSlop = -1;
    public static boolean fNoText = false;
    public static long mainTreadID = 0;
    public static CHandler rootHandler = null;
    public static int SINGER_DIALOG_THEME = THEME_HOLO_DARK; //THEME_HOLO_LIGHT;
    public static ActionCallback onResumed = null;

    //**********************************************
    public static class CasheFromExplore {
        public ImagesCache cache;
        public ArrayList<DirEntry> result;

        public CasheFromExplore()
        {
            cache = new ImagesCache();
            result = new ArrayList<DirEntry>();

        }
        public void Clear()
        {
            cache.Restart();
            result.clear();

        }

    }

    //**********************************************
    static HashMap<String, CasheFromExplore> dataExplore = new HashMap<String, CasheFromExplore>();

    //---------------------------------------------------------
//
//---------------------------------------------------------
    public AGlobals(Application ap)
    {
        if (contextApp == null)
        {
            try
            {
                rootHandler = new CHandler();
                if (ap != null)
                {

                    App = ap;
                    contextApp = App.getApplicationContext();
                    contentResolver = contextApp.getContentResolver();
                    cachePath = Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState()) ||
                            !Environment.isExternalStorageRemovable() ? contextApp.getExternalCacheDir().getPath() :
                            contextApp.getCacheDir().getPath();
                } else
                {
                    Class<?> threadClass = Class.forName("android.app.ActivityThread");
                    Method method = threadClass.getMethod("currentApplication");
                    App = (Application) method.invoke(null, (Object[]) null);
                    contextApp = App.getApplicationContext();
                    contentResolver = contextApp.getContentResolver();
                    cachePath = Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState()) ||
                            !Environment.isExternalStorageRemovable() ? contextApp.getExternalCacheDir().getPath() :
                            contextApp.getCacheDir().getPath();

                }
                mainTreadID = Thread.currentThread().getId();
                String uri = "drawable/empty_photo";
                int imageResource = contextApp.getResources().getIdentifier(uri, null, contextApp.getPackageName());
                default_image = ((BitmapDrawable) ContextCompat.getDrawable(AGlobals.contextApp, imageResource)).getBitmap();
                uri = "drawable/error_photo";
                imageResource = contextApp.getResources().getIdentifier(uri, null, contextApp.getPackageName());
                error_image = ((BitmapDrawable) ContextCompat.getDrawable(AGlobals.contextApp, imageResource)).getBitmap();
                touchSlop = ViewConfiguration.get(contextApp).getScaledTouchSlop();

                App.registerActivityLifecycleCallbacks(new CActivityCallback());

            } catch (Exception e)
            {
                cachePath = null;
            }
        }
    }

    static public void SetTextColor(TextView colorCtrl, int color)
    {

        float r = Color.red(color);
        float g = Color.green(color);
        float b = Color.blue(color);
        int res = (int) ((0.2126 * r) + (0.7152 * g) + (0.0722 * b));
        if (res > 127)
            colorCtrl.setTextColor(0xFF000000);
        else
            colorCtrl.setTextColor(0xFFFFFFFF);
        Integer R = Color.red(color);
        Integer G = Color.green(color);
        Integer B = Color.blue(color);
        String text = R.toString() + "/" + G.toString() + "/" + B.toString();
        colorCtrl.setText(text);
        colorCtrl.setBackgroundColor(color);

    }

    static public void SetBackground(TextView colorCtrl, int color)
    {

        float r = Color.red(color);
        float g = Color.green(color);
        float b = Color.blue(color);
        int res = (int) ((0.2126 * r) + (0.7152 * g) + (0.0722 * b));
        if (res > 127)
            colorCtrl.setTextColor(0xFF000000);
        else
            colorCtrl.setTextColor(0xFFFFFFFF);
        Integer R = Color.red(color);
        Integer G = Color.green(color);
        Integer B = Color.blue(color);
        colorCtrl.setBackgroundColor(color);

    }

    //-------------------------------------------------------
    //
    //-------------------------------------------------------
    static public void showFragment(Fragment fragment, int fragment_container, Bundle args)
    {
        FragmentManager fragmentManager;
        fragmentManager = ((AppCompatActivity) currentActivity).getSupportFragmentManager();
        if (args != null)
            fragment.setArguments(args);
        FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        fragmentTransaction.add(fragment_container, fragment);
        fragmentTransaction.commit();

    }

    //-------------------------------------------------------
    //
    //-------------------------------------------------------
    static public void dismissFragment(Fragment fragment)
    {

        FragmentManager fragmentManager;
        fragmentManager = ((AppCompatActivity) currentActivity).getSupportFragmentManager();
        FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        fragmentTransaction.remove(fragment);
        fragmentTransaction.commitAllowingStateLoss();
    }

    public static boolean hideSpinnerDropDown(Spinner spinner)
    {
        try
        {
            Method method = Spinner.class.getDeclaredMethod("onDetachedFromWindow");
            method.setAccessible(true);
            method.invoke(spinner);
        } catch (Exception e)
        {
            FileUtils.AddToLog(e);
            return false;
        }
        return true;
    }

    //------------------------------------------------------
    //
    //-------------------------------------------------------
    static public String GetString(String s)
    {
        String packageName = ((AppCompatActivity) currentActivity).getPackageName();
        int resId = ((AppCompatActivity) currentActivity).getResources().getIdentifier(s, "string", packageName);
        return ((AppCompatActivity) currentActivity).getString(resId);
    }

    //------------------------------------------------------
    //
    //-------------------------------------------------------
    static public String GetInternalString(String s)
    {
        try
        {

        int id = Resources.getSystem().getIdentifier(s, "string", "android");
        if (id > 0)
        {
            return Resources.getSystem().getString(id);
        }
        else
        {
            String packageName = ((AppCompatActivity)currentActivity).getPackageName();
            int resId = ((AppCompatActivity)currentActivity).getResources().getIdentifier(s, "string", packageName);
            return ((AppCompatActivity)currentActivity).getString(resId);
        }

        }
        catch (Exception e)
        {
            ;
        }
        return null;
    }

    //------------------------------------------------------
    //
    //-------------------------------------------------------
    public static boolean KeyboardIsAtached()
    {
        InputMethodManager imm = (InputMethodManager)((AppCompatActivity)currentActivity).getSystemService(Context.INPUT_METHOD_SERVICE);
        return imm.isAcceptingText();

    }

    //-------------------------------------------------------
    //
    //-------------------------------------------------------
    public static void ShowKeyboard(boolean show)
    {
        InputMethodManager imm = (InputMethodManager) ((AppCompatActivity) currentActivity).getSystemService(Context.INPUT_METHOD_SERVICE);
        if (show)
        {
            imm.toggleSoftInput(InputMethodManager.SHOW_FORCED, 0);
        } else
        {
            {

                View v = ((AppCompatActivity) currentActivity).getCurrentFocus();
                if (v == null)
                    v = new View(currentActivity);
                if (v != null)
                {
                    IBinder b = v.getWindowToken();
                    if (b != null)
                        imm.hideSoftInputFromWindow(b, 0);
                }
            }
        }
    }

    public static void ShowKeyboard(View view)
    {
        if (view.requestFocus())
        {
            InputMethodManager imm = (InputMethodManager)
                    ((AppCompatActivity) currentActivity).getSystemService(Context.INPUT_METHOD_SERVICE);
            boolean isShowing = imm.showSoftInput(view, InputMethodManager.SHOW_FORCED);
            if (!isShowing)
                ((AppCompatActivity) currentActivity).getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_VISIBLE);
        }
    }

    //------------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------------
    public static DisplayMetrics GetDisplayMetrcs()
    {
        DisplayMetrics metrics = null;
        WindowManager wm = currentActivity != null ? (WindowManager)currentActivity.getSystemService(Context.WINDOW_SERVICE) : (WindowManager)contextApp.getSystemService(Context.WINDOW_SERVICE);
        if (wm != null)
        {
            Display display = wm.getDefaultDisplay();
            metrics = new DisplayMetrics();
            display.getMetrics(metrics);
        }
        return metrics;
    }

    //----------------------------------------------------
    //
    //-----------------------------------------------------
    static public float StringToFloat(String s)
    {
        float f;
        try
        {
            int i = s.lastIndexOf(",");
            if (i != -1)
                s = s.replace(',', '.');


            f = Float.valueOf(s.trim()).floatValue();

        } catch (NumberFormatException nfe)
        {
            f = 0.0f;
        }
        return f;
    }

    //----------------------------------------------------
    //
    //-----------------------------------------------------
    @SuppressLint("DefaultLocale")
    static public String FloatToString(float val)
    {
        String str = format("%.1f", val);
        int i = str.lastIndexOf(",");
        if (i != -1)
            str = str.replace(',', '.');
        return str;
    }

    //---------------------------------------------------------
    //
    //---------------------------------------------------------
    public static Object CreateInstance(Class<?> cls, Object... args)
    {

        Object res = null;
        Class<?>[] param = null;
        if (args != null)
        {
            param = new Class<?>[args.length];
            for (int i = 0; i < args.length; ++i)
                param[i] = args[i].getClass();
        }

        try
        {
            Constructor<?> c = cls.getConstructor(param);
            res = c.newInstance(args);
        } catch (NoSuchMethodException | InstantiationException | InvocationTargetException | IllegalAccessException e)
        {
            FileUtils.AddToLog(e);
        }
        return res;
    }

    //---------------------------------------------------------
    //
    //---------------------------------------------------------
    static public CasheFromExplore CreateCache(String key)
    {
        CasheFromExplore res = dataExplore.get(key);
        if (res == null)
        {
            res = new CasheFromExplore();
            res.cache.Init(key);
            dataExplore.put(key, res);
        }
       else
        {
            if (res.cache.path == null) {
                dataExplore.remove(key);
                return CreateCache(key);
            }
        }


        return res;
    }

    //---------------------------------------------------------
    //
    //---------------------------------------------------------
    static public void ClearCache()
    {
        for (String key : dataExplore.keySet())
        {
            CasheFromExplore res = dataExplore.get(key);
            try
            {
                res.cache.close();
            } catch (IOException e)
            {
                FileUtils.AddToLog(e);
            }
            res.result.clear();
        }
        dataExplore.clear();
      /*
        try {
            DeleteFilesDir(new File(cachePath));
        } catch (IOException e) {
            FileUtils.AddToLog(e);
        }
*/
    }

    //---------------------------------------------------------
    //
    //---------------------------------------------------------
    public static File[] DirPicture()
    {
        return new File[]{
                //new File("/storage")};
                //    new File("/sdcard"),
                Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOCUMENTS),
                Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM),
                Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES),
                Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS),
                Environment.getExternalStoragePublicDirectory(Environment.MEDIA_MOUNTED),
                AGlobals.currentActivity.getExternalFilesDir(null)};
    }

    //---------------------------------------------------------
    //
    //---------------------------------------------------------
    public static String[] MaskPicture()
    {
        return new String[]{".jpg", "jpeg"};
    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public static File[] DirDesign()
    {
        String path = AGlobals.contextApp.getApplicationInfo().dataDir;
        path += "/SewNet";
        return new File[]{
                //  new File(path)  };
                Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOCUMENTS),
                Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM),
                Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES),
                Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS),
                Environment.getExternalStoragePublicDirectory(Environment.MEDIA_MOUNTED),
           //     new File(AGlobals.contextApp.getApplicationInfo().dataDir + "/SewNet"),
                AGlobals.currentActivity.getExternalFilesDir(null)};

//                new File("/storage")};
        //  return new File[]{ getExternalStorageDirectory(),
        //        new File("/storage") };

    }

    public static File[] DirFonts()
    {
        return new File[]{
                Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOCUMENTS),
                Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM),
                Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES),
                Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS),
                Environment.getExternalStoragePublicDirectory(Environment.MEDIA_MOUNTED),
                new File("/system/fonts"), new File("/system/font"), new File("/data/fonts")};
    }

    public static String[] MaskFonts()
    {
        return new String[]{".ttf", ".otf"};
    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public static String[] MaskDesign()
    {
        return new String[]{".fhe", ".vip", ".hus", ".che", ".xxx", ".psw", ".pec", ".pes", ".sew", ".exp", ".dst", ".pcs", ".jef", ".shv"};
    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public static String[] MaskWriteDesign()
    {
        return new String[]{"*.fhe", "*.vip", ".hus", "*.xxx", "*.psw", "*.pec", "*.pes", "*.sew", "*.exp", "*.dst", "*.pcs", "*.jef"};
    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public static String CreateHashKey(String str)
    {
        String hashKey;
        try
        {
            final MessageDigest mDigest = MessageDigest.getInstance("MD5");
            mDigest.update(str.getBytes());
            byte b[] = mDigest.digest();
            StringBuilder sb = new StringBuilder();
            for (Byte bb : b)
            {
                String hex = bb.toString();
                sb.append(hex);
            }
            hashKey = sb.toString();

        } catch (NoSuchAlgorithmException e)
        {
            hashKey = String.valueOf(str.hashCode());
        }
        return hashKey;
    }

    //----------------------------------------------------------------------------------
    //
    //----------------------------------------------------------------------------------
    static public void hideFloatingActionButton(FloatingActionButton fab)
    {
        CoordinatorLayout.LayoutParams params = (CoordinatorLayout.LayoutParams) fab.getLayoutParams();
        FloatingActionButton.Behavior behavior = (FloatingActionButton.Behavior) params.getBehavior();
        if (behavior != null)
            behavior.setAutoHideEnabled(false);
        fab.hide();
    }

    //----------------------------------------------------------------------------------
    //
    //----------------------------------------------------------------------------------
    static public void showFloatingActionButton(FloatingActionButton fab)
    {
        fab.show();
        CoordinatorLayout.LayoutParams params = (CoordinatorLayout.LayoutParams) fab.getLayoutParams();
        FloatingActionButton.Behavior behavior = (FloatingActionButton.Behavior) params.getBehavior();

        if (behavior != null)
            behavior.setAutoHideEnabled(true);
    }

    //**************************************************************************************
    public static class CActivityCallback implements Application.ActivityLifecycleCallbacks {
        //------------------------------------------------------------------------------
        //
        //------------------------------------------------------------------------------
        @SuppressLint("SourceLockedOrientationActivity")
        @Override
        public void onActivityCreated(Activity activity, Bundle savedInstanceState)
        {
         /*   activity.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                    WindowManager.LayoutParams.FLAG_FULLSCREEN);
            activity.getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
            activity.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
            //   setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE);
            activity.requestWindowFeature(Window.FEATURE_NO_TITLE);
*/
            currentActivity = activity;
        }

        //------------------------------------------------------------------------------
        //
        //------------------------------------------------------------------------------
        @Override
        public void onActivityStarted(Activity activity)
        {
            currentActivity = activity;
        }

        //------------------------------------------------------------------------------
        //
        //------------------------------------------------------------------------------
        @Override
        public void onActivityResumed(Activity activity)
        {
            currentActivity = activity;
            if (onResumed != null)
                onResumed.run(activity);
            // if (!(AGlobals.currentActivity instanceof ClemNaviActivity))
            // CLEM.Resume();
        }

        //------------------------------------------------------------------------------
        //
        //------------------------------------------------------------------------------
        @Override
        public void onActivityPaused(Activity activity)
        {

        }

        //------------------------------------------------------------------------------
        //
        //------------------------------------------------------------------------------
        @Override
        public void onActivityStopped(Activity activity)
        {

            if (currentActivity != null && currentActivity.equals(activity))
            {
                //CLEM.Pause();
                currentActivity = null;
            }
        }

        //------------------------------------------------------------------------------
        //
        //------------------------------------------------------------------------------
        @Override
        public void onActivitySaveInstanceState(Activity activity, Bundle outState)
        {


        }

        @Override
        public void onActivityDestroyed(Activity activity)
        {
            if (currentActivity != null && currentActivity.equals(activity))
            {
                currentActivity = null;
            }
        }
    }

    ;

    public static void BackTrace()
    {
        BackTrace(null);
    }

    public static void BackTrace(String func)
    {
        if (func == null)
            func = "";
        FileUtils.AddToLog(func);
        StackTraceElement[] elements = Thread.currentThread().getStackTrace();
        for (int i = 1; i < elements.length; ++i)
        {
            StackTraceElement s = elements[i];
            FileUtils.AddToLog(s.getClassName() + "." + s.getMethodName() + "(File :" + s.getFileName() + "Line :" + s.getLineNumber() + ")");
        }
    }

    public static void Sleep(int Mili)
    {
        try
        {
            Thread.sleep(Mili);
        } catch (InterruptedException e)
        {
            e.printStackTrace();
        }
    }
    static public Drawable GetDrawable(String uri, int left, int top, int right, int bottom)
    {
        int imageResource = contextApp.getResources().getIdentifier(uri, null, contextApp.getPackageName());
        Drawable d = ContextCompat.getDrawable(AGlobals.contextApp,imageResource);
        d.setBounds(left, top, right, bottom);
        return d;


    }
    static public VectorDrawableCompat GetDrawable(String uri)
    {
        //String uri = "drawable/logo_proba";
        int imageResource = contextApp.getResources().getIdentifier(uri, null, contextApp.getPackageName());
        VectorDrawableCompat d = VectorDrawableCompat.create(contextApp.getResources(), imageResource, null);
//        VectorDrawable d = (VectorDrawable)ContextCompat.getDrawable(AGlobals.contextApp, imageResource);

        return  d;
    }

    //***********************************************************************************
    static public native boolean SendCommand2Thread(long ID, long cmd, long wparam, long lparam);

    static public native boolean PostCommand2Thread(long ID, long cmd, long wparam, long lparam);

    static public native boolean CallCommand(long ID, long cmd, long wparam, long lparam);

    static public native long AttachThread(long global_obj);

    static public native boolean DetachThread(long id);

    static public native boolean OnThreadLoop(long threadID);

    static public native Object LongToObject(long obj);

    static public native void ReleaseGlobalInstance(long global);

    static public native void AddGlobalIdle(ActionCallback param);

    static public native void ReleaseGlobalIdle();

    static public native String ReadString(String id);
    static public native void SaveIni(String section, String id, String data, String filename);

    static public native String ReadIni(String section, String id, String filename);

    static public native void SaveIniW(String section, String id, String data, String filename);
    static public native String ReadIniW(String section, String id, String filename);

    static public native String ReadLang();

    static public native void WriteLang(String lg);


}
