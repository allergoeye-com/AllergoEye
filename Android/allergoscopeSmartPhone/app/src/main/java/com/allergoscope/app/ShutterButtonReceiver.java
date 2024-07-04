package com.allergoscope.app;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.widget.Toast;

import androidx.media.session.MediaButtonReceiver;

public class ShutterButtonReceiver  extends BroadcastReceiver {
    @Override
    public void onReceive(Context context, Intent intent) {
            Toast.makeText(context, "debug media button test", Toast.LENGTH_LONG).show();
    }
}
