package com.allergoscope.app.wifi;


import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Looper;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import android.util.DisplayMetrics;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.allergoscope.app.App;
import com.allergoscope.app.R;
import com.allergoscope.app.card.FragmentExploreCards;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

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
import lib.utils.TreeDir;

@SuppressLint("ValidFragment")
public class FragmentSyncAll extends Fragment implements IResultFragmentDlg {
    View self;
    ActionCallback onexit;
    DlgSelectHost dlgSelectHost;
    Runnable runnable = null;
    CPanelDialog test;
    File zip = null;
    SyncThread syncThread = null;
    String mydir = "";
    ArrayList<String> list = new ArrayList<String>();
    ArrayList<String> roi = new ArrayList<String>();
    SendThread thread = new SendThread();
    TextView idNameText;
    CTimer timer;
    int count;
    String sget = null;
    boolean fWaitCalc = true;
    String command = null;
    ProgressLayout barWait;

    public static Comparator<TreeDir.FileNode> cmpFileNode = new Comparator<TreeDir.FileNode>() {
        public int compare(TreeDir.FileNode s1, TreeDir.FileNode s2) {
            String t1 = s1.name.toUpperCase();
            String t2 = s2.name.toUpperCase();
            Object b ;
            return t1.compareTo(t2);
        }};
    public static Comparator<File> cmpFile = new Comparator<File>() {
        public int compare(File s1, File s2) {
            String t1 = s1.getName().toUpperCase();
            String t2 = s2.getName().toUpperCase();
            Object b ;
            return t1.compareTo(t2);
        }};

    public FragmentSyncAll(String _get, ActionCallback _onexit)
    {
        super();
        sget = _get;
        command = "FSYS";
        onexit = _onexit;
    }

