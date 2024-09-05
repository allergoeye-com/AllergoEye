package com.dryeye.app;

import static android.view.KeyEvent.KEYCODE_CAMERA;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.drawable.ColorDrawable;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;

import androidx.fragment.app.Fragment;
import android.util.DisplayMetrics;
import android.util.TypedValue;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.dryeye.app.camera.FragmentCamera;
import com.dryeye.app.card.CException;
import com.dryeye.app.card.FraggmentCardImages;
import com.dryeye.app.card.FragmentCard;
import com.dryeye.app.dlgvect.DlgPlukVect;
import com.dryeye.app.roi.FragmentROI;
import com.dryeye.app.timer.TimerButton;
import com.dryeye.app.wifi.FragmentSyncFile;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.OutputStream;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashMap;

import lib.common.AActivity;
import lib.common.ActionCallback;
import lib.common.CHandler;
import lib.common.CWnd;
import lib.common.IResultFragmentDlg;
import lib.common.MessageBox;
import lib.common.layout.CPanelDialog;
import lib.common.layout.LayoutButton;
import lib.utils.AGlobals;
import lib.utils.BitmapUtils;
import lib.utils.FileUtils;

import static lib.common.CDialog.IDCANCEL;
import static lib.common.CDialog.IDOK;
import static lib.common.CDialog.MB_OKCANCEL;

public class ViewActivity extends AActivity {
    private static boolean fFirstTime = true;
    public CView view;
    private Fragment dialog = null;
    private CPanelDialog dialogContainer;
    CViewLayout  viewContainer;
    Uri uri = null;
    String pathImage = null;
    Bitmap bitmap;
    boolean fInit = false;
    String roi = null;
    boolean fRoiView = false;
    //----------------------------------------------------
    //
    //----------------------------------------------------
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        bitmap = null;
        uri = getIntent().getData();
        setContentView(R.layout.activity_view2);
     //   Toolbar toolbar = findViewById(R.id.toolbar);
       // setSupportActionBar(toolbar);
        view = (CView) CWnd.CreateInstance(CView.class, this);
        view.Init();
        viewContainer = findViewById(R.id.viewContaner);

