package com.dryeye.app.camera;

import static android.content.Context.AUDIO_SERVICE;

import static androidx.core.content.PackageManagerCompat.LOG_TAG;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.PendingIntent;
import android.bluetooth.BluetoothDevice;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Bitmap;
import android.graphics.PorterDuff;
import android.graphics.Rect;
import android.graphics.Typeface;
import android.hardware.camera2.CameraAccessException;
import android.media.AudioManager;
import android.media.session.MediaSession;
import android.media.session.PlaybackState;
import android.os.Bundle;
import android.os.Handler;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import android.support.v4.media.MediaBrowserCompat;
import android.support.v4.media.session.MediaControllerCompat;
import android.support.v4.media.session.MediaSessionCompat;
import android.support.v4.media.session.PlaybackStateCompat;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.dryeye.app.App;
import com.dryeye.app.CViewLayout;
import com.dryeye.app.R;
import com.dryeye.app.ShutterButtonReceiver;
import com.dryeye.app.timer.TimerButton;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Set;

import lib.OrientActivity;
import lib.camera.CCamera;
import lib.camera.CCameraView;
import lib.camera.CameraCalculatePreview;
import lib.common.ActionCallback;
import lib.common.CHandler;
import lib.common.IResultFragmentDlg;
import lib.common.button.CMenuButton;
import lib.common.button.CSpinButton;
import lib.common.grid.Cell;
import lib.common.grid.Grid;
import lib.common.grid.GridRow;
import lib.common.layout.CPanelDialog;
import lib.explorer.common.DirEntry;
import lib.utils.AGlobals;
import lib.utils.FileUtils;

import static java.lang.String.format;
import static lib.camera.CameraCalculatePreview.fPirke;
import static lib.common.grid.Cell.CELL_TEXT;
import static lib.common.grid.Cell.TEXT_BOLD;
import static lib.common.grid.Cell.TEXT_READONLY;
import static lib.utils.FileUtils.AddToLog;


@SuppressLint("ValidFragment")
public class FragmentCamera extends Fragment implements IResultFragmentDlg {
    View self;
    ActionCallback onexit;
    Runnable runnable = null;
    CPanelDialog test;
    boolean fAnimate = true;
    TextView zoomText;
    CCameraView drawPtreview;
    CPanelDialog menu;
    Grid grid;
    private MediaSession mMediaSession = null;
    CSpinButton[] repeat;
    HashMap<Object, CCamera.MapModeKey> modes;

    public FragmentCamera()
    {
        super();
    }
    public FragmentCamera(ActionCallback _onexit, boolean _fAnimate)
    {
        super();
        onexit = _onexit;
        fAnimate = _fAnimate;
    }