    public FragmentSyncAll(ActionCallback _onexit)
    {
        super();
        command = "FSYS";
        onexit = _onexit;
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


            int h = self.getMeasuredHeight();
            int w = self.getMeasuredWidth();
            float Y = (metrics.heightPixels - h) / 2.f;
            float X = (metrics.widthPixels - w) / 2.f;
            test.setTranslationY(Y);
            test.setTranslationX(X);

            timer = new CTimer(this, "OnTimer");
            count = 0;
            dlgSelectHost = null;
            FragmentExploreCards.LastPath = null;
            CTextButton bt = self.findViewById(R.id.idCancelWait);
            bt.Init(new ActionCallback(this, "OnCancelWait"));

            barWait = self.findViewById(R.id.waitBar);
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
        if (mydir.length() == 0)
        {
            TreeDir dir = new TreeDir(App.defaultPath + "/", new String[]{".jpg", ".ini"});
            ArrayList<String> list = dir.GetFileList(new String[]{".jpg"});
            for (String g : list)
            {
                String path = g.replace(".jpg", ".roi");

                String str = App.defaultPath + path;
                File file = new File(str);
          //      if (!file.exists())
                    roi.add(path);
            }
            mydir = dir.MakeXml();
        }


        if (mydir == null)
        {
            ((OrientActivity) getActivity()).DialogHide();
            return;
        }

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
            String res = syncThread._Write(command, mydir);
            if (res == null)
            {
                ondisconnect(b);
                return;
            }
            Log.e("RESULT", res);
            try
            {
                TreeDir dir = new TreeDir();
                list = dir.ReadXml(res);
            //    File outputDir = getContext().getCacheDir();
              //  File outputFile = File.createTempFile("test", ".zip", outputDir);
               // if (dir.root.MakeZip(outputFile, App.defaultPath))
                //{
               //     zip = outputFile;
               // }
               // else
                    zip = null;


                thread.Run();
            } catch (Exception e)
            {
                e.printStackTrace();
            }

        }
    }

    @SuppressLint("SuspiciousIndentation")
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

        fWaitCalc = false;
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
        return runnable;
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
                    if (path.contains(".ini") && !path.contains("info.ini"))
                        continue;

                    final String ph = path;
                   /* AGlobals.rootHandler.Send(new Runnable() {
                        @Override
                        public void run()
                        {
                            DrawText(ph);
                        }
                    });*/
                    FileUtils.AddToLog(path + " -> " +
                            syncThread._SendFile(App.defaultPath, path, false));

                } else
                    break;
            }
            AGlobals.rootHandler.Send(new Runnable() {
                @Override
                public void run()
                {
                    if (barWait != null) {
                        barWait.setVisibility(View.VISIBLE);
                        DrawText(null);
                    }
                }
            });
            TreeDir tmp = new TreeDir();
            final int lroi = roi.size();
            int i;
            for(i = 0; i < lroi; ++i)
            {
                final String ph = "" + (i + 1) + "/" + lroi + " " + getContext().getString(R.string.sync_poc);

                AGlobals.rootHandler.Send(new Runnable() {
                    @Override
                    public void run()
                    {
                        DrawText(ph);
                    }
                });
                String path = roi.get(i);
                if (syncThread.IsRun() && timer != null && fWaitCalc)
                {
                        String p = path.replace(".roi", ".res");
                        FileUtils.AddToLog(p + " -> " + syncThread._GetFile("ROI", App.defaultPath, path));
                        FileUtils.AddToLog(p + " -> " + syncThread._GetFile("FILE", App.defaultPath, p));
                } else
                    tmp.AddFile(path);

            }
            if (tmp.Root() != null && !fWaitCalc)
            {
                String  s = tmp.MakeXml();
                syncThread._Write("CALCALL", s);

            }

            //if (timer == null)
            //String res = syncThread.Write(command, mydir);

            if (syncThread.IsRun() && sget != null)
            {
                mydir = sget;
                try
                {
                    String res = syncThread._Write("FGET", mydir);
                    Log.e("RESULT", res);
                    TreeDir dir = new TreeDir();
                    list = dir.ReadXml(res);
                    dir.CreateDirFromDisk(App.defaultPath);
                    if (dir.Root() != null)
                        dir.Root().ReloadFiles(App.defaultPath, new String[]{".jpg", ".ini"});
                    OnRun2(dir);

                } catch (Exception e)
                {
                    e.printStackTrace();
                }


            }
            if (timer != null)
            {
                AGlobals.rootHandler.Send(new Runnable() {
                    @Override
                    public void run()
                    {
                        try
                        {
                            close();

                            syncThread.Stop();
                             if (timer != null)
                             {
                                 idNameText.setText(null);
                                MdnsUtils.MDNSPause();
                                ((OrientActivity) AGlobals.currentActivity).DialogHide();
                             }
                        } catch (IOException e)
                        {
                            e.printStackTrace();
                        }
                    }
                });
            }
        }
        protected void OnRun2( TreeDir dir)
        {

            String path = App.defaultPath;
            if (syncThread.IsRun())
                Sync(dir, new String[]{".jpg", ".ini"}, new File(path), "/");
            if (syncThread.IsRun())
            {
                dir = new TreeDir(App.defaultPath + "/", new String[]{".jpg"});
                ArrayList<String> list = dir.GetFileList(new String[]{".jpg"});
                roi = new ArrayList<String>();
                for (String g : list)
                {
                    path = g.replace(".jpg", ".roi");

                    String str = App.defaultPath + path;
                    File file = new File(str);
                    if (!file.exists())
                        roi.add(path);
                }
                for (String path_roi : roi)
                {
                    if (syncThread.IsRun())
                    {
                        String p = path_roi.replace(".roi", ".res");
                        FileUtils.AddToLog(p + " -> " + syncThread._GetFile("ROI", App.defaultPath, path_roi));
                        FileUtils.AddToLog(p + " -> " + syncThread._GetFile("FILE", App.defaultPath, p));
                    } else
                        break;
                }
            }
        }
        protected void Sync(TreeDir dir, String [] mask, File curDir, String ph)
        {
            if (syncThread.IsRun())
                if (dir != null)
                {
                    String path = App.defaultPath;
                    if (!ph.equals("/"))
                        path += ph;
                    ArrayList<File> fdir = new ArrayList<File>();
                    ArrayList<File> ffile = new ArrayList<File>();
                    ArrayList< TreeDir.FileNode > sdir = new ArrayList< TreeDir.FileNode >();
                    ArrayList< TreeDir.FileNode > sfile = new ArrayList< TreeDir.FileNode >();

                    dir.GetFileList(ph, mask, sdir, sfile);

                    FileUtils.GetFileList(curDir, mask, fdir, ffile);
                    Collections.sort(sfile, cmpFileNode);
                    Collections.sort(sdir, cmpFileNode);
                    Collections.sort(ffile, cmpFile);
                    Collections.sort(fdir, cmpFile);
                    for (int i = 0; i < fdir.size(); ++i)
                    {
                        TreeDir.FileNode f = new TreeDir.FileNode(null, fdir.get(i).getName());
                        int index = Collections.binarySearch(sdir, f, cmpFileNode);
                        if (index < 0)
                        {
                            try {
                                FileUtils.DeleteDir(fdir.get(i));
                            } catch (Exception e) {
                                FileUtils.AddToLog(e);
                            }
                            fdir.remove(i);
                            --i;
                        }

                    }
                    for (int i = 0; i < sdir.size(); ++i)
                    {
                        String p = path + File.separator + sdir.get(i).name;
                        File d = new File(p);
                        int index = Collections.binarySearch(fdir, d, cmpFile);
                        if (index < 0)
                            d.mkdir();
                        Sync(dir, mask, d, ph + (ph.equals("/") ? "" : "/") + sdir.get(i).name);

                    }
                    for (int i = 0; syncThread.IsRun() && i <  ffile.size(); ++i)
                    {
                        TreeDir.FileNode f = new TreeDir.FileNode(null, ffile.get(i).getName());
                        int index = Collections.binarySearch(sfile, f, cmpFileNode);
                        if (index < 0)
                        {
                            try {
                                ffile.get(i).delete();
                            }
                            catch (Exception e)
                            {
                                FileUtils.AddToLog(e);
                                break;
                            }
                            ffile.remove(i);
                            --i;
                        }
                    }
                    String s;
                    File fi;
                    TreeDir.FileNode node;
                    for (int i = 0; syncThread.IsRun() && i < sfile.size(); ++i)
                    {
                        try {
                            node = sfile.get(i);
                            String ph1 = ph + (!ph.equals("/") ? File.separator : "") + node.name;

                            boolean fGet = false;
                            File f = new File(node.name);
                            int index = Collections.binarySearch(ffile, f, cmpFile);
                            if (index < 0)
                                fGet = true;
                            else
                            {
                                CDateTime tm1 = new CDateTime(new File(App.defaultPath + ph1));
                                CDateTime tm2 = new CDateTime(node.date);
                                fGet = CDateTime.Compare(tm2, tm1) > 0;
                            }
                            if (fGet)
                            {
                                try {
                                    if (node.name.contains(".ini") && !node.name.equals("info.ini")) continue;
                                    FileUtils.AddToLog("GET :" +ph1 + " -> " + syncThread._GetFile("FILE", App.defaultPath, ph1));

                                    File _file = new File(App.defaultPath + ph1);
                                    _file.setLastModified(App.formatDate.parse(node.date).getTime());

                                }
                                catch (Exception e)
                                {
                                    FileUtils.AddToLog(e);
                                }
                            }


                        }
                        catch(Exception e)
                        {
                            FileUtils.AddToLog(e);

                        }

                    }


                }


        }

        @Override
        protected void OnClose()
        {

        }
    }

}