package com.dryeye.app.wifi;

import com.dryeye.app.App;
import com.dryeye.app.MainActivity;
import com.dryeye.app.MenuActivity;
import com.dryeye.app.ViewActivity;

import java.io.Closeable;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;

import lib.common.ActionCallback;
import lib.common.CHandler;
import lib.common.CSemaphore;
import lib.common.CriticalCounter;
import lib.utils.AGlobals;
import lib.utils.FileUtils;
import lib.utils.TreeDir;

import static com.dryeye.app.wifi.SyncThread.ReadMdnsRecords;
import static com.dryeye.app.wifi.MdnsUtils.MDNSIsRun;
import static com.dryeye.app.wifi.MdnsUtils.StopMDNS;
import static lib.common.CDialog.IDOK;
import static lib.common.CDialog.MB_OKCANCEL;

public class MDNSServerClient extends Thread implements Closeable {
    static public String address = null;
    public final static int RUN_IDLE = 1;
    public final static int RUN_CMD = 2;
    public final static int RUN_SERVER = 3;
    public final static int RUN_CLIENT = 4;
    public final static int RUN_SERVER_CLIENT = 5;
    public final static int EXIT = 6;
    private String mdnsPatern = "com.dryeye.app";
    private String mdnsName = "com.dryeye.client";
    static public String LocalHost = null;
    protected CriticalCounter fRun = new CriticalCounter(7);
    private CSemaphore sec = new CSemaphore(1);
    MDNSServerClient self;
    private String INET_ADDR = "224.0.0.251";
    private int PORT = 5353;
    private int PORT_CLIENT = 7281;
    private CHandler client = null;
    private DatagramSocket clientSocket = null;
   // private InetAddress inetAddress = null;
    private String findName = null;
    private int iTimeOut = 0;
    private ActionCallback calback = null;
    private int WAIT_MULTISOCK = 3000;
    private int TIMER_TIMEOUT = 1000;
    private long MDNS_TIMEOUT = 15000;
    private long iStartTime = 0;
    public final static int SINC_TRUE = 1;
    public final static int SINC_FALSE = 0;
    protected CriticalCounter inSinc = new CriticalCounter(2);
    public int Number = 0;
    static public

    byte[] msg = null;

    public void Pause()
    {
        fRun.SetCount(RUN_CMD);
        if (!sec.TryEnter(2000)) return;
     //   String[] s = new String[2];
      //  s[0] = mdnsName;
      //  s[1] = "PAUSE";

      //  msg = SyncThread.CreateMdnsRecords(LocalHost, s);
        fRun.SetCount(RUN_SERVER_CLIENT);
        sec.Leave();

    }
    //-------------------------------------
    //
    //-------------------------------------
    public MDNSServerClient()
    {
        self = this;
    }

    public void Run()
    {
        LocalHost = SyncThread.LocalHost();
        INET_ADDR = SyncThread.MulticastAddress();
        PORT = SyncThread.MulticastPort();
        LocalHost = MainActivity.ReadIni("HOSTNAME");
        if (LocalHost == null)
        {
            DlgLocalHostName dlg = new DlgLocalHostName(AGlobals.currentActivity);
            dlg.DoModal();
            LocalHost = dlg.result;
            MainActivity.WriteIni("HOSTNAME", LocalHost);
            SyncThread.LocalHost(LocalHost);

        }

        try
        {
            sec.acquire();
        } catch (InterruptedException e)
        {
            e.printStackTrace();
        }
        this.start();
        try
        {
            sec.acquire();
            sec.release();
        } catch (InterruptedException e)
        {
            e.printStackTrace();
        }


    }

    public void ChangeOptions()
    {
        int c = fRun.Count();
        fRun.SetCount(RUN_CMD);
        if (!sec.TryEnter(2000)) return;
        INET_ADDR = SyncThread.MulticastAddress();
        PORT = SyncThread.MulticastPort();
     //   msg = null;
        fRun.SetCount(c);
        sec.Leave();
    }
    public void ChangeHostName()
    {
        int c = fRun.Count();
        fRun.SetCount(RUN_CMD);
        if (!sec.TryEnter(2000)) return;
        DlgLocalHostName dlg = new DlgLocalHostName(LocalHost, AGlobals.currentActivity);
        dlg.DoModal();
        LocalHost = dlg.result;
        SyncThread.LocalHost(LocalHost);
        msg = null;
        fRun.SetCount(c);
        sec.Leave();

    }

    //-------------------------------------
    //
    //-------------------------------------
    @Override
    public void run()
    {
        fRun.SetCount(RUN_SERVER_CLIENT);
        sec.release();
        while (fRun.Count() != EXIT) // && !SyncThread.IsRun())
        {
            try
            {
                Thread.sleep(TIMER_TIMEOUT);
                if (fRun.Count() > RUN_CMD && sec.TryEnter(100))
                {
                    if (fRun.Count() == RUN_SERVER || fRun.Count() == RUN_SERVER_CLIENT)
                    {
                        if (fRun.Count() == RUN_SERVER)
                        {
                            calback = null;
                            client = null;
                            findName = null;
                        }

                        OnServer();
                    } else if (fRun.Count() == RUN_CLIENT)
                        OnClient();
                    sec.Leave();
                }
            } catch (Exception e)
            {
                if (sec.Count() == 0)
                    sec.Leave();
                FileUtils.AddToLog(e);
            }
        }
    }

