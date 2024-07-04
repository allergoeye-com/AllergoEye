package com.allergoscope.app.wifi;

import android.net.Uri;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import com.allergoscope.app.R;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;

import javax.net.ssl.HttpsURLConnection;

import lib.OrientActivity;
import lib.common.ActionCallback;
import lib.common.CStdThread;
import lib.common.IResultFragmentDlg;
import lib.common.layout.CPanelDialog;
import lib.common.layout.ProgressLayout;
import lib.utils.AGlobals;
import lib.utils.FileUtils;

public class FragmentDownloadFile extends Fragment implements IResultFragmentDlg {
    View self;
    CPanelDialog test;
    TextView idNameText;
    String link;
    static protected final String URI_CHARS = "@#&=*+-_.,:!?()/~'%";
    static protected final int REDIRECT_COUNT = 3;
    static protected final int CONNECT_TIMEOUT = 5000;
    static protected final int READ_TIMEOUT = 20000;
    protected InputStream pStream = null;
    protected HttpURLConnection pHttp = null;
    protected int iSizeStream = 0;
    public boolean isSownload = false;
    public byte [] buffer = null;
    String text = null;
    ActionCallback callback = null;
    Runnable result = null;
    DownloadThread thread = new DownloadThread();

    public FragmentDownloadFile(String _link, String _text, ActionCallback _callback)
    {
        super();
        link = _link;
        text = _text;
        callback = _callback;

    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
    {
        try
        {
            test = ((CPanelDialog) container);
            test.SetOrient("Right");

            test.DisableRequestLayout(true);
            test.SetCallbacks(new ActionCallback(this, "OnInitDialog"), new ActionCallback(this, "OnDestroyDialog"));
            test.AddOnShowAction(new ActionCallback(this, "OnShow"));
            test.fAutoClosed = false;
            self = inflater.inflate(R.layout.dlg_sync, container, false);
            DisplayMetrics metrics = AGlobals.GetDisplayMetrcs();
            self.setLayoutParams(new LinearLayout.LayoutParams(metrics.widthPixels, metrics.heightPixels));
            LinearLayout barWait;
            barWait = self.findViewById(R.id.waitProcessing);
            barWait.setVisibility(View.INVISIBLE);
            self.measure(0, 0);
            container.setTranslationY(0);
            container.setTranslationX(0);


        } catch (Exception p)
        {
            p.printStackTrace();
        }
        return self;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState)
    {
        idNameText = self.findViewById(R.id.idWaitText);
        idNameText.setText(text);
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
        thread.Run();

    }


    public void OnInitDialog()
    {
    }

    public void OnDestroyDialog()
    {
        result = new Runnable() {
            @Override
            public void run()
            {
                callback.run(buffer);
            }
        };
        ((OrientActivity) getActivity()).CloseDialog();

    }

    @Override
    public Runnable GetResult()
    {
        return result;
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

    class DownloadThread extends CStdThread {
        //--------------------------------------------------
//
//--------------------------------------------------
        protected HttpURLConnection Connection(String url) throws IOException
        {

            String encodedUrl = Uri.encode(url, URI_CHARS);
            HttpURLConnection con = (HttpURLConnection) new URL(url).openConnection();
            boolean isHTTPs = HttpsURLConnection.class.isAssignableFrom(con.getClass());
            con.setConnectTimeout(CONNECT_TIMEOUT);
            con.setReadTimeout(READ_TIMEOUT);
            return con;
        }
        //--------------------------------------------------
//
//--------------------------------------------------
        protected boolean Start(String url) throws IOException
        {
            try {
                HttpURLConnection con1, con = Connection(url);
                int redirectCount = 0;
                int res = con.getResponseCode();
                while (con.getResponseCode()/100 == 3 && redirectCount < REDIRECT_COUNT)
                {
                    if (Thread.interrupted()) return false;
                    con1 = Connection(con.getHeaderField("Location"));
                    con.disconnect();
                    con = con1;
                    redirectCount++;
                }
                InputStream imageStream = con.getInputStream();
                if (con.getResponseCode() != HttpURLConnection.HTTP_OK)
                    return false;
                pHttp = con;
                pStream = imageStream;

                iSizeStream = con.getContentLength();
            }catch(Exception e)
            {
                FileUtils.AddToLog(e);
            }
            return true;
        }
        //--------------------------------------------------
//
//--------------------------------------------------
        protected boolean Stop() throws IOException {
            if (pStream != null)
                pStream.close();
            if (pHttp != null)
                pHttp.disconnect();
            pStream = null;
            pHttp = null;
            return true;
        }


        @Override
        protected void OnRun()
        {
            try {
                int k, n = 0;
                int BUFFER_SIZE = 64  * 1024;
                byte b[] = null;
                if (Start(link))
                {
                    if (iSizeStream > 0)
                    {
                        b = new byte[iSizeStream];
                        int dwRead = iSizeStream > BUFFER_SIZE ?  BUFFER_SIZE : iSizeStream;
                        while((k = pStream.read(b, n, dwRead)) > 0)
                        {
                            n += k;
                            dwRead = iSizeStream - n > BUFFER_SIZE ?  BUFFER_SIZE : iSizeStream - n;
                        }
                    }
                    else
                    {
                        byte[] buffer = new byte[1024];
                        int size;
                        ByteArrayOutputStream out = new ByteArrayOutputStream();

                        while ((size = pStream.read(buffer)) != -1) {
                            out.write(buffer, 0, size);
                        }
                        b = out.toByteArray();
                        out.close();


                    }
                    Stop();
                    isSownload = n  == iSizeStream;
                    buffer = b;


                }
            }catch (IOException e) {

                FileUtils.AddToLog(e);
            }
            AGlobals.rootHandler.Send(new Runnable() {
                @Override
                public void run()
                {
                    try
                    {
                        close();
                        ((OrientActivity)AGlobals.currentActivity).DialogHide();
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
