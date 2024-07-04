package lib.utils;


import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.location.LocationManager;
import android.net.ConnectivityManager;
import android.net.DhcpInfo;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkInfo;
import android.net.NetworkRequest;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.provider.Settings;
import android.text.Html;
import android.widget.Toast;

import androidx.annotation.NonNull;

import com.allergoscope.app.App;
import com.allergoscope.app.wifi.INetwork;
import com.allergoscope.app.wifi.MDNSServerClient;

import java.io.Closeable;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.math.BigInteger;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.net.UnknownHostException;
import java.util.ArrayList;

import lib.common.ActionCallback;
import lib.common.CHandler;
import lib.common.CStdThread;
import lib.common.CriticalCounter;
import lib.common.CriticalSection;
import lib.common.MessageBox;

import static android.net.wifi.WifiInfo.FREQUENCY_UNITS;
import static android.net.wifi.WifiInfo.LINK_SPEED_UNITS;
import static lib.common.CDialog.IDCANCEL;
import static lib.common.CDialog.IDYES;
import static lib.common.CDialog.MB_OKCANCEL;
import static lib.common.CDialog.MB_YESNO;
import static lib.utils.FileUtils.AddToLog;

public abstract class WiFiUtils {
    static ThreadCheckNetwork network = null;
    static ThreadCheckMonitor monitor = null;
    static ThreadScan scan = null;
    static CriticalCounter fConnect;


    public static boolean IsNetworkConnect()
    {
        ConnectivityManager cm = (ConnectivityManager) App.GetInstance().getApplicationContext().getSystemService(Context.CONNECTIVITY_SERVICE);
        if (cm == null) return false;
        NetworkInfo wifiInfo = cm.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
        if (wifiInfo != null && wifiInfo.getDetailedState() == NetworkInfo.DetailedState.CONNECTED)
            return true;
        wifiInfo = cm.getNetworkInfo(ConnectivityManager.TYPE_MOBILE);
        if (wifiInfo != null && wifiInfo.isConnected())
            return true;
        wifiInfo = cm.getActiveNetworkInfo();
        return wifiInfo != null && wifiInfo.isConnected();
    }

    static Object checkConnect = new Object();

    //----------------------------------------------------------------------------------
    //
    //----------------------------------------------------------------------------------
    public static boolean WiFiIsConnected()
    {
        return GetWiFiInfo() != null;
    }