    public FragmentCamera(ActionCallback _onexit)
    {
        super();
        onexit = _onexit;
    }
    private static final String MEDIA_SESSION_TAG = "A proba shutter";

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
    {
        try {
            test = ((CPanelDialog)container);
            test.SetOrient("Right");
            if (fAnimate)
            {
                test.DisableRequestLayout(true);
                test.fAutoClosed = false;
            }
            test.Reset();

            test.SetCallbacks(new ActionCallback(this, "OnInitDialog"), new ActionCallback(this, "OnDestroyDialog"));
            self = inflater.inflate(fPirke ? R.layout.activity_camera_pirke : R.layout.activity_camera, container, false);
            drawPtreview = self.findViewById(R.id.drawPtreview);
            drawPtreview.callbackOnScale = new ActionCallback(this, "OnZoom", float.class);
            drawPtreview.fromUpdate = self.findViewById(R.id.drawView);
            drawPtreview.callbackOnPreviewOk = new ActionCallback(this, "OnCameraState", int.class);
            drawPtreview.wnd = this;
            DisplayMetrics metrics = AGlobals.GetDisplayMetrcs();
            self.setLayoutParams(new LinearLayout.LayoutParams(metrics.widthPixels, metrics.heightPixels));
            self.measure(0, 0);
                container.setTranslationY(0);
                container.setTranslationX(0);
            if (!fAnimate)
            {
                test.SetScreenVisible(true,true);
                test.fAutoClosed = true;
            }
            repeat = new CSpinButton[2];

            repeat[0] = (CSpinButton) self.findViewById(R.id.zoomPlus);
            repeat[1] = (CSpinButton) self.findViewById(R.id.zoomMinus);
            zoomText =  self.findViewById(R.id.zoomText);
            CMenuButton bt = self.findViewById(R.id.idFoto);
            if (!fPirke)
            {
               TimerButton b =  self.findViewById(R.id.idTimer);
                b.SetCallback(App.GetInstance(), "OnTimeButton");
                bt.SetCallback(this, "OnSetting");

            }


            menu = self.findViewById(R.id.cameraPanel);
            grid = menu.findViewById(R.id.idGridMenu);
            menu.setVisibility(View.GONE);
            menu.fDetachOld = false;
            bt = self.findViewById(R.id.advanced_ok);
            bt.SetCallback(this, "OnOkMenu");
            bt = self.findViewById(R.id.advanced_cancel);
            bt.SetCallback(this, "OnCancelMenu");
            bt = self.findViewById(R.id.cameraSetting);
       /*     mMediaSession = new MediaSessionCompat(getContext().getApplicationContext(), MEDIA_SESSION_TAG);
            mMediaSession.setFlags(MediaSessionCompat.FLAG_HANDLES_MEDIA_BUTTONS);
         //   Intent mediaButtonIntent = new Intent(Intent.ACTION_MEDIA_BUTTON, null, getContext(), ShutterButtonReceiver.class);
           // mMediaSession.setMediaButtonReceiver(PendingIntent.getBroadcast(getContext(), 0, mediaButtonIntent, 0));

            mMediaSession.setCallback(new MediaSessionCompat.Callback() {
                                          @Override
                                          public boolean onMediaButtonEvent(Intent mediaButtonEvent) {
                                              return super.onMediaButtonEvent(mediaButtonEvent);
                                          }
                                      });
            PlaybackStateCompat state = new PlaybackStateCompat.Builder()
                    .setActions(PlaybackStateCompat.ACTION_PLAY_PAUSE)
                    .setState(PlaybackStateCompat.STATE_PLAYING, 0, 0, 0)
                    .build();
            mMediaSession.setPlaybackState(state);

//            mMediaSession.setFlags(MediaSessionCompat.FLAG_HANDLES_MEDIA_BUTTONS | MediaSessionCompat.FLAG_HANDLES_TRANSPORT_CONTROLS);

            mMediaSession.setActive(true);
*/
          /*  PlaybackState state = new PlaybackState.Builder()
                    .setActions(PlaybackState.ACTION_PLAY_PAUSE)
                    .setState(PlaybackState.STATE_PLAYING, 0, 0, 0)
                    .build();
            audioSession.setPlaybackState(state);

            audioSession.setFlags(MediaSession.FLAG_HANDLES_MEDIA_BUTTONS | MediaSession.FLAG_HANDLES_TRANSPORT_CONTROLS);

            audioSession.setActive(true);

           */
/*
            ShutterButtonReceiver shutterButtonReceiver = new ShutterButtonReceiver();
            IntentFilter a_filter = new IntentFilter(BluetoothDevice.ACTION_ACL_CONNECTED);
            getContext().registerReceiver(shutterButtonReceiver, a_filter);
            IntentFilter b_filter = new IntentFilter(Intent.ACTION_MEDIA_BUTTON);
            getContext().registerReceiver(shutterButtonReceiver, b_filter);
*/

         /*   AudioManager mAudioManager = (AudioManager) getContext().getSystemService(Context.AUDIO_SERVICE);
            ComponentName mRemoteControlResponder = new ComponentName(getContext().getPackageName(),
                    ShutterButtonReceiver.class.getName());
            mAudioManager.registerMediaButtonEventReceiver(
                    mRemoteControlResponder);

          */
     /*      mMediaSession = new MediaSession(getActivity(), MEDIA_SESSION_TAG);
            mMediaSession.setCallback(new MediaSession.Callback() {
                @Override
                public boolean onMediaButtonEvent(Intent mediaButtonIntent) {
                    Toast.makeText(getActivity(), "debug media button test", Toast.LENGTH_LONG).show();

                    return true;
                }
            });
            mMediaSession.setFlags(MediaSession.FLAG_HANDLES_MEDIA_BUTTONS | MediaSession.FLAG_HANDLES_TRANSPORT_CONTROLS);
            mMediaSession.setActive(true);
*/

        }
        catch(Exception e)
        {
            AddToLog(e);
        }
        return self;
    }
    public void OnDestroyMenu()
    {
            grid.Clear();
            menu.setVisibility(View.GONE);
        if (!fPirke)
        {

            repeat[0].setVisibility(View.VISIBLE);
            repeat[1].setVisibility(View.VISIBLE);
        }

    }
    public void OnInitMenu()
    {
    }
    public void OnOkMenu()
    {
        grid.UpdateData();
        CCamera camera = drawPtreview.Camera();
        try
        {
            camera.SetModes(modes);
        } catch (CameraAccessException e)
        {
            e.printStackTrace();
        }
        menu.Hide();
    }
    public void OnCancelMenu()
    {
        menu.Hide();
    }


