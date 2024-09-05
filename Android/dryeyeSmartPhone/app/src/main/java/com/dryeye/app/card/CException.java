package com.dryeye.app.card;

import android.os.Build;

import androidx.annotation.RequiresApi;

import lib.utils.FileUtils;

public class CException extends Exception {
    public CException()
    {
        super();
        FileUtils.AddToLog(this);
    }
    public CException(String message) {
        super(message);
        FileUtils.AddToLog(this);
    }
    public CException(String message, Throwable cause) {
        super(message, cause);
        FileUtils.AddToLog(this);
    }
    public CException(Throwable cause) {
        super(cause);
        FileUtils.AddToLog(this);
    }
    @RequiresApi(api = Build.VERSION_CODES.N)
    protected CException(String message, Throwable cause,
                         boolean enableSuppression,
                         boolean writableStackTrace) {
        super(message, cause, enableSuppression, writableStackTrace);
        FileUtils.AddToLog(this);
    }
}