    //----------------------------------------------------------------------------------
    //
    //----------------------------------------------------------------------------------
    public static String[] GetWiFiInfo()
    {
        synchronized (checkConnect)
        {
            String[] sinfo = null;
            // if (fConnect == null || fConnect.Count() == 1)
            {
                try
                {
                    ConnectivityManager cm = (ConnectivityManager) AGlobals.contextApp.getSystemService(Context.CONNECTIVITY_SERVICE);
                    if (cm != null)
                    {

                        NetworkInfo wifiInfo = null;
                        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.M)
                        {
                            Network network = cm.getActiveNetwork();
                            NetworkCapabilities capabilities = cm.getNetworkCapabilities(network);
                            if (capabilities != null && capabilities.hasTransport(NetworkCapabilities.TRANSPORT_WIFI))
                            {
                                wifiInfo = cm.getNetworkInfo(network);
                            }
                        } else
                            wifiInfo = cm.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
                        //       Log.e("=========>", "1. NetworkInfo " + wifiInfo);
                        if (wifiInfo == null)
                        {
                            Network[] net = cm.getAllNetworks();
                            for (Network n : net)
                            {
                                NetworkCapabilities capabilities = cm.getNetworkCapabilities(n);
                                if (capabilities != null && capabilities.hasTransport(NetworkCapabilities.TRANSPORT_WIFI))
                                {
                                    wifiInfo = cm.getNetworkInfo(n);
                                    break;
                                }

                            }

                        }
                        //      Log.e("=========>", "2. NetworkInfo " + wifiInfo);
                        if (wifiInfo != null && wifiInfo.isConnected()) //wifiInfo.getTypeName().equals("WIFI")
                        {

                            WifiManager manager = (WifiManager) AGlobals.contextApp.getSystemService(Context.WIFI_SERVICE);
                            DhcpInfo info = manager.getDhcpInfo();
                            //     Log.e("=========>", "DhcpInfo " + info);

                            if (info != null)
                            {
                                String ip = GetIP(info.ipAddress);
                                String dns = GetIP(info.dns1);
                                boolean fRet = !ip.equals("0.0.0.0") && !dns.equals("0.0.0.0");
                                if (fRet)
                                {
                                    sinfo = new String[3];
                                    WifiInfo winfo = manager.getConnectionInfo();
                                    String tmp = winfo.getSSID();
                                    sinfo[2] = tmp.substring(1, tmp.length() - 1);
                                    sinfo[1] = dns;
                                    sinfo[0] = ip;
                                }
                            }

                        }
                    }
                } catch (Exception e)
                {

                    sinfo = null;
                }
            }
            return sinfo;
        }
    }

    //----------------------------------------------------------------------------------
    //
    //----------------------------------------------------------------------------------
    public static boolean EnableWiFi()
    {
        WifiManager manager = (WifiManager) App.GetInstance().getApplicationContext().getSystemService(Context.WIFI_SERVICE);

        if (!manager.isWifiEnabled())
        {
            MessageBox b = new MessageBox("WIFI is disabled on your device. Would you like to switch WIFI on? ", null, MB_OKCANCEL);
            if (b.DoModal() == IDCANCEL) return false;

            manager.setWifiEnabled(true);

            while (!manager.isWifiEnabled())
            {
                AGlobals.Sleep(50);
                manager = (WifiManager) App.GetInstance().getApplicationContext().getSystemService(Context.WIFI_SERVICE);
            }
        }
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)
        {
            final LocationManager manager2 = (LocationManager) App.GetInstance().getApplicationContext().getSystemService(Context.LOCATION_SERVICE);
            if (!manager2.isProviderEnabled(LocationManager.GPS_PROVIDER))
            {
                MessageBox box = new MessageBox("GPS is disabled on your device, so program cannot see any networks. Would you like to switch GPS on? ", null, MB_YESNO);
                if (box.DoModal() == IDYES)
                {
                    final Intent intent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
                    AGlobals.currentActivity.startActivity(intent);
                } else
                    return false;


            }
        }

        return true;

    }

    //----------------------------------------------------------------------------------
    //
    //----------------------------------------------------------------------------------
    public static boolean IsWiFiEnable()
    {
        WifiManager manager = (WifiManager) App.GetInstance().getApplicationContext().getSystemService(Context.WIFI_SERVICE);

        return manager != null && manager.isWifiEnabled();
    }

    //----------------------------------------------------------------------------------
    //
    //----------------------------------------------------------------------------------
    public static String GetIP(int ip)
    {
        if (ip != 0)
        {
            try
            {
                byte[] _myIPAddress = BigInteger.valueOf(ip).toByteArray();
                byte[] myIPAddress = new byte[_myIPAddress.length];
                for (int i = 0; i < _myIPAddress.length; ++i)
                    myIPAddress[i] = _myIPAddress[_myIPAddress.length - 1 - i];
                InetAddress inetip = InetAddress.getByAddress(myIPAddress);
                return inetip.getHostAddress();
            } catch (UnknownHostException e)
            {
                return "0.0.0.0";
            }
        }
        return "0.0.0.0";


    }

    //----------------------------------------------------------------------------------
    //
    //----------------------------------------------------------------------------------
    public static CharSequence GetStringWiFiInfo()
    {

        StringBuilder builder = new StringBuilder();
        WifiManager manager = (WifiManager) App.GetInstance().getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        if (!manager.isWifiEnabled()) return "Wi-Fi disabled";
        WifiInfo info = manager.getConnectionInfo();
        DhcpInfo dhcpInfo = manager.getDhcpInfo();
        builder.append(String.format("<b>Wi-Fi informatiom</b><br/><b>SSID: </b>%s<br/>", info.getSSID()));
        builder.append(String.format("<b>RSSI: </b>%s dBm<br/>", info.getRssi()));
        builder.append(String.format("<b>Link speed: </b>%s %s<br/>", info.getLinkSpeed(), LINK_SPEED_UNITS));
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.LOLLIPOP)
        {
            builder.append(String.format("<b>Frequency: </b>%s %s<br/>", "" + info.getFrequency(), FREQUENCY_UNITS));
        }
        String ip = "0.0.0.0";
        String dns = "0.0.0.0";
        String dns2 = "0.0.0.0";
        String gateway = "0.0.0.0";
        if (info != null)
        {
            ip = GetIP(dhcpInfo.ipAddress);
            dns = GetIP(dhcpInfo.dns1);
            dns2 = GetIP(dhcpInfo.dns2);
            gateway = GetIP(dhcpInfo.gateway);
        }
        builder.append(String.format("<b>IP: </b>%s<br/>", ip));
        builder.append(String.format("<b>DNS: </b>%s<br/>", dns));
        if (!dns2.equals("0.0.0.0"))
            builder.append(String.format("<b>DNS 2: </b>%s<br/>", dns2));
        if (!gateway.equals("0.0.0.0"))
            builder.append(String.format("<b>Gateway: </b>%s<br/>", gateway));

        return Html.fromHtml(builder.toString().trim());
    }

    //----------------------------------------------------------------------------------
    //
    //----------------------------------------------------------------------------------
    static public boolean IsNetOk()
    {
        final CriticalSection sec1 = new CriticalSection();
        final boolean[] fError = {false};
        sec1.Enter();
        Thread th = new Thread(new Runnable() {
            @Override
            public void run()
            {

                InetAddress inetAddress = null;
                MulticastSocket clientSocket = null;
                try
                {
                    inetAddress = InetAddress.getByName("224.0.0.251");
                    clientSocket = new MulticastSocket(5353);
                    clientSocket.joinGroup(inetAddress);
                    clientSocket.leaveGroup(inetAddress);
                    clientSocket.close();
                } catch (UnknownHostException e)
                {
                    fError[0] = true;
                } catch (IOException e)
                {
                    fError[0] = true;
                } catch (Exception e)
                {
                    fError[0] = true;
                }
                sec1.Leave();
            }
        });
        th.start();
        sec1.Enter();
        return !fError[0];

    }


    //----------------------------------------------------------------------------------
    //
    //----------------------------------------------------------------------------------
    public static void NetworkStartTrace(String name, INetwork receiver)
    {
        if (network != null)
            network.Start(name, receiver);
    }

    public static void NetworkEndTrace()
    {
        if (network != null)
            network.End();
    }

    //----------------------------------------------------------------------------------
    //
    //----------------------------------------------------------------------------------
    public static void Init()
    {

        fConnect = null;
        boolean b = GetWiFiInfo() != null;
        fConnect = new CriticalCounter(2);
        fConnect.SetCount(b ? 1 : 0);
        if (network == null)
        {
            network = new ThreadCheckNetwork();
            network.Run();
        }

        if (monitor == null)
        {
            monitor = new ThreadCheckMonitor();
            monitor.Run();
        }
        if (scan == null)
        {
            scan = new ThreadScan();
            scan.Run();
        }

    }

    //----------------------------------------------------------------------------------
    //
    //----------------------------------------------------------------------------------
    public static void Close()
    {
        try
        {
            if (network != null)
                network.close();
            if (monitor != null)
                monitor.close();
            if (scan != null)
                scan.close();

        } catch (IOException e)
        {
            e.printStackTrace();
        }
        network = null;
        monitor = null;

    }

    //----------------------------------------------------------------------------------
    //
    //----------------------------------------------------------------------------------
    public static void RegisterMonitor(BroadcastReceiver b)
    {
        if (monitor != null)
            monitor.Register(b);
    }

    //----------------------------------------------------------------------------------
    //
    //----------------------------------------------------------------------------------
    public static void UnRegisterMonitor(BroadcastReceiver b)
    {
        if (monitor != null)
            monitor.UnRegister(b);
    }

    //----------------------------------------------------------------------------------
    //
    //----------------------------------------------------------------------------------
    public static void RegisterScan(BroadcastReceiver b)
    {
        if (scan != null)
            scan.Register(b);
    }

    //----------------------------------------------------------------------------------
    //
    //----------------------------------------------------------------------------------
    public static void UnRegisterScan(BroadcastReceiver b)
    {
        if (scan != null)
            scan.UnRegister(b);
    }

    //----------------------------------------------------------------------------------
    //
    //----------------------------------------------------------------------------------
    static class ThreadCheckNetwork extends CStdThread {
        CNetwork network = null;

        @Override
        public void Run()
        {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
                super.Run();
            else
                OnRun();

        }
        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        @Override
        protected void OnRun()
        {
            network = new CNetwork(looper);
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
                Thread.currentThread().setUncaughtExceptionHandler(OutOfException());
        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        @Override
        public void close() throws IOException
        {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
                super.close();
            else
                OnClose();
        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        @NonNull
        private UncaughtExceptionHandler OutOfException()
        {

            return new UncaughtExceptionHandler() {

                @Override
                public void uncaughtException(Thread t, Throwable ex)
                {

                    try
                    {
                        StringWriter errors = new StringWriter();
                        ex.printStackTrace(new PrintWriter(errors));
                        String s = errors.toString();
                        ex.printStackTrace();
                        AddToLog(s);

                    } catch (Exception e1)
                    {

                        e1.printStackTrace();

                    }

                }

            };

        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        @Override
        protected void OnClose()
        {
            if (network != null)
            {
                try
                {
                    network.close();
                } catch (IOException e)
                {
                    e.printStackTrace();
                }
                network = null;
            }

        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        public void Start(final String s, final INetwork inet)
        {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
            {
                final CriticalSection sec = new CriticalSection();
                sec.Enter();
                CHandler h = new CHandler(looper);
                h.Send(new Runnable() {
                    @Override
                    public void run()
                    {
                        try
                        {
                            network.Start(s, inet);
                        } catch (Exception e)
                        {
                            AddToLog(e);
                        }
                        sec.Leave();
                    }

                });
                sec.Enter();
            } else
                network.Start(s, inet);
        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        public void End()
        {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
            {
                final CriticalSection sec1 = new CriticalSection();
                sec1.Enter();
                CHandler h = new CHandler(looper);
                h.Send(new Runnable() {
                    @Override
                    public void run()
                    {
                        try
                        {
                            network.End();
                        } catch (Exception e)
                        {
                            ;
                        }
                        sec1.Leave();
                    }
                });
                sec1.Enter();
            } else
                network.End();

        }

    }

    //*********************************************************************************
    //
    //*********************************************************************************
    static class ThreadCheckMonitor extends CStdThread {
        CMonitor network = null;

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        @Override
        protected void OnRun()
        {

            network = new CMonitor(looper);

        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        @Override
        protected void OnClose()
        {
            if (network != null)
            {
                try
                {
                    network.close();
                } catch (IOException e)
                {
                    e.printStackTrace();
                }
                network = null;
            }

        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        void Register(BroadcastReceiver b)
        {
            network.Register(b);
        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        void UnRegister(BroadcastReceiver b)
        {
            network.UnRegister(b);


        }

    }

    //*********************************************************************************
    //
    //*********************************************************************************
    static class ThreadScan extends CStdThread {
        CScan network = null;

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        @Override
        protected void OnRun()
        {

            network = new CScan(looper);
        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        @Override
        protected void OnClose()
        {
            if (network != null)
            {
                try
                {
                    network.close();
                } catch (IOException e)
                {
                    e.printStackTrace();
                }
                network = null;
            }

        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        void Register(BroadcastReceiver b)
        {

            network.Register(b);

        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        void UnRegister(BroadcastReceiver b)
        {
            network.UnRegister(b);


        }

    }


    //----------------------------------------------------------------------------------
    //
    //----------------------------------------------------------------------------------
    public static class CMonitor extends BroadcastReceiver implements Closeable {
        protected WifiManager manager;
        static ArrayList<BroadcastReceiver> receivers = new ArrayList<BroadcastReceiver>();
        protected Object sync = new Object();
        protected Looper looper;

        CMonitor(Looper _looper)
        {
            looper = _looper;
            manager = (WifiManager) App.GetInstance().getApplicationContext().getSystemService(Context.WIFI_SERVICE);
            IntentFilter intentFilter = new IntentFilter();
            intentFilter.addAction(WifiManager.SUPPLICANT_CONNECTION_CHANGE_ACTION);
            intentFilter.addAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
            intentFilter.addAction("android.net.conn.CONNECTIVITY_CHANGE");
            intentFilter.addAction(WifiManager.RSSI_CHANGED_ACTION);
            App.GetInstance().getApplicationContext().registerReceiver(this, intentFilter, null, new Handler(looper));

        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        public void Register(BroadcastReceiver b)
        {
            synchronized (sync)
            {
                int i = receivers.indexOf(b);
                if (i < 0)
                    receivers.add(b);
            }
        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        public void UnRegister(BroadcastReceiver b)
        {
            synchronized (sync)
            {
                int i = receivers.indexOf(b);
                if (i >= 0)
                    receivers.remove(b);
            }
        }

        public void Resume()
        {
        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        @Override
        public void onReceive(Context context, Intent intent)
        {
            synchronized (sync)
            {

                if (context == null || intent == null || AGlobals.currentActivity == null) return;
                final Context _context = context;
                final Intent _intent = intent;
                for (BroadcastReceiver e : receivers)
                {
                    final BroadcastReceiver ee = e;
                    AGlobals.rootHandler.Send(new Runnable() {
                        @Override
                        public void run()
                        {
                            int i = receivers.indexOf(ee);
                            if (i >= 0)
                                if (AGlobals.currentActivity != null)
                                    ee.onReceive(_context, _intent);


                        }
                    });

                }
            }

        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        @Override
        public void close() throws IOException
        {
            App.GetInstance().getApplicationContext().unregisterReceiver(this);
        }

    }

    //-----------------------------------------------------------------
    //
    //-----------------------------------------------------------------
    public static class CScan extends BroadcastReceiver implements Closeable {
        protected WifiManager manager;
        static ArrayList<BroadcastReceiver> receivers = new ArrayList<BroadcastReceiver>();
        protected Object sync = new Object();
        protected Looper looper;

        public CScan(Looper _looper)
        {
            looper = _looper;
            manager = (WifiManager) App.GetInstance().getApplicationContext().getSystemService(Context.WIFI_SERVICE);
            IntentFilter intent = new IntentFilter();
            intent.addAction(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION);
            App.GetInstance().getApplicationContext().registerReceiver(this, intent, null, new Handler(looper));

        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        public void UnRegister(BroadcastReceiver b)
        {
            synchronized (sync)
            {
                if (receivers.size() > 0)
                {
                    int i = receivers.indexOf(b);
                    if (i >= 0)
                        receivers.remove(b);
                }
            }
        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        @Override
        public void onReceive(Context context, Intent intent)
        {
            synchronized (sync)
            {

                if (context == null || intent == null || AGlobals.currentActivity == null) return;
                final Context _context = context;
                final Intent _intent = intent;
                for (BroadcastReceiver e : receivers)
                {
                    final BroadcastReceiver ee = e;
                    AGlobals.rootHandler.Send(new Runnable() {
                        @Override
                        public void run()
                        {
                            int i = receivers.indexOf(ee);
                            if (i >= 0)
                                if (AGlobals.currentActivity != null)
                                    ee.onReceive(_context, _intent);


                        }
                    });

                }
                if (receivers.size() > 0)
                    (new Handler()).postDelayed(new Runnable() {
                        @Override
                        public void run()
                        {
                            manager.startScan();
                        }
                    }, 3000);


            }

        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        @Override
        public void close() throws IOException
        {
            App.GetInstance().getApplicationContext().unregisterReceiver(this);
        }

        public void Resume()
        {
            synchronized (sync)
            {

                if (AGlobals.currentActivity != null && receivers.size() > 0)
                    manager.startScan();
            }
        }

        public void Register(BroadcastReceiver b)
        {
            synchronized (sync)
            {
                int i = receivers.size() > 0 ? receivers.indexOf(b) : -1;
                if (i < 0)
                    receivers.add(b);
                if (receivers.size() == 1)
                    manager.startScan();
            }
        }

    }

    //**********************************************************************************
    //
    //***********************************************************************************
    static class CNetwork extends ConnectivityManager.NetworkCallback implements Closeable {
        ConnectivityManager connectivityManager;
        static String netSSID = null;
        static Network net = null;
        static INetwork receiver = null;
        Looper loop = null;
        ActionCallback onStop = null;
        public CNetwork(Looper looper)
        {
            this(looper, null);
        }
        public void SetOnStop(ActionCallback _onStop)
        {
            onStop = _onStop;
        }
        void Stop()
        {
            if (onStop != null)
                onStop.run();
        }
        public CNetwork(Looper looper, ActionCallback _onStop)
        {
            onStop = _onStop;
            connectivityManager = (ConnectivityManager) App.GetInstance().getApplicationContext().getSystemService(Context.CONNECTIVITY_SERVICE);
            NetworkRequest.Builder builder;
            builder = new NetworkRequest.Builder();
            builder.addTransportType(NetworkCapabilities.TRANSPORT_WIFI);
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
            {
                connectivityManager.registerNetworkCallback(builder.build(), this, new Handler(looper));
            } else
                connectivityManager.registerNetworkCallback(builder.build(), this);
        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        public void Start(String _netSSID, INetwork _receiver)
        {
            net = null;

            netSSID = _netSSID;
            receiver = _receiver;
        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        public void End()
        {
            netSSID = null;
            net = null;
            receiver = null;
        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        @Override
        public void onAvailable(Network network)
        {
            try
            {
                fConnect.SetCount(1);
                WifiManager manager = (WifiManager) App.GetInstance().getApplicationContext().getSystemService(Context.WIFI_SERVICE);
                WifiInfo info = null;
                if (AGlobals.currentActivity == null || manager == null || receiver == null)
                {

                    BindNetwork(network);
                    return;
                }
                info = manager.getConnectionInfo();
                if (info != null)
                {
                    final String ssid = info.getSSID();
                    final Network _network = network;
                    if (netSSID != null)
                    {
                        if (ssid != null && ssid.equals(netSSID))
                        {
                            net = network;


                            BindNetwork(network);
                            AddToLog("Set default network " + ssid);
                            AGlobals.rootHandler.Send(new Runnable() {
                                //-----------------------------------------
                                @Override
                                public void run()
                                {
                                    if (AGlobals.currentActivity != null)
                                        Toast.makeText(AGlobals.currentActivity, "Set default network " + ssid, Toast.LENGTH_SHORT).show();

                                }
                            });

                        } else
                            BindNetwork(net);
                        AGlobals.rootHandler.Send(new Runnable() {
                            //-----------------------------------------
                            @Override
                            public void run()
                            {

                                if (receiver != null)
                                    if (AGlobals.currentActivity != null)
                                        receiver.onAvailable(netSSID, _network);

                            }
                        });

                    } else
                    {
                        BindNetwork(network);
                    }
                }


            } catch (Exception e)
            {
                AddToLog(e);
            }
        }

        @Override
        public void onLosing(Network network, int maxMsToLive)
        {
            fConnect.SetCount(0);
            super.onLosing(network, maxMsToLive);
        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        @Override
        public void onLost(Network network)
        {
            Stop();
            net = null;
            fConnect.SetCount(0);
            super.onLost(network);
            final Network _network = network;

            if (AGlobals.rootHandler != null)
            {

                AGlobals.rootHandler.Send(new Runnable() {
                    //-----------------------------------------
                    @Override
                    public void run()
                    {
                        if (receiver != null)
                            if (AGlobals.currentActivity != null)
                                receiver.onLost(_network);
                        Stop();
                    }
                });

            }
        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        protected void BindNetwork(Network network)
        {

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)
                connectivityManager.bindProcessToNetwork(network);
            else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP)
                ConnectivityManager.setProcessDefaultNetwork(network);
        }

        //----------------------------------------------------------------------------------
        //
        //----------------------------------------------------------------------------------
        @Override
        public void close() throws IOException
        {

            connectivityManager.unregisterNetworkCallback(this);
        }
    }

}