    //-------------------------------------
    //
    //-------------------------------------
    private void CloseSocket()
    {
        if (clientSocket != null)
        {
            try
            {
               // clientSocket.leaveGroup(inetAddress);
                clientSocket.close();
            } catch (Exception e)
            {
                e.printStackTrace();
            }
            clientSocket = null;
        }
    }

    //-------------------------------------
    //
    //-------------------------------------
    private boolean OpenSocket()
    {
        boolean fRet = false;
        if (MdnsUtils.WiFiIsConnected())
            try
            {
                //inetAddress = InetAddress.getByName(INET_ADDR);
                clientSocket = new DatagramSocket( PORT_CLIENT);
              //  clientSocket.joinGroup(inetAddress);
                clientSocket.setSoTimeout(WAIT_MULTISOCK);
                fRet = true;

            } catch (IOException e)
            {
                FileUtils.AddToLog(e);
                OnSocketError();

            }
        return fRet;
    }

    //-------------------------------------
    //
    //-------------------------------------
    private void OnSocketError()
    {
        final ActionCallback a = calback;
        if (calback != null)
            client.Send(new Runnable() {
                @Override
                public void run()
                {

                    FileUtils.AddToLog("RESTART----------------->");
                    a.run((MDnsInfo) null);
                }
            });
        calback = null;
        CloseSocket();
        calback = null;

    }


    public void SetSync(boolean b)
    {
        inSinc.SetCount(b ? SINC_TRUE : SINC_FALSE);
    }
    protected void SendLast()
    {
        if (msg != null)
        {
            try {
                InetAddress addr = InetAddress.getByName(INET_ADDR);
                MulticastSocket serverSocket = null;
                serverSocket = new MulticastSocket();
                DatagramPacket msgPacket = new DatagramPacket(msg, msg.length, addr, PORT);
                serverSocket.send(msgPacket);
                serverSocket.close();
            } catch (IOException e) {
                ;
            }
        }

    }
    //-------------------------------------
    //
    //-------------------------------------
    public void OnServer() throws IOException
    {

        if (inSinc.Count() == SINC_FALSE && (fRun.Count() == RUN_SERVER || fRun.Count() == RUN_SERVER_CLIENT))
        {
            if (true) //msg == null)
            {

                String[] s = new String[3];
                s[0] = mdnsName;
                TreeDir dir = new TreeDir(App.defaultPath + "/", new String[]{".jpg", ".ini"});
                s[1] = dir.MakeDirXml();
                s[2] = "" + Number;
                msg = SyncThread.CreateMdnsRecords2W(LocalHost, s[0], s[1], s[2]);
                //msg = SyncThread.CreateMdnsRecords(LocalHost, s);
            }
                InetAddress addr = InetAddress.getByName(INET_ADDR);
                MulticastSocket serverSocket = new MulticastSocket();
                DatagramPacket msgPacket = new DatagramPacket(msg, msg.length, addr, PORT);
                serverSocket.send(msgPacket);
                serverSocket.close();
                if (fRun.Count() == RUN_SERVER_CLIENT)
                {
                    try
                    {
                        if (!OpenSocket()) return;
                        byte[] buf = new byte[0xFFFF];
                        msgPacket = new DatagramPacket(buf, 0xFFFF);
                        clientSocket.receive(msgPacket);

                    if (msgPacket.getLength() > 12)
                    {
                        String[] res = ReadMdnsRecords(buf, msgPacket.getLength());
                        if (res != null && res.length > 4)
                        {
                            if (res[3].contains(mdnsPatern))
                            {
                                String pat = LocalHost + ":";
                                for (int i = 4; i < res.length; ++i)
                                {
                                    if (res[i].contains(pat))
                                    {
                                        final String _pat = res[i];
                                        inSinc.SetCount(SINC_TRUE);
                                        AGlobals.rootHandler.Send(new Runnable() {
                                            @Override
                                            public void run()
                                            {
                                                if (AGlobals.currentActivity instanceof MenuActivity)
                                                {
                                                    ((MenuActivity) AGlobals.currentActivity).OnSyncGet(_pat);


                                                } else
                                                {
                                                    if (AGlobals.currentActivity instanceof ViewActivity)
                                                    {
                                                        if (!((ViewActivity)AGlobals.currentActivity).IsFragmentSyncFile())
                                                        {
                                                            if (SyncMsgBox.onFinish)
                                                            {

                                                                SyncMsgBox box = new SyncMsgBox("Server", "Synchronize server data ?", MB_OKCANCEL);
                                                                if (box.DoModal() == IDOK)
                                                                {
                                                                    ((ViewActivity) AGlobals.currentActivity).OnBackPressed();
                                                                }
                                                            }

                                                        }
                                                    }
                                                    inSinc.SetCount(SINC_FALSE);
                                                }

                                            }
                                        }, 500);
                                        break;

                                    }

                                }

                            }
                        }
                    }
                    CloseSocket();
                    }catch (SocketTimeoutException e)
                    {
                        CloseSocket();
                        return;
                    }
                    catch (IOException e)
                    {
                        CloseSocket();
                        FileUtils.AddToLog(e);
                        throw new IOException();
                    }
                }

        }

    }

