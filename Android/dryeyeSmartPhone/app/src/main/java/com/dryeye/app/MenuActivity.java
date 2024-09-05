package com.dryeye.app;

import static android.view.KeyEvent.ACTION_DOWN;
import static android.view.KeyEvent.KEYCODE_CAMERA;

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;

import androidx.annotation.NonNull;
import androidx.appcompat.app.ActionBar;
import androidx.core.app.ActivityCompat;
import androidx.fragment.app.Fragment;
import androidx.core.content.ContextCompat;

import android.provider.Settings;
import android.util.DisplayMetrics;
import android.util.Log;
import android.util.TypedValue;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.TextView;


import com.dryeye.app.camera.FragmentCamera;
import com.dryeye.app.card.CException;
import com.dryeye.app.card.FragmentCard;
import com.dryeye.app.card.FragmentExploreCards;
import com.dryeye.app.lang.LangDlg;
import com.dryeye.app.timer.TimerButton;
import com.dryeye.app.wifi.FragmentSyncAll;

import lib.camera.CameraCalculatePreview;
import lib.common.CCheckBox;
import lib.common.CComboBox;
import lib.utils.TreeDir;

import com.dryeye.app.wifi.FragmentSyncFile;
import com.dryeye.app.wifi.MdnsUtils;
import com.dryeye.app.wifi.SyncMsgBox;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Locale;

import lib.ExitProgramActivity;
import lib.common.AActivity;
import lib.common.ActionCallback;
import lib.common.CHandler;
import lib.common.IResultFragmentDlg;
import lib.common.button.CMenuButton;
import lib.common.button.CTextButton;
import lib.common.layout.CPanelDialog;
import lib.common.layout.LayoutButton;
import lib.utils.AGlobals;
import lib.utils.FileUtils;

import static lib.common.CDialog.IDOK;
import static lib.common.CDialog.MB_OKCANCEL;


public class MenuActivity  extends AActivity {
    private static boolean fFirstTime = true;
    private Fragment dialog = null;
    private Fragment dialog2 = null;
    private CPanelDialog dialogContainer;
    private CPanelDialog dialogContainer2;
    final int ID_NEW_CARD = 100;
    final int ID_LOAD_CARD = 200;
    final int ID_EXIT = 300;
    final int ID_SYNC = 400;
    final int ID_SETTING = 500;
    ActionCallback onload = null;
    ActionCallback onnewcard = null;
    ActionCallback oncamera = null;
    ActionCallback _onexit = null;
    boolean flagFirsRun = false;

    //----------------------------------------------------
    //
    //----------------------------------------------------
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        if (fFirstTime)
            InitInstance();
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_layout2);
        onexit = MainActivity.mainActivity.menuMainMenu.get("back");
        _onexit = MainActivity.mainActivity.menuMainMenu.get("exit");
        onload = MainActivity.mainActivity.menuMainMenu.get("load");
        onnewcard = MainActivity.mainActivity.menuMainMenu.get("newcard");
        oncamera = MainActivity.mainActivity.menuMainMenu.get("camera");
//        ColorPlane.Init();
        //      CSliderLum.Init();

        LayoutButton button = (LayoutButton)findViewById(R.id.homeNew);
        //-------------------------------------------------------------------------
        //button.setOnClickListener(new COnClickButton(ID_NEW_CARD));
        button.setVisibility(View.GONE);
        //---------------------------------------------------------------
        DisplayMetrics metrics = AGlobals.GetDisplayMetrcs();
        float size = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 1, metrics);

        // LayoutButton button = findViewById(R.id.homeSetting);
        // button.setOnClickListener(new COnClickButton(ID_SETTING));
        CCheckBox c = findViewById(R.id.idChecBox);
        c.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                CameraCalculatePreview.fPirke = isChecked;
                MainActivity.WriteIni("IsPrick", isChecked);

            }
        });

        button = (LayoutButton)findViewById(R.id.homeLoad);
        button.setOnClickListener(new COnClickButton(ID_LOAD_CARD));
        button = (LayoutButton)findViewById(R.id.homeExit);
        button.setOnClickListener(new COnClickButton(ID_EXIT));
        button = (LayoutButton)findViewById(R.id.homeSync);
        button.setOnClickListener(new COnClickButton(ID_SYNC));
        CMenuButton btn = findViewById(R.id.idSettings);
        btn.setOnClickListener(new COnClickButton(ID_SETTING));
        dialogContainer = (CPanelDialog)findViewById(R.id.dialogContainer);
        dialogContainer2 = (CPanelDialog) findViewById(R.id.dialogContainer2);
        CMenuButton tb = findViewById(R.id.idLang);
        tb.Init(this, "OnLang");
        TimerButton b =  findViewById(R.id.idTimer);
        b.SetCallback(App.GetInstance(), "OnTimeButton");
        try {
            PackageInfo info = getPackageManager().getPackageInfo(getPackageName(), 0);
            String ver = "( version " + info.versionName + " )";
            TextView txt = findViewById(R.id.textVersion);
            txt.setText(ver);
        } catch (PackageManager.NameNotFoundException e) {
            throw new RuntimeException(e);
        }


        if (fFirstTime)
        {
            flagFirsRun = !(AGlobals.contextApp !=null && ContextCompat.checkSelfPermission(AGlobals.contextApp, Manifest.permission.WRITE_EXTERNAL_STORAGE ) == PackageManager.PERMISSION_GRANTED);

            fFirstTime= false;

            CheckAppPermissions(this);
        } else
            MdnsUtils.StopMDNS();
        SetLang(AGlobals.ReadLang());
    }
