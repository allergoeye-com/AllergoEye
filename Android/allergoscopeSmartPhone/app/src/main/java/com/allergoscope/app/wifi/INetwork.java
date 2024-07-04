package com.allergoscope.app.wifi;

import android.net.Network;

public interface INetwork {
    void onAvailable(String name, Network network);

    void onLosing(Network network);

    void onLost(Network network);

}
