package com.allergoscope.app.wifi;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Looper;
import android.util.DisplayMetrics;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import com.allergoscope.app.App;
import com.allergoscope.app.R;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import lib.OrientActivity;
import lib.common.ActionCallback;
import lib.common.CDialog;
import lib.common.CHandler;
import lib.common.CStdThread;
import lib.common.CTimer;
import lib.common.IResultFragmentDlg;
import lib.common.button.CTextButton;
import lib.common.layout.CPanelDialog;
import lib.common.layout.ProgressLayout;
import lib.utils.AGlobals;
import lib.utils.CDateTime;
import lib.utils.FileUtils;

@SuppressLint("ValidFragment")
public class FragmentSyncFile extends Fragment implements IResultFragmentDlg {
    View self;
    ActionCallback onexit;
    DlgSelectHost dlgSelectHost =null;
    Float X  = null;
    Float Y = null;

    CPanelDialog test;
    SyncThread syncThread = null;
    ArrayList<String> list;
    ArrayList<String> roi;
    SendThread thread = new SendThread();
    TextView idNameText;
    CTimer timer;
    int count;

    public FragmentSyncFile(ArrayList<String> _list, ArrayList<String> _roi, ActionCallback _onexit)
    {
        super();
        onexit = _onexit;
        roi = _roi;
        list = _list;

    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
    {
        File path = new File(App.defaultPath);
        if (!path.exists())
            path.mkdirs();

        try
        {
            test = ((CPanelDialog) container);
            test.SetOrient("Top");

            test.DisableRequestLayout(true);
            test.SetCallbacks(new ActionCallback(this, "OnInitDialog"), new ActionCallback(this, "OnDestroyDialog"));
            test.AddOnShowAction(new ActionCallback(this, "OnShow"));
            test.fAutoClosed = false;
            self = inflater.inflate(R.layout.dlg_sync2, container, false);
            DisplayMetrics metrics = AGlobals.GetDisplayMetrcs();
            self.setLayoutParams(new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT));
            self.measure(0, 0);
            //idWaitText
            int h = self.getMeasuredHeight();
            int w = self.getMeasuredWidth();
            float Y = (metrics.heightPixels - h) / 2.f;
            float X = (metrics.widthPixels - w) / 2.f;
            test.setTranslationY(Y);
            test.setTranslationX(X);




            timer = new CTimer(this, "OnTimer");
            count = 0;
 	        CTextButton bt = self.findViewById(R.id.idCancelWait);
            bt.Init(new ActionCallback(this, "OnCancelWait"));
           
            LinearLayout barWait;
            barWait = self.findViewById(R.id.waitProcessing);
            barWait.setVisibility(View.INVISIBLE);



        } catch (Exception p)
        {
            p.printStackTrace();
        }
        return self;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState)
    {
        idNameText = self.findViewById(R.id.idNameText);
        idNameText.setText(null);
        idNameText = self.findViewById(R.id.idNameText1);
        idNameText.setText(getContext().getString(R.string.sync_poc));


        test.Start();



    }

    boolean fShow = false;

    public void OnShow()
    {

        if (fShow) return;
        fShow = true;
        AGlobals.rootHandler.Send(new Runnable() {
            @Override
            public void run()
            {
                Start();
            }
        });
    }

    void Start()
    {


        ProgressLayout progress = self.findViewById(R.id.waitBar);
        progress.Init();
        progress.Play();
        StartMDNS(SyncThread.ReadProfile() == null);

    }

    public void OnTimer()
    {
        if (syncThread == null)
            StartMDNS(true);

    }

    void StartMDNS(boolean fCheckHost)
    {

        SyncThread.Profile prof = SyncThread.ReadProfile();
        if (fCheckHost)
        {
            MdnsUtils.StopMDNS();
            timer.Stop();
            try
            {
                Thread.sleep(1000);
            } catch (InterruptedException e)
            {
                e.printStackTrace();
            }
            dlgSelectHost = new DlgSelectHost();
            if (dlgSelectHost.DoModal() == CDialog.IDCANCEL)
            {
                dlgSelectHost = null;
                ((OrientActivity) getActivity()).DialogHide();
                return;
            }
            prof = SyncThread.ReadProfile();
        }
        dlgSelectHost = null;
        timer.Run(10000);
        MdnsUtils.StartMDNS(-1, new CHandler(Looper.myLooper()), new ActionCallback(this, "OnFindHost", MDnsInfo.class), prof.HostName);

    }

    public void OnFindHost(MDnsInfo info)
    {
        MdnsUtils.MDNSPause();
        if (dlgSelectHost == null && syncThread == null)
        {

            timer.Stop();
            if (info.num > 0)
            {
                syncThread = SyncThread.MakeInstance();
                syncThread.Run(info.ip, new ActionCallback(this, "onconnect", boolean.class), new ActionCallback(this, "ondisconnect", boolean.class));
            } else
            {
                DrawText("Server busy..");
            }
        }
    }

    public void onconnect(boolean b)
    {
        if (b)
        {
            try
            {
                MdnsUtils.MDNSPause();
                thread.Run();
            } catch (Exception e)
            {
                e.printStackTrace();
            }

        }
    }

    public void ondisconnect(boolean b)
    {
        if (MdnsUtils.MDNSIsRun())
        {
            MdnsUtils.StopMDNS();
            if (timer != null)
            {
                ((OrientActivity) getActivity()).DialogHide();
            }
        }
    }

    public void OnInitDialog()
    {


    }
    public void OnCancelWait()
    {


        CTextButton bt = self.findViewById(R.id.idCancelWait);
        bt.setEnabled(false);

        TextView txt = self.findViewById(R.id.textWaitCalc);
        txt.setText("Wait close");
    }
    public void OnDestroyDialog()
    {
        timer.Stop();
        timer = null;
        if (syncThread != null)
            syncThread.Stop();
        MdnsUtils.StopMDNS();
        MdnsUtils.OnSync();

        ((OrientActivity) getActivity()).CloseDialog();

    }

    @Override
    public Runnable GetResult()
    {
        return onexit;
    }

    void DrawText(final String text)
    {
        AGlobals.rootHandler.Send(new Runnable() {
            @Override
            public void run()
            {
                idNameText.setText(text);
            }
        });
    }

    class SendThread extends CStdThread {
        @Override
        protected void OnRun()
        {
            for (String path : list)
            {
                if (syncThread.IsRun())
                {

                    /*int last_i = path.lastIndexOf(".");
                    int first_i = path.lastIndexOf("/");
                    String title = CDateTime.ConvertTime(path.substring(first_i +1, last_i));

                    DrawText(path);

                     */
                    syncThread._SendFile(App.defaultPath, path, false);

                } else
                    break;
            }
            if (syncThread.IsRun())
            {
                for (String path : roi)
                {
                    if (syncThread.IsRun())
                    {
                        String p = path.replace(".roi", ".res");
                        FileUtils.AddToLog(p + " -> " + syncThread._GetFile("ROI", App.defaultPath, path));
                        FileUtils.AddToLog(p + " -> " + syncThread._GetFile("FILE", App.defaultPath, p));
                    } else
                        break;
                }

            }
            AGlobals.rootHandler.Send(new Runnable() {
                @Override
                public void run()
                {
                    try
                    {
                    //    idNameText.setText(null);
                        close();
                        syncThread.Stop();
                        ((OrientActivity) getActivity()).DialogHide();
                    } catch (IOException e)
                    {
                        e.printStackTrace();
                    }
                }
            });

        }

        @Override
        protected void OnClose()
        {

        }
    }

}