        viewContainer.setBackground(new ColorDrawable(android.graphics.Color.TRANSPARENT));
        viewContainer.addView(view, new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT,LinearLayout.LayoutParams.MATCH_PARENT));
        dialogContainer = (CPanelDialog)findViewById(R.id.dialogContainer);
        viewContainer.CalbackOnLayout = new ActionCallback(this, "OnViewLayout", boolean.class, int.class, int.class, int.class, int.class);

    }
    public void OnViewLayout(boolean changed, int left, int top, int right, int bottom)
    {

        if (right - left > 0 && bottom - top > 0)
        {
            if (view != null)
            {
                (new CHandler()).Send(new Runnable() {
                    @Override
                    public void run()
                    {
                        view.setSize(right - left, bottom - top);
                        view.Invalidate();


                    }
                });
            }
        }
    }
    public void OnUpdateInfo()
    {
        (new Handler()).postDelayed(new Runnable() {
            @Override
            public void run()
            {

                TextView text = findViewById(R.id.idNameCard);
                TextView text1 = findViewById(R.id.idResult);
                text1.setText(" ");
                text.setText(" ");
                try {

                    String[] info = App.cardPath.ReadCardInfo();
                    String s = info[0] + " " + info[1] + " " + info[2];
                menuMainMenu = new HashMap<String, ActionCallback>();
                text.setText(s);
                text = findViewById(R.id.idResult);
                if (pathImage != null)
                {
                    String filename = pathImage.replace(".jpg", ".res");
                    ArrayList<String> t = new ArrayList<String> ();
                    FileUtils.ReadText(filename, t);
                    text1.setText(t.size() > 0 ? t.get(0) : " ");

                }
                else
                    text1.setText(" ");
                }catch (Exception e)

                {
                    text1.setText(" ");
                    text.setText(" ");
                }
            }
        }, 200);

    }
    //----------------------------------------------------
    //
    //----------------------------------------------------
    public void OnInfo()
    {
        if (dialog != null)
            return;

        showFragment(dialog = new FragmentCard(null), R.id.dialogContainer, null);
        //showFragment(dialog = new DlgPlukVect(null), R.id.dialogContainer, null);


    }
    public void OnImages()
    {
        if (dialog != null)
            return;
        try
        {
            showFragment(dialog = new FraggmentCardImages(App.cardPath.Dir(), new ActionCallback(this, "LoadFile", ViewActivity.class, String.class)), R.id.dialogContainer, null);
        } catch (CException e)
        {
            e.printStackTrace();
            dialog = null;
        }
    }
    //----------------------------------------
    //
    //----------------------------------------
    @Override
    public void DialogHide()
    {
        Runnable result = null;

        if (dialog == null)
            return;
        try {
            super.DialogHide();

            if (dialog instanceof IResultFragmentDlg) {
                result = (ActionCallback) ((IResultFragmentDlg) dialog).GetResult();
            }
        if (result != null)
        {
            if (dialog instanceof FragmentCamera)
            {
                result.run();
            }
        }
        }
        catch(Exception e)
        {
        }
        dialogContainer.Hide();
        OnUpdateInfo();


    }
    public void OnNewCard(ViewActivity ac, String s)
    {

    }

    @Override
    public void CloseDialog()
    {
        Runnable result = null;
        if (dialog == null)
            return;
        Runnable res = null;
        if (dialog instanceof IResultFragmentDlg && !(dialog instanceof  FragmentCamera))
            res = ((IResultFragmentDlg)dialog).GetResult();
        try {
            dismissFragment(dialog);
            dialog = null;
            super.CloseDialog();
        }catch (Exception e)
        {
            dialog = null;
            super.CloseDialog();

        }
        if (res != null)
        {
            (new Handler()).postDelayed(res, 100);
        }

    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    public void OnCamera()
    {
/*        intent.setFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT);
        startActivityForResult(intent, 100);*/
        if (dialog != null)
            return;

        showFragment(dialog = new FragmentCamera(new ActionCallback(this, "LoadBitmap", Bitmap.class)), R.id.dialogContainer,null);

    }
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        int key = event.getKeyCode();
        switch(key)
        {
            case KeyEvent.KEYCODE_VOLUME_DOWN:
            case KeyEvent.KEYCODE_VOLUME_UP:
            case KEYCODE_CAMERA:
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

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        if (requestCode == 100)
        {
            if (resultCode == RESULT_OK)
            {
                uri = data.getData();
                String res = data.getData().toString();
                try {

                    Bitmap bitmap = BitmapUtils.Load(res, 1);
                   // FileUtils.DeleteFile(res);
                    view.OnOpenDocument(bitmap);
                    view.Invalidate();

                } catch (Exception e) {
                    e.printStackTrace();
                }

            }
        }
        super.onActivityResult(requestCode, resultCode, data);
    }
    //----------------------------------------------------
    //
    //----------------------------------------------------
    public void OnSave()
    {
//        Intent intent = new Intent(this, ExplorerActivity.class);
  //      intent.setFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT);
    //    startActivityForResult(intent, 100);

        if (bitmap != null)
        {

                SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH-mm-ss");
                Calendar c = Calendar.getInstance();
                String sname = sdf.format(c.getTime());
            try
            {
                pathImage = App.cardPath.Dir() + "/" + sname + ".jpg";
            } catch (CException e)
            {
                e.printStackTrace();
            }
            try
            {
                        OutputStream stream = null;
                        File pictureFile = new File(pathImage);
                        try {
                            stream = AGlobals.contentResolver.openOutputStream(Uri.fromFile(pictureFile));
                            bitmap.compress(Bitmap.CompressFormat.JPEG, 100, stream);
                            stream.close();

                        } catch (FileNotFoundException e) {
                            FileUtils.AddToLog(e);
                        }
                        catch (IOException e) {
                            FileUtils.AddToLog(e);
                        }

//                        BitmapUtils.Save(pathImage, bitmap);
                    bitmap.recycle();
                    bitmap = null;
                    _InitButtonRoi();
                } catch (Exception e) {
                    e.printStackTrace();
                }

        }
        LayoutButton b;
        b = findViewById(R.id.idDelete);
        b.Enable(pathImage != null);



    //  finish();

    }

    public void OnDelete()
    {
        if (pathImage != null)
        {
            String str = getString(R.string.ids_delete_image);


           MessageBox box = new MessageBox(str, null, MB_OKCANCEL);
            if (box.DoModal() == IDCANCEL) return;

            File f = new File(pathImage);
            if (f.exists())
                f.delete();
            f = new File(pathImage.replace(".jpg", ".roi"));
            if (f.exists())
                f.delete();
            f = new File(pathImage.replace(".jpg", ".res"));
            if (f.exists())
                f.delete();
            roi = null;
            pathImage = null;
            bitmap = null;
            view.OnOpenDocument(bitmap);
            view.Invalidate();


        }

    }
    //----------------------------------------------------
    //
    //----------------------------------------------------
    @Override
    public boolean dispatchTouchEvent(MotionEvent ev)
    {
      /*  if (dialog != null && dialog instanceof FragmentCamera)
            return ((FragmentCamera)dialog).dispatchTouchEvent(ev);

       */
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
        LayoutButton button;
        super.onResume();
        if (!fInit)
        {
            fInit = true;
            button = findViewById(R.id.idCamera);
            button.Init();
            button.SetCallback(this, "OnCamera");
            button = findViewById(R.id.idInfo);
            button.Init();
            button.SetCallback(this, "OnInfo");
            button = findViewById(R.id.idListPhoto);
            button.Init();
            button.SetCallback(this, "OnImages");
            button = findViewById(R.id.idExit);
            button.Init();
            button.SetCallback(this, "onBackPressed");
            button = findViewById(R.id.idDelete);
            button.Init();
            button.SetCallback(this, "OnDelete");
            button.Enable(pathImage != null);
            button =  findViewById(R.id.idTimerBtn);
            button.Enable(true);
            button.SetCallback(App.GetInstance(), "OnTimeButton");

        }
        TextView txt = findViewById(R.id.idExitText);
        String str = getString(R.string.view_done);
        txt.setText(str);
        txt = findViewById(R.id.idCameraText);
        str = getString(R.string.photo_over);
        txt.setText(str);

        /*
        String str = AGlobals.ReadString("IDS_EXIT");
        if (str != null)
            txt.setText(str);
        txt = findViewById(R.id.idCameraText);
        str = AGlobals.ReadString("IDS_CAMERA");
        if (str != null)
            txt.setText(str);


        txt = findViewById(R.id.idDeleteText);
        str = AGlobals.ReadString("IDS_DELETE");
        if (str != null)
            txt.setText(str);

        txt = findViewById(R.id.idInfoText);
        str = AGlobals.ReadString("IDS_PATIENT");
        if (str != null)
            txt.setText(str);

        txt = findViewById(R.id.idListPhotoText);
        str = AGlobals.ReadString("IDS_PHOTO");
        if (str != null)
            txt.setText(str);
*/
        roi = null;
        if (uri != null)
        {
            String res = uri.toString();
            LoadFile(this, res);
            uri = null;
        }
        InitButtonRoi();
    //    button.Enable(roi != null);

        OnUpdateInfo();
        (new CHandler()).Send(new Runnable() {
                                  @Override
                                  public void run() {
                                      LinearLayout l = findViewById(R.id.viewMenuButton);
                                      l.requestLayout();
                                      viewContainer.requestLayout();

                                  }
                              }, 10000);

    }
    void _InitButtonRoi()
    {

        LayoutButton button = findViewById(R.id.idRoi);
        button.Init();

        if (roi != null)
        {
            button.SetText("ROI");

            button.Button().SetPngColors(true);
            button.SetImage(R.drawable.view);
            button.SetCallback(this, "OnRoi");
        }
        else
        {
            button.SetText("Sync");
            button.Button().SetPngColors(false);
            button.SetImage(R.drawable.synchronizec);
            button.SetCallback(this, "OnSync");

        }
        button.Enable(pathImage != null);

    }
    void InitButtonRoi()
    {

        _InitButtonRoi();
        OnUpdateInfo();
    }

    public void LoadBitmap(Bitmap bmp)
    {
        roi = null;
        pathImage = null;
        bitmap = bmp;
        _InitButtonRoi();
        if (bitmap != null)
        {
            view.OnOpenDocument(bitmap);
            OnSave();
            view.Invalidate();
        }
        OnUpdateInfo();
    }

    public void LoadFile(ViewActivity v, String res)
    {
        try {
            File f = new File(res);
            if (f.exists() && !f.isDirectory())
            {
                Bitmap bitmap = BitmapUtils.Load(res, 1);
                roi = res.replace(".jpg", ".roi");
                File file = new File(roi);
                view.OnOpenDocument(bitmap);
                if (!file.exists())
                    roi = null;
               else
                    view.OnOpenRoi(roi, fRoiView = true);
                view.Invalidate();
                pathImage = res;
                bitmap.recycle();
                bitmap = null;
                InitButtonRoi();

            }

        } catch (Exception e) {
            e.printStackTrace();
        }
        if (fInit)
        {
            LayoutButton b = findViewById(R.id.idDelete);
            b.Enable(pathImage != null);

        }
    }
    public void OnNewRoi()
    {
        roi = pathImage.replace(".jpg", ".roi");

        File file = new File(roi);
       if (!file.exists())
            roi = null;
       else
               view.OnOpenRoi(roi, fRoiView = true);
        InitButtonRoi();
        view.Invalidate();
    }
    public void OnRoi()
    {
        if (dialog != null) return;
        DisplayMetrics metrics = AGlobals.GetDisplayMetrcs();
        float size = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 120, metrics);
        showFragment(dialog = new FragmentROI(view, view.GetRoi((int) metrics.widthPixels,  (int)size)), R.id.dialogContainer,null);

    }
    public boolean IsFragmentSyncFile() { return dialog != null && dialog instanceof FragmentSyncFile; }
        public void OnSync()
    {
        if (dialog != null) return;

        if (bitmap != null)
            OnSave();
        ArrayList< String > list = new ArrayList< String > ();
        ArrayList< String > _roi = new ArrayList< String > ();
        try
        {
            list.add(App.cardPath.RootDir() + "/info.ini");
        } catch (CException e)
        {
            e.printStackTrace();
        }
        String jpg = pathImage.substring(App.defaultPath.length(), pathImage.length());
        list.add(jpg);

        _roi.add(jpg.replace(".jpg", ".roi"));

        showFragment(dialog = new FragmentSyncFile(list, _roi, new ActionCallback(this, "OnNewRoi")), R.id.dialogContainer,null);


/*



        fRoiView = !fRoiView;
        view.OnOpenRoi(roi, fRoiView);
        view.Invalidate();

 */
    }

  void ExitDlg ()
    {
        try {
            DlgExit dlg = new DlgExit(this);
            int res = dlg.DoModal();
            if (res == IDCANCEL)
                return;
            if (res ==  IDOK)
            {
                OnSave();
            }
            AGlobals.rootHandler.Send(new Runnable() {
                @Override
                public void run() {
                    OnBackPressed();
                }
            }, 500);

        }catch (Exception e)
        {
            FileUtils.AddToLog(e);
            return;
        }

    }
    @Override
    public void onBackPressed() {
        if (dialog != null)
        {
            DialogHide();
            return;
        }
        if (bitmap != null)
        {
            AGlobals.rootHandler.Send(new Runnable() {
                @Override
                public void run() {
                    ExitDlg ();
                }
            }, 500);
        }
        else
            AGlobals.rootHandler.Send(new Runnable() {
                @Override
                public void run() {
                    OnBackPressed();
                }
            }, 500);

    }
    public void OnBackPressed()
    {
        finish();
    }
}