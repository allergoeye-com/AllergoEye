package lib;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.hardware.SensorManager;
import android.net.wifi.WifiInfo;
import android.os.Build;
import android.os.Bundle;

import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentTransaction;
import androidx.core.content.ContextCompat;
import androidx.appcompat.app.AppCompatActivity;

import android.view.Window;
import android.view.WindowManager;

import com.dryeye.app.R;

public class OrientActivity extends AppCompatActivity {
    public Activity parent = null;
    boolean fStartHideDialog = false;
    public OrientActivity()
    {
    super();
    }
    @SuppressLint("SourceLockedOrientationActivity")
    protected void onCreate(Bundle savedInstanceState)
    {
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O)
        {

            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        }
        else
          setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE);
//setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        super.onCreate(savedInstanceState);


    }

    @Override
    protected void onResume()
    {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O)
        {

            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        }
        else
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE);
        super.onResume();
        OnChangeConnect();

    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    protected final void SetParent(Activity a)
    {
        parent = a;
    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    public void OnMonitor(WifiInfo info)
    {
    }

    public void OnChangeConnect()
    {


    }

    //-------------------------------------------------------
    //
    //-------------------------------------------------------
    public void showFragment(Fragment fragment, int fragment_container, Bundle args)
    {
        if (fragment == null) return;
        FragmentManager fragmentManager = getSupportFragmentManager();
        if (args != null)
            fragment.setArguments(args);
        FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        fragmentTransaction.add(fragment_container, fragment);
        fragmentTransaction.commit();
    }

    public void replaceFragment(int idContainer, Fragment fragment_new)
    {
        FragmentTransaction transaction;
        transaction = getSupportFragmentManager().beginTransaction();
        transaction.setCustomAnimations(R.anim.slide_in_left, R.anim.slide_in_right);

        transaction.replace(idContainer, fragment_new);
        transaction.addToBackStack(null);
        transaction.commit();
    }
    //-------------------------------------------------------
    //
    //-------------------------------------------------------
    public void dismissFragment(Fragment fragment)
    {

        if (fragment == null) return;
        FragmentManager fragmentManager = getSupportFragmentManager();
        FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        fragmentTransaction.remove(fragment);
        fragmentTransaction.commitAllowingStateLoss();
    }

    public boolean TryConnectIsAllowed()
    {
        return false;
    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    public void CloseDialog()
    {
        fStartHideDialog = false;
    }

    public void DialogHide()
    {
        fStartHideDialog = true;
    }

}
