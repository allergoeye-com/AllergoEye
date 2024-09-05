package com.dryeye.app.card;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.graphics.Point;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.coordinatorlayout.widget.CoordinatorLayout;

import com.google.android.material.floatingactionbutton.FloatingActionButton;

import androidx.fragment.app.Fragment;

import android.util.DisplayMetrics;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.dryeye.app.App;
import com.dryeye.app.R;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import lib.OrientActivity;
import lib.common.ActionCallback;
import lib.common.IResultFragmentDlg;
import lib.common.OnClickCalback;
import lib.common.button.CMenuButton;
import lib.common.ImageSlider;
import lib.common.layout.CPanelDialog;
import lib.explorer.common.DirEntry;
import lib.utils.AGlobals;
import lib.utils.BitmapUtils;
import lib.utils.CDateTime;
import lib.utils.FileUtils;


@SuppressLint("ValidFragment")
public class FraggmentCardImages extends Fragment implements IResultFragmentDlg {
    View self;

    Runnable runnable = null;
    String path = null;
    CPanelDialog test;
    ImageSlider slider;
    ActionCallback onexit;

    public FraggmentCardImages(String _path, ActionCallback _onexit)
    {
        super();
        path = _path;
        onexit = _onexit;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
    {
        try
        {
            test = ((CPanelDialog) container);
            test.SetOrient("Top");
            test.DisableRequestLayout(true);
            test.fAutoClosed = false;
            self = inflater.inflate(R.layout.list_photo, container, false);
            slider = self.findViewById(R.id.idSlider);
            DisplayMetrics metrics = AGlobals.GetDisplayMetrcs();
            self.setLayoutParams(new CoordinatorLayout.LayoutParams(metrics.widthPixels, metrics.heightPixels));
            self.measure(0, 0);
            int x = self.getMeasuredWidth();
            int y = self.getMeasuredHeight();
       //     TextView txt = self.findViewById(R.id.textSelect);
          //  String str = AGlobals.ReadString("IDS_SELECT_IMAGE");
         //   if (str != null)
          //      txt.setText(str);

            //   View v1 = getActivity().findViewById(R.id.AppBar);
            // int n = v1 == null ? 0 : v1.getBottom();

            //   DisplayMetrics metrics = AGlobals.GetDisplayMetrcs();
            // self.setLayoutParams(new LinearLayout.LayoutParams(metrics.widthPixels, metrics.heightPixels - n));
            //self.measure(0, 0);
            container.setTranslationY(0);
            container.setTranslationX(0);
            //test.requestLayout();
            CMenuButton mb = self.findViewById(R.id.idCancel);
            mb.SetCallback(this, "OnCancel");

            test.SetCallbacks(new ActionCallback(this, "OnInitDialog"), new ActionCallback(this, "OnDestroyDialog"));

            test.Start();
            FloatingActionButton fab = (FloatingActionButton) self.findViewById(R.id.idOk);
            fab.setOnClickListener(new OnClickCalback(this, "OnOk"));


        } catch (Exception p)
        {
            p.printStackTrace();
        }


        return self;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState)
    {

        InitCard();

    }

    private void InitCard()
    {


        try
        {
            ArrayList<DirEntry> dirEntries = new ArrayList<DirEntry>();
            String currentPath = null;
            currentPath = App.cardPath.Dir();

            if (App.cardPath.Dir() != null)
        {
            String[] mask = new String[1];
            mask[0] = ".jpg";
            File dir = new File(currentPath);
            ArrayList<String> path = new ArrayList<String>();
            FileUtils.GetFileList(dir, mask, false, path);
            int len = path.size();
            for (int i = 0; i < len; ++i)
            {
                DirEntry entry = new DirEntry();
                String str = path.get(i);
                int last_i = str.lastIndexOf(".");
                int first_i = str.lastIndexOf("/");
                entry.img.url = str;
                entry.title = CDateTime.ConvertTime(str.substring(first_i + 1, last_i));
                if (str.contains(".jpg"))
                {
                    String filename = str.replace(".jpg", ".res");
                    ArrayList<String> t = new ArrayList<String>();
                    FileUtils.ReadText(filename, t);
                    if (t.size() > 0)
                        entry.info = t.get(0);
                }
                Point dm = null;
                try
                {
                    dm = BitmapUtils.Dimensions(str);
                    entry.img.width = dm.x;
                    entry.img.height = dm.y;
                } catch (IOException e)
                {
                    e.printStackTrace();
                }

                entry.thumb.url = str;
                entry.thumb.width = dm.x;
                entry.thumb.height = dm.y;
                dirEntries.add(entry);
            }
        }

        slider.Init(dirEntries, 0, null);
        } catch (CException e)
        {
            e.printStackTrace();
        }

    }

    public void OnOk(View v)
    {
        final String s = slider.Result();
        final Activity ac = getActivity();

        if (onexit != null)
            runnable = new ActionCallback() {

                @Override
                public void run()
                {
                    onexit.run(ac, s);
                }
            };
        ((OrientActivity) getActivity()).DialogHide();

    }

    public void OnCancel()
    {
        slider.Reset();
        ((OrientActivity) getActivity()).DialogHide();
    }


    public void OnInitDialog()
    {

    }

    public void OnDestroyDialog()
    {
        ImageSlider slider = self.findViewById(R.id.idSlider);
        ((OrientActivity) getActivity()).CloseDialog();

    }

    @Override
    public Runnable GetResult()
    {
        return runnable;
    }
}
