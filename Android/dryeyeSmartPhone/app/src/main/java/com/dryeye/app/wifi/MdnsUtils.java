package com.dryeye.app.wifi;


import android.os.Handler;

import java.io.IOException;


import lib.common.ActionCallback;
import lib.common.CHandler;

import lib.utils.WiFiUtils;


public class MdnsUtils extends WiFiUtils {
    private static MDNSServerClient mdns = null;

    public static void Run()
    {
        mdns = new MDNSServerClient();
        mdns.Run();
    }
    //-------------------------------------
    //
    //-------------------------------------
    public static void Stop()
    {
        if (mdns != null)
        {
            try
            {
                mdns.close();
            } catch (IOException e)
            {
                e.printStackTrace();
            }
            mdns = null;
        }
    }
    //-------------------------------------
    //
    //-------------------------------------
    static public void StartMDNS(final int iTimoutMDNSFind, final CHandler looper, ActionCallback calback)
    {
        StartMDNS(iTimoutMDNSFind, looper, calback, null);
    }

    //-------------------------------------
    //
    //-------------------------------------
    static public boolean MDNSIsRun()
    {
        return mdns.IsRun();
    }

    //-------------------------------------
    //
    //-------------------------------------
    static public void MDNSPause()
    {
        mdns.Pause();
    }

    static public void MDNSChangeHostName()
    {
        mdns.ChangeHostName();
    }

    static public void MDNSChangeOptions()
    {
        mdns.ChangeOptions();
    }
    static public void OnSync()
    {
        ++mdns.Number;

    }

    //-------------------------------------
    //
    //-------------------------------------
    static public void StartMDNS(final int iTimoutMDNSFind, final CHandler looper, ActionCallback calback, String s)
    {
        if (!WiFiIsConnected()) return;
        if (mdns == null)
        {
            mdns = new MDNSServerClient();
            mdns.Run();
        }
        if (!WiFiIsConnected())
        {
            final ActionCallback c = calback;
            if (c != null)
                (new Handler()).post(new Runnable() {
                    @Override
                    public void run()
                    {
                        c.run((String) null);
                    }
                });
            return;
        }
        mdns.Close();

        mdns.Run(iTimoutMDNSFind, looper, calback, s);
    }

    //-------------------------------------
    //
    //-------------------------------------
    static public void StopMDNS()
    {
        if (mdns != null)
            mdns.Close();


    }


}