    public boolean IsRun()
    {
        return calback != null;
    }

    public void Run(final int iTimoutMDNSFind, final CHandler _looper, final ActionCallback _calback, final String name)
    {
        fRun.SetCount(RUN_CMD);
        if (!sec.TryEnter(2000)) return;
      //  msg = null;
        inSinc.SetCount(SINC_FALSE);
        calback = _calback;
        client = _looper;
        MDNS_TIMEOUT = iTimoutMDNSFind;
        findName = name;
        fRun.SetCount(RUN_CLIENT);
        sec.Leave();

    }

    //-------------------------------------
    //
    //-------------------------------------
    public void Close()
    {
        Close(RUN_SERVER_CLIENT);
    }

    public void Close(int f)
    {
        fRun.SetCount(RUN_CMD);
        if (!sec.TryEnter(2000)) return;
        inSinc.SetCount(SINC_FALSE);
        calback = null;
        client = null;
        msg = null;
        MDNS_TIMEOUT = -1;
        findName = null;
        fRun.SetCount(f);
        sec.Leave();
    }

    //-------------------------------------
    //
    //-------------------------------------
    public void OnClient() throws IOException
    {

        if (AGlobals.currentActivity == null)
        {
            final ActionCallback a = calback;
            if (a != null)
                client.Send(new Runnable() {
                    @Override
                    public void run()
                    {

                        StopMDNS();
                        a.run((MDnsInfo) null);

                    }
                });
            return;
        }

        if (MDNSIsRun() && fRun.Count() == RUN_CLIENT)
        {
            try
            {
                if ((MDNS_TIMEOUT != -1 && (System.currentTimeMillis() - iStartTime) > MDNS_TIMEOUT) || !MdnsUtils.WiFiIsConnected())
                {
                    final ActionCallback a = calback;
                    if (a != null)
                        client.Send(new Runnable() {
                            @Override
                            public void run()
                            {
                                StopMDNS();
                                a.run((MDnsInfo) null);

                            }
                        });
                    return;
                }
                if (!OpenSocket()) return;
                while (fRun.Count() == RUN_CLIENT)
                {
                    SendLast();

                    if ((MDNS_TIMEOUT != -1 && (System.currentTimeMillis() - iStartTime) > MDNS_TIMEOUT) || !MdnsUtils.WiFiIsConnected())
                    {
                        CloseSocket();
                        final ActionCallback a = calback;
                        if (a != null)
                            client.Send(new Runnable() {
                                @Override
                                public void run()
                                {
                                    StopMDNS();
                                    a.run((MDnsInfo) null);

                                }
                            });
                        return;
                    }
                    Thread.sleep(300);

                    byte[] buf = new byte[2048];
                    int i, l, j;
                    for (i = 0; i < 2048; ++i)
                        buf[i] = 0;
                    DatagramPacket msg = new DatagramPacket(buf, 2048);
                    try
                    {
                        clientSocket.receive(msg);
                    } catch (SocketTimeoutException e)
                    {
                        continue;
                    }
                    j = 0;
                    if (msg.getLength() < 12)
                        continue;
                    String[] res = ReadMdnsRecords(buf, msg.getLength());
                    if (res != null && res.length >= 4)
                    {
                        if (res[3].contains(mdnsPatern))
                        {
                            if (findName == null || res[0].equals(findName))
                            {
                                fRun.SetCount(RUN_IDLE);
                                final MDnsInfo info = new MDnsInfo(msg.getAddress().getHostAddress(), res);
                                final ActionCallback a = calback;
                                if (calback != null)
                                    client.Send(new Runnable() {
                                        @Override
                                        public void run()
                                        {
                                            if (fRun.Count() == RUN_IDLE)
                                            {
                                                address = info.ip;
                                                a.run(info);
                                            }
                                            if (fRun.Count() == RUN_IDLE)
                                                fRun.SetCount(RUN_CLIENT);

                                        }
                                    });


                            }
                        }
                    }
                }
                CloseSocket();
            } catch (SocketTimeoutException e)
            {
                CloseSocket();
                return;
            } catch (Exception ex)
            {
                CloseSocket();
                FileUtils.AddToLog(ex);
                OnSocketError();
                throw new IOException();

            }

        }

    }

    @Override
    public void close() throws IOException
    {
        fRun.SetCount(EXIT);
        if (!sec.TryEnter(2000))
            return;
        calback = null;
        client = null;
        MDNS_TIMEOUT = -1;
        findName = null;
        sec.Leave();

    }
}