/*
    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        int key = event.getKeyCode();

        switch(key)
        {
            case KeyEvent.KEYCODE_VOLUME_DOWN:
            case KeyEvent.KEYCODE_VOLUME_UP:
            case KeyEvent.KEYCODE_MEDIA_PLAY:
            case KEYCODE_CAMERA:

                //    default:
            {
                if (dialog != null && dialog instanceof FragmentCamera)
                {
                    if (event.getAction() == ACTION_DOWN)
                    {
                        ((FragmentCamera)dialog).OnFoto();
                    }
                    return true;
                }
            }
        }
        return super.dispatchKeyEvent(event);
    }
*/
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        int key = event.getKeyCode();
        switch(key)
        {
            case KeyEvent.KEYCODE_VOLUME_DOWN:
            case KeyEvent.KEYCODE_VOLUME_UP:
            case KEYCODE_CAMERA:
        //    default:
            {
                if (dialog != null && dialog instanceof FragmentCamera)
                {
                    ((FragmentCamera)dialog).onFoto();
                    return true;
                }
            }
        }
        return super.onKeyDown(keyCode, event);
    }

    //-------------------------------------------------------
    //
    //-------------------------------------------------------
    @Override
    public void showFragment(Fragment fragment, int fragment_container, Bundle args)
    {

        if (fragment == null) return;
        LayoutButton button = (LayoutButton) findViewById(R.id.homeNew);
        button.setEnabled(false);

        button = (LayoutButton) findViewById(R.id.homeLoad);
        button.setEnabled(false);
        button = (LayoutButton) findViewById(R.id.homeExit);
        button.setEnabled(false);
        button = (LayoutButton) findViewById(R.id.homeSync);
        button.setEnabled(false);
        CMenuButton btn = findViewById(R.id.idSettings);
        btn.setEnabled(false);
        super.showFragment(fragment, fragment_container, args);

    }

    //-------------------------------------------------------
    //
    //-------------------------------------------------------
    @Override
    public void dismissFragment(Fragment fragment)
    {

        if (fragment == null) return;
        LayoutButton button = (LayoutButton) findViewById(R.id.homeNew);
        button.setEnabled(true);
        button = (LayoutButton) findViewById(R.id.homeLoad);
        button.setEnabled(true);
        button = (LayoutButton) findViewById(R.id.homeExit);
        button.setEnabled(true);
        button = (LayoutButton) findViewById(R.id.homeSync);
        button.setEnabled(true);
        CMenuButton btn = findViewById(R.id.idSettings);
        btn.setEnabled(true);
        super.dismissFragment(fragment);

    }


      //------------------------------------------------------
    //
    //------------------------------------------------------
    public void Exit()
    {
        _onexit.run();
    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    @Override
    public void onBackPressed() {

        if (dialog != null)
            DialogHide();
      /*  else
        {
            CPackageInstaller.Install(new File("/sdcard/Download/allergoeye-arm8-arm64-v8a-debug.apk"));
        }
*/
        //else
        //  moveTaskToBack(true);


    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    @Override
    public void onDestroy() {
        if (dialog != null)
            DialogHide();
        super.onDestroy();

    }
    @Override
    public void finish() {
        if (dialog == null)
            super.finish();
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
    @Override
    protected void onResume()
    {
        super.onResume();
        CCheckBox c = findViewById(R.id.idChecBox);
        CameraCalculatePreview.fPirke = MainActivity.ReadIniBool("IsPrick");
        c.setChecked(CameraCalculatePreview.fPirke);

        DrawText();
        if (FragmentExploreCards.LastPath != null)
        {
            (new CHandler()).Send(new Runnable() {
                @Override
                public void run()
                {
                    OnLoadCardFragment();

                }
            });
        }

    }

    protected void DrawText()
    {
        TextView txt = findViewById(R.id.homeNewString);
        String str = getString(R.string.new_card);
        txt.setText(str);
        txt = findViewById(R.id.homeLoadString);
        str = getString(R.string.card_list);
        if (str != null)
            txt.setText(str);
        txt = findViewById(R.id.homeSyncString);
        str = getString(R.string.card_cync);
        txt.setText(str);
        txt = findViewById(R.id.homeExitString);
        str =  getString(R.string.exit);
        if (str != null)
            txt.setText(str);

    }

    public void OnLang()
    {
        LangDlg dlg = new LangDlg();
        dlg.DoModal();
        SetLang(dlg.Result);

     //   CTextButton tb = findViewById(R.id.idLang);
       // String res;
       // res = (String) tb.getText();
       // SetLang(res);


    }

    public void SetLang(String res)
    {
        AGlobals.WriteLang(res);
        /*
        CTextButton tb = findViewById(R.id.idLang);

        if (res.equals("DE"))
            tb.setText("EN");
        else
            tb.setText("DE");



         */
        Resources resources = getResources();
        DisplayMetrics displayMetrics = resources.getDisplayMetrics();
        Configuration configuration = resources.getConfiguration();
        configuration.setLocale(new Locale(res.toLowerCase()));
        resources.updateConfiguration(configuration, displayMetrics);
        configuration.locale = new Locale(res.toLowerCase());
        resources.updateConfiguration(configuration, displayMetrics);
        DrawText();

    }

    //------------------------------------------------
    //
    //------------------------------------------------
    private void CheckAppPermissions(Context context) {

        final String[] permissions = GetManifestPermissions(this);
     /*   if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            if (!getPackageManager().canRequestPackageInstalls()) {
                startActivityForResult(new Intent(Settings.ACTION_MANAGE_UNKNOWN_APP_SOURCES).setData(Uri.parse(String.format("package:%s", getPackageName()))), 1234);
            }
        }
        */

        for (String permission : permissions)
        {
            int res = ContextCompat.checkSelfPermission(this, permission);

            if (res != PackageManager.PERMISSION_GRANTED)
            {

                //Нужно ли нам показывать объяснения , зачем нам нужно это разрешение
                if (ActivityCompat.shouldShowRequestPermissionRationale(this, permission))
                {
                    ; //показываем объяснение
                }
                else
                {
                    ; //просим разрешение

                }
            }
        }
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(permissions, 1000);
        }
    }
    //-----------------------------------------------------------
    // запрос какие есть разрешния в манифесте
    //-----------------------------------------------------------
    private static String[] GetManifestPermissions(Context context)
    {
        try {
            PackageInfo info = context.getPackageManager().getPackageInfo(context.getPackageName(), PackageManager.GET_PERMISSIONS);
            if (info.requestedPermissions != null)
                return info.requestedPermissions;

        } catch (Exception e) {
            FileUtils.AddToLog(e);
        }
        return new String[0];
    }
    //---------------------------------------------
    // ответ дано ли разрешение
    //----------------------------------------------
    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults)
    {
        switch (requestCode)
        {
            case 1000:
            {
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED)
                {
                    try
                    {
                        if (flagFirsRun)
                        {
                            File dir = getExternalCacheDir();
                            File dir2 = getExternalFilesDir(null);


                            if (dir != null)
                            {
                                FileUtils.Clear(dir);
                            }
                            if (dir2 != null)
                            {
                                FileUtils.Clear(dir2);
                            }
                        }

                    } catch (Exception e)
                    {
                        e.printStackTrace();
                    }

                    Calendar c = Calendar.getInstance();
                    SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH-mm-ss");
                    FileUtils.AddToLog("===================================");
                    String manufacturer = Build.MANUFACTURER;
                    String board = Build.BOARD;
                    String model = Build.MODEL;
                    String brand = Build.BRAND;
                    String device = Build.DEVICE;
                    int version = Build.VERSION.SDK_INT;
                    String versionRelease = Build.VERSION.RELEASE;

                    FileUtils.AddToLog("manufacturer " + manufacturer
                            + " \n brand " + brand
                            + " \n board " + board
                            + " \n device " + device
                            + " \n model " + model
                            + " \n version " + version
                            + " \n versionRelease " + versionRelease
                    );
                    String sname = sdf.format(c.getTime());
                    FileUtils.AddToLog(sname);
                    FileUtils.AddToLog("User has permitted access");
                    ArrayList<String> dirs = new ArrayList<String>();
                    FileUtils.GetDirList(App.defaultPath, dirs);
                    for (String s : dirs)
                    {
                        String s1 = s.trim();
                        if (!s1.equals(s))
                            (new File(s)).renameTo(new File(s1));
                    }
                    dirs.clear();
                    FileUtils.GetDirList(App.defaultPath, dirs);
                    for (String s : dirs)
                    {
                        String s1 = s.trim();
                        if (!s1.equals(s))
                            Log.e("FILE ERROR", s);
                    }
                    File test = new File(App.defaultPath, "progini.ini");
                    if (!test.exists())
                    {
                        FileUtils.ExtractAssetsFile(App.defaultPath, "progini.ini");
                    }

                    MdnsUtils.Run();
                } else
                {
                    FileUtils.AddToLog("Use has declined access");
                    ExitProgramActivity.exitApplication(this);
                }
                return;
            }
        }
    }
    class COnClickButton implements View.OnClickListener {
        int id;
        public COnClickButton (int ID)
        {
            id = ID;

        }
        @Override
        public void onClick(View v)
        {
            switch(id)
            {
                case ID_NEW_CARD:
                    App.cardPath.Reset();
                    OnOpenNewCardFragment();
                    break;
                case ID_LOAD_CARD:
//                    onexit = onload;
  //                  finish();
                    OnLoadCardFragment();
                    break;
                case  ID_EXIT:
                    Exit();
                    break;
                case ID_SYNC:
                {
                    OnSyncFragment();
                    //DialogCameraInfo n = new DialogCameraInfo();
                    //n.DoModal();

                }
                    break;
                case ID_SETTING:
                {
                /*    if (false)
                    {
                            try
                            {

                            Intent intent = new Intent(App.instance, CInstallApk.class);
                            intent.setFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT);
                                intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                             MainActivity.mainActivity.startActivity(intent);
                            }catch(Exception e)
                            {
                                FileUtils.AddToLog(e);
                            }

*/
                   /*     Intent launchIntent = getPackageManager().getLaunchIntentForPackage("com.dryeye.installer");
                        if (launchIntent != null)
                        {
                            launchIntent.putExtra("PathNewApk", Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM).getAbsoluteFile() + "/" + "allergoeye-arm8-arm64-v8a-debug.apk");

                            startActivity(launchIntent);
                            Exit();
                        }
                        else
                        Toast.makeText(AGlobals.currentActivity, "There is no package available in android", Toast.LENGTH_LONG).show();
*/
              //          break;

                //    }
                  //  else
                           MdnsUtils.MDNSChangeHostName();
                            // DlgOptions dlg = new DlgOptions(AGlobals.currentActivity);
                            //if (dlg.DoModal() == IDOK)
                            //MdnsUtils.MDNSChangeOptions();


                }
                break;

            }

        }
    }
    public void OnLoadCardFragment()
    {
        if (dialog != null)
            return;
        //----------------------------------------------
        //
        //----------------------------------------------
        ArrayList<String> fdir = new ArrayList<String>();
        FileUtils.GetDirList(new File(App.defaultPath), fdir);
        if (fdir.size() > 0)
        {
            MdnsUtils.MDNSPause();
            showFragment(dialog = new FragmentExploreCards(onload, new ActionCallback(this, "OnAddTestFromCard")), R.id.dialogContainer,null);
        }
    }
    public void OnAddTestFromCard()
    {
      /*  if (dialog instanceof FragmentExploreCards)
        {
            FragmentTransaction transaction;
            transaction = getSupportFragmentManager().beginTransaction();
            //dismissFragment(dialog);
            //showFragment(dialog = new FragmentCamera(oncamera, false), R.id.dialogContainer,null);
            transaction.setTransition(FragmentTransaction.TRANSIT_FRAGMENT_OPEN);
            transaction.setCustomAnimations(R.anim.slide_in_left1, R.anim.slide_in_right);

            transaction.replace(R.id.dialogContainer, dialog = new FragmentCamera(oncamera, false));
            transaction.addToBackStack(null);
            transaction.commit();
            return;

        }
        */

    }
    public void OnSyncFragment()
    {
        if (dialog != null)
            return;

        showFragment(dialog = new FragmentSyncAll(null), R.id.dialogContainer,null);
    }
    public void OnSyncDir(ArrayList <String> list, ArrayList <String> _roi)
    {

        showFragment(dialog2 = new FragmentSyncFile(list, _roi, null), R.id.dialogContainer2,null);
    }

    public boolean OnSyncGet(String pat)
    {
        if (dialog != null)
        {
            if (dialog instanceof FragmentSyncAll)
            {
                return true;
            } else
            {
                int fRes;
                if (SyncMsgBox.onFinish)
                {
                    SyncMsgBox box = new SyncMsgBox("Server", "Synchronize server data ?", MB_OKCANCEL);
                    fRes = box.DoModal();
                    if (fRes == IDOK)
                    {
                        DialogHide();
                        return true;
                    }
                }
                return false;
            }
        }

        showFragment(dialog = new FragmentSyncAll(pat, null), R.id.dialogContainer,null);
        return true;
    }

    public void OnOpenNewCardFragment()
    {
        if (dialog != null)
            return;
        MdnsUtils.MDNSPause();
        showFragment(dialog = new FragmentCard(onnewcard), R.id.dialogContainer, null);
    }

    public void OnEditCardFragment(TreeDir.DirNodeVisible node, ActionCallback _onexit)
    {
        if (dialog == null)
            return;
        MdnsUtils.MDNSPause();
        showFragment(dialog2 = new FragmentCard(node, _onexit), R.id.dialogContainer2, null);
    }
    public void OnOpenCameraFragment(boolean fAnimate)
    {
        if (dialog != null)
            return;

      //  MdnsUtils.MDNSPause();
        showFragment(dialog = new FragmentCamera(oncamera, fAnimate), R.id.dialogContainer,null);
    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    @Override
    public boolean dispatchTouchEvent(MotionEvent ev)
    {
     /*   if (dialog != null && dialog instanceof FragmentCamera)
            return ((FragmentCamera) dialog).dispatchTouchEvent(ev);

      */
        return super.dispatchTouchEvent(ev);
    }
    //----------------------------------------
    //
    //----------------------------------------
    @Override
    public void DialogHide()
    {
        if (dialog2 != null)
        {
            dialogContainer2.Hide();
            return;
        }
        if (dialog == null)
            return;
        MdnsUtils.MDNSPause();
        super.DialogHide();
        ActionCallback result = null;
        if (dialog instanceof IResultFragmentDlg)
            result = (ActionCallback)((IResultFragmentDlg)dialog).GetResult();
        if (result != null)
        {
            if ((dialog instanceof FragmentExploreCards && ((FragmentExploreCards) dialog).pathJPG != null) || dialog instanceof FragmentCamera)
            {

                dialog = null;
                onexit = result;
                finish();
                return;
            }
        }

        dialogContainer.Hide();

    }
    @Override
    public void CloseDialog()
    {
        if (dialog2 != null)
        {

            dismissFragment(dialog2);
            dialog2 = null;
            MdnsUtils.StopMDNS();
            return;
        }
        ActionCallback result = null;
        if (dialog == null)
            return;
        (new Handler()).postDelayed(new Runnable() {
            @Override
            public void run()
            {
                MdnsUtils.StopMDNS();
            }
        }, 100);
        if (dialog instanceof IResultFragmentDlg)
            result = (ActionCallback)((IResultFragmentDlg)dialog).GetResult();
        try {
            dismissFragment(dialog);
        }catch (Exception e)
        {
            dialog = null;
            super.CloseDialog();
            MdnsUtils.StopMDNS();
            return;

        }
        if (result != null)
        {
            if (dialog instanceof FragmentExploreCards && ((FragmentExploreCards) dialog).pathJPG == null)
            {
                dialog = null;
                super.CloseDialog();


                (new Handler()).post(new Runnable() {
                    @Override
                    public void run()
                    {
                        OnOpenCameraFragment(true);
                    }
                });

                return;
            }


            if (dialog instanceof  FragmentCard)
            {
                dialog = null;
                super.CloseDialog();
                MdnsUtils.StopMDNS();
               (new Handler()).postDelayed(new Runnable() {
                                                @Override
                                                public void run()
                                                {
                                                    try
                                                    {
                                                        if (App.cardPath.CreateNewTest())
                                                            OnOpenCameraFragment(true);
                                                    } catch (CException e)
                                                    {
                                                        e.printStackTrace();
                                                    }
                                                }
                }, 100);
                return;
            }
             else
            {
                (new Handler()).postDelayed(result, 100);
            }
        }

        dialog = null;
        super.CloseDialog();
        MdnsUtils.StopMDNS();

    }

    native private void InitInstance();
}