    public void OnSetting()
    {

        if (grid.GetNumRow() > 0)
        {
            menu.Hide();
            return;
        }
        repeat[0].setVisibility(View.GONE);
        repeat[1].setVisibility(View.GONE);

        menu.setVisibility(View.VISIBLE);
        menu.SetOrient("Top");
        menu.SetCallbacks(new ActionCallback(this, "OnInitMenu"), new ActionCallback(this, "OnDestroyMenu"));

        grid.iLayoutParamsWidth = LinearLayout.LayoutParams.MATCH_PARENT;
        CCamera camera = drawPtreview.Camera();
        try
        {
            modes = camera.GetModes();
            Set<Object> set = modes.keySet();
            for (Object key: set)
            {
                Cell[] header = new Cell[1];
                header[0] = new Cell(CELL_TEXT, TEXT_READONLY | TEXT_BOLD, modes.get(key).str);
                header[0].iAligment = Gravity.LEFT;
                grid.AddRow(header);
                grid.AddRow(modes.get(key).key);


            }
        } catch (CameraAccessException e)
        {
            AddToLog(e);
        }

        grid.measure(0, 0);
        menu.measure(0, 0);
        menu.Start();

    }
    @SuppressLint("DefaultLocale")
    public void OnZoom(float val)
    {
        zoomText.setText(format("%.2f", val));
    }
    public void OnZoomPlus()
    {
        drawPtreview.Scale(drawPtreview.GetScale() + 0.05f);
    }
    public void OnZoomMinus()
    {
        drawPtreview.Scale(drawPtreview.GetScale() - 0.05f);
    }
    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState)
    {
        if (fAnimate)
            test.Start();
        else
            test.requestLayout();
        repeat[0].SetCallback(this, "OnZoomPlus");
        repeat[1].SetCallback(this, "OnZoomMinus");

        CMenuButton bt = self.findViewById(R.id.idFoto);
        bt.SetCallback(this, "onFoto");




    }

    public void OnRequestLayout(boolean changed, int left, int top, int right, int bottom)
    {


    }

    int LastState = -1;
    int LastStateK = -1;
    int LastStateI = 0;
    public void OnCameraState(int state)
    {
        if (LastState != state)
        {
            if (LastStateK != state)
                LastStateI = 0;
            LastStateI += LastStateK == state ? 1 : 0;
            LastStateK = state;
            if (LastStateI > 5 || state == 0)
            {
                LastState = state;
                final int lI = LastStateI;
                final int lS = LastState;
                AGlobals.rootHandler.Send(new Runnable() {
                    @Override
                    public void run()
                    {
                        if (lS == LastState)
                        {
                            if (!fPirke)
                            {
                                CMenuButton bt = self.findViewById(R.id.idFoto);

                                bt.Enable(lS == CCamera.CAMERA_FOCUSED);
                                ImageView v = self.findViewById(R.id.cameraState);

                            //v.setBackgroundColor(lS != CCamera.CAMERA_FOCUSED ? 0xFF00FF00 : 0xFFFF0000); //.setImageResource(lS != CCamera.CAMERA_FOCUSED ? R.drawable.led0 :  R.drawable.led2);
                                v.setColorFilter(lS != CCamera.CAMERA_FOCUSED ? 0xFFFF0000 : 0xFF00FF00, PorterDuff.Mode.SRC_IN);
                                v.invalidate();
                            }
                            if (lI > 5 && lS == 0)
                            {
                                CCamera.self.LockFocus();
                            }
                            else
                            {
                                switch(lS)
                                {
                                    case CCamera.CAMERA_SCAN_OK:
                                        CCamera.self.LockFocus();
                                        break;
                                    case CCamera.CAMERA_FOCUSED:
                                        CCamera.self.UnlockFocus();
                                        break;


                                }
                            }
                        }



                    }

                }, 1);
            }
        }
    }
    public void OnFoto()
    {

        if (CCamera.self != null)
        {
            CCamera.self.StartFoto(new ActionCallback(this, "OnOk", Bitmap.class), new ActionCallback(this, "onErrorFoto"));
        }

    }

    public void onFoto()
    {
        if (!fPirke)
        {

            CMenuButton bt = self.findViewById(R.id.idFoto);
            if (bt.IsEnable())
            {
                bt.setColorFilter(0xFF000000, PorterDuff.Mode.SRC_IN);
                bt.Enable(false);
                bt.invalidate();
                AGlobals.rootHandler.Send(new Runnable() {
                    @Override
                    public void run()
                    {
                        OnFoto();
                    }
                }, 1);
            }
        }
        else
        {
            AGlobals.rootHandler.Send(new Runnable() {
                @Override
                public void run()
                {
                    OnFoto();
                }
            }, 1);
        }

    }
    public void onErrorFoto()
    {
        CMenuButton bt = self.findViewById(R.id.idFoto);
        bt.Enable(true);

    }

    public void OnOk(Bitmap bmp)
    {

        if (bmp == null)
        {
            (new CHandler()).Send(new Runnable() {
                                      @Override
                                      public void run()
                                      {
                                          CMenuButton bt = self.findViewById(R.id.idFoto);
                                          bt.Enable(true);
                                          ImageView v = self.findViewById(R.id.cameraState);
                                          v.setImageResource(R.drawable.led0);
                                          LastState = -1;
                                          v.invalidate();
                                          drawPtreview.Restart();
                                      }
                                  }, 10);
            return;
        }
        if (CCamera.self != null)
        {
            try
            {
                CCamera.self.close();
            } catch (IOException e)
            {
                e.printStackTrace();
            }
        }
            final Activity ac = getActivity();
        final Bitmap s = bmp;

        if (onexit != null)
            runnable = new ActionCallback() {

                @Override
                public void run() {
                    onexit.run(s);
                }
            };
        ((OrientActivity)getActivity()).DialogHide();

    }


    public void OnInitDialog()
    {

    }
    public void OnDestroyDialog()
    {
    /*    @SuppressLint("RestrictedApi")
        MediaSessionCompat mediaSession = new MediaSessionCompat(getContext(), LOG_TAG);
        mediaSession.setMediaButtonReceiver(null);
*/
        ((OrientActivity)getActivity()).CloseDialog();

    }

    @Override
    public Runnable GetResult() {
        return runnable;
    }
}