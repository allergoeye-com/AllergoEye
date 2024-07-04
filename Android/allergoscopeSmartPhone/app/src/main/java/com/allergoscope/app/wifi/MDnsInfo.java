package com.allergoscope.app.wifi;

public class MDnsInfo {

    public String ip;
    public String name;
    public int num;
    public String[] records;


    public MDnsInfo(String _ip, String[] all)
    {
        ip = _ip;
        name = all[0];
        num = Integer.valueOf(all[2]);
        if (all.length > 4)
        {
            records = new String[all.length - 3];
            for (int k = 0, i = 3; i < all.length; ++i, ++k)
                records[k] = all[i];
        } else
            records = null;

    }

}
