package com.dryeye.app.wifi;

import static java.lang.Thread.sleep;

import android.os.Handler;
import android.widget.Toast;

import com.dryeye.app.MainActivity;

import java.io.Closeable;
import java.io.IOException;
import java.util.concurrent.Semaphore;

import lib.common.ActionCallback;
import lib.common.CObject;
import lib.common.CStdThread;
import lib.common.CriticalCounter;
import lib.utils.AGlobals;
import lib.utils.FileUtils;

public class SyncThread implements Runnable {
    public static class Profile {
        public String SSID;
        public String HostName;

        public Profile(String ssid, String name)
        {
            SSID = ssid;
            HostName = name;
        }

        public Profile()
        {
            SSID = null;
            HostName = null;

        }

        public boolean IsEmpty()
        {
            return SSID == null || HostName == null;
        }

    }

    private static SyncThread instance = null;
    private Thread self = null;
    final static int RUN_TRUE = 1;
    final static int RUN_FALSE = 2;
    private CriticalCounter fIsRun;
    private Semaphore sec = null;
    private String address = null;
    static int port = 7182;
    private ActionCallback onconnect = null;
    private ActionCallback ondisconnect = null;

    static public SyncThread MakeInstance()
    {
        if (instance == null)
            instance = new SyncThread();
        return instance;
    }

    private SyncThread()
    {
        fIsRun = new CriticalCounter(3);
        fIsRun.SetCount(RUN_FALSE);
        sec = new Semaphore(1, false);

    }

    public static SyncThread Instance()
    {
        return instance;
    }

    //-----------------------------------------
    //
    //-----------------------------------------
    public static boolean IsRun()
    {
        return instance != null && instance.self != null && instance.fIsRun.Count() == RUN_TRUE;

    }

    //-----------------------------------------
    //
    //-----------------------------------------
    public boolean IsWait()
    {
        return sec.availablePermits() == 0 && (self != null && fIsRun.Count() != RUN_TRUE);

    }

    public boolean Run(String _address, ActionCallback _onconnect, ActionCallback _ondisconnect)
    {
        if (IsWait() || IsRun()) return true;
        address = _address;
        fIsRun.SetCount(RUN_FALSE);
        try
        {

            if (!sec.tryAcquire()) return false;
            onconnect = _onconnect;
            ondisconnect = _ondisconnect;
            Start();
            sec.acquire();

        } catch (InterruptedException e)
        {
            FileUtils.AddToLog(e);
        }
        return true;
    }

    //-----------------------------------------
    //
    //-----------------------------------------
    private void Start()
    {
        self = new Thread(this);
        self.start();
    }

    //-----------------------------------------
    //
    //-----------------------------------------
    public void Stop()
    {
        if (self != null)
        {
            {
                try
                {
                    ondisconnect = null;
                    DetachCppThread();
                    self = null;


                } catch (Exception e)
                {
                    e.printStackTrace();
                }
            }

        }
        instance = null;
    }

    //-----------------------------------------
    //
    //-----------------------------------------
    @Override
    public void run()
    {
        try
        {
            sec.release();
            _run();
        } catch (Exception e)
        {
            FileUtils.AddToLog(e);

        }
    }

    //-----------------------------------------
    //
    //-----------------------------------------
    private boolean run_TryConnect()
    {
        boolean b = false;
        if (address != null)
        {
            for (int i = 0; i < 3; ++i)
            {
                if (TryConnect(address, port))
                {
                    b = true;
                    break;
                }
                AGlobals.Sleep(1000);
            }
            FileUtils.AddToLog("tryConnect = " + b);
        } else
            FileUtils.AddToLog("tryConnect =  address null");
        if (b)
            fIsRun.SetCount(RUN_TRUE);

        return b;
    }

    //-----------------------------------------
    //
    //-----------------------------------------
    private void _run()
    {
        boolean fRet = false;

        try
        {
            if ((fRet = run_TryConnect()))
            {
                if (AGlobals.currentActivity != null)
                    AGlobals.rootHandler.Send(new Runnable() {
                        @Override
                        public void run()
                        {
                            FileUtils.AddToLog("Connected " + address);
                            if (fIsRun.Count() == RUN_TRUE)
                            {
                                try
                                {

                                    if (onconnect != null)
                                    {
                                        ActionCallback c = onconnect;
                                        onconnect = null;
                                        Toast.makeText(AGlobals.contextApp, "Connected " + address, Toast.LENGTH_SHORT).show();
                                        c.run(true);
                                    }
                                } catch (Exception ex)
                                {

                                }
                            }

                        }
                    }, 200);
                LoopThread();
                fIsRun.SetCount(RUN_FALSE);
            }

        } catch (Exception e)
        {
            FileUtils.AddToLog(e);

        }
        _run_ExitThread();
    }

    //-----------------------------------------
    //
    //-----------------------------------------
    private void _run_ExitThread()
    {
        if (AGlobals.currentActivity != null)
            AGlobals.currentActivity.runOnUiThread(new Runnable() {
                @Override
                public void run()
                {
                    FileUtils.AddToLog("Diconnected " + address);
                    if (onconnect != null)
                    {
                        Toast.makeText(AGlobals.contextApp, "Connect failed " + address, Toast.LENGTH_SHORT).show();
                        ActionCallback c = onconnect;
                        onconnect = null;
                        ondisconnect = null;
                        c.run(false);
                    } else if (ondisconnect != null)
                    {
                        ActionCallback c = ondisconnect;
                        ondisconnect = null;
                        c.run(false);
                        Toast.makeText(AGlobals.contextApp, "Disconnect " + address, Toast.LENGTH_SHORT).show();
                    }

                }
            });
    }

    //-----------------------------------------
    //
    //-----------------------------------------
    static public String LocalHost()
    {
        return MainActivity.ReadIni("LOCAL_NAME");
//        return AGlobals.ReadIni("LOCAL", "NAME", "wifi.ini");
    }

    //-----------------------------------------
    //
    //-----------------------------------------
    static public String MulticastAddress()
    {
        String address = "224.0.0.251"; //AGlobals.ReadIni("MULTICAST", "ADDRESS", "wifi.ini");
     /*   if (address == null)
        {
            address = "224.0.0.251";
            MulticastAddress(address);
        }*/
        return address;

    }

    //-----------------------------------------
    //
    //-----------------------------------------
    static public int MulticastPort()
    {
/*
        String port = AGlobals.ReadIni("MULTICAST", "PORT", "wifi.ini");
        if (port == null)
        {
            port = "5353";
            MulticastPort(port);
        }
        */

        return 5353; //Integer.valueOf(port).intValue();
    }

    //-----------------------------------------
    //
    //-----------------------------------------
    static public void MulticastAddress(String address)
    {
        AGlobals.SaveIni("MULTICAST", "ADDRESS", address, "wifi.ini");
    }

    //-----------------------------------------
    //
    //-----------------------------------------
    static void MulticastPort(String n)
    {
        AGlobals.SaveIni("MULTICAST", "PORT", n, "wifi.ini");
    }

    //-----------------------------------------
    //
    //-----------------------------------------
    static public void LocalHost(String name)
    {
        MainActivity.WriteIni("LOCAL_NAME", name);

        //AGlobals.SaveIni("LOCAL", "NAME", name, "wifi.ini");
    }

    //-----------------------------------------
    //
    //-----------------------------------------
    static public Profile ReadProfile()
    {
        Profile con = new Profile();
        con.HostName = AGlobals.ReadIni("CONNECT", "NAME", "wifi.ini");
        con.SSID = AGlobals.ReadIni("CONNECT", "SSID", "wifi.ini");
        return con.IsEmpty() ? null : con;
    }

    //-----------------------------------------
    //
    //-----------------------------------------
    static public void SaveProfile(Profile con)
    {
        if (con == null)
            con = new Profile();

        AGlobals.SaveIni("CONNECT", "SSID", con.SSID, "wifi.ini");
        AGlobals.SaveIni("CONNECT", "NAME", con.HostName, "wifi.ini");
    }

    native private void DetachCppThread();

    native private boolean TryConnect(String address, int port);

    native private void LoopThread();
    public String _SendFile(String path_dir, String path_relative, boolean fIsNew)
    {
        String ret = "ERROR";
        for (int i = 0; i < 3; ++i) {
            ret = SendFile(path_dir, path_relative, fIsNew);
            if (ret == null) ret = "ERROR";

            if (!ret.equals("ERROR"))
                break;
            else {
                try {
                    sleep(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
        return ret;
    }
    public String _Write(String cmd, String param)
    {
        String ret = "ERROR";
        for (int i = 0; i < 3; ++i) {
            ret = Write(cmd, param);
            if (ret == null) ret = "ERROR";
            if (!ret.equals("ERROR"))
                break;
            else {
                try {
                    sleep(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
        return ret;
    }
    public String _GetFile(String cmd, String path_dir, String path_relative)
    {
        {
            String ret = "ERROR";
            for (int i = 0; i < 3; ++i) {
                ret = GetFile(cmd, path_dir, path_relative);
                if (ret == null) ret = "ERROR";
                if (!ret.equals("ERROR"))
                    break;
                else {
                    try {
                        sleep(100);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
            return ret;
        }
    }

    native private String SendFile(String path_dir, String path_relative, boolean fIsNew);

    native private String Write(String cmd, String param);

   // native public String Read(String cmd);

    native private String GetROI(String path_dir, String path_relative);

    native private String GetFile(String cmd, String path_dir, String path_relative);

    native public static String[] ReadMdnsRecords(byte[] arr, int len);

    native public static byte[] CreateMdnsRecords(String ClientName, String[] arr);
    native public static byte[] CreateMdnsRecords2W(String ClientName, String client, String data, String number);


}
