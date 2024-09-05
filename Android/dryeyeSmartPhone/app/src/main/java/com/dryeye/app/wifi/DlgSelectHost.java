package com.dryeye.app.wifi;

import android.content.Context;
import android.content.res.ColorStateList;
import android.graphics.PorterDuff;
import android.graphics.drawable.Drawable;
import android.net.ConnectivityManager;

import android.net.NetworkInfo;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Handler;

import androidx.annotation.NonNull;
import androidx.core.content.ContextCompat;
import androidx.core.graphics.drawable.DrawableCompat;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.util.DisplayMetrics;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.dryeye.app.R;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

import lib.common.ActionCallback;
import lib.common.CDialog;
import lib.common.CHandler;
import lib.common.EventIdle;
import lib.common.OnClickCalback;
import lib.utils.AGlobals;

public class DlgSelectHost extends CDialog {
    protected RecyclerView recyclerView = null;
    public AddapterListScan adapter;
    View view;

    public DlgSelectHost()
    {
        super(AGlobals.currentActivity);

        // state.EnableWiFi();
        view = LayoutInflater.from(getContext()).inflate(R.layout.dlg_select_host, null);
        DisplayMetrics metrics = AGlobals.GetDisplayMetrcs();
        view.setLayoutParams(new LinearLayout.LayoutParams((int) ((double) metrics.widthPixels / 1.5), (int) ((double) metrics.heightPixels / 2.0)));
        final ConnectivityManager cm = (ConnectivityManager) AGlobals.contextApp.getSystemService(Context.CONNECTIVITY_SERVICE);
        final NetworkInfo activeNetwork = cm.getActiveNetworkInfo();
        GridLayoutManager gridLayoutManager = new GridLayoutManager(getContext(), 1);
        recyclerView = (RecyclerView) view.findViewById(R.id.listWiFi);
        //InitRCButton(view, R.id.idScan, "OnScan");
        // AGlobals.currentActivity.getString(R.string.cancel);
        // ((Button) view.findViewById(R.id.idOk)).setText("Connect");
        // ((Button) view.findViewById(R.id.idCancel)).setText("Close");
        InitRCButton(view, R.id.idOk, "OnConnect");
        InitRCButton(view, R.id.idCancel, "OnCancel");
        view.findViewById(R.id.exploreWiFi).setVisibility(View.VISIBLE);
        recyclerView.setHasFixedSize(true);
        recyclerView.setLayoutManager(gridLayoutManager);
        adapter = new AddapterListScan(null);
        recyclerView.setAdapter(adapter);
        this.setView(view);
        Lock(true);

    }

    public void OnInitDialog()
    {
        adapter.Enable(true);
        adapter.Scan(idle);

    }

    public void OnCancel(View v)
    {
        Result = IDCANCEL;
        adapter.Enable(false);
        recyclerView.setAdapter(null);
        Lock(false);
        MdnsUtils.StopMDNS();
        dismiss();
    }

    public void OnConnect(View v)
    {
        AddapterListScan.Comp res = adapter.Result();
        if (res == null) return;
        WifiManager manager = (WifiManager) AGlobals.contextApp.getSystemService(Context.WIFI_SERVICE);
        WifiInfo info = manager.getConnectionInfo();
        String tmp = info.getSSID();
        SyncThread.SaveProfile(new SyncThread.Profile(tmp.substring(1, tmp.length() - 1), res.Name));
        adapter.Enable(false);
        recyclerView.setAdapter(null);
        Lock(false);
        EndDialog(IDOK);

    }

    public class AddapterListScan extends RecyclerView.Adapter<AddapterListScan.AdapterListWiFiHolder> {
        RecyclerView View = null;
        boolean fEnableScan = false;


        public ArrayList<Comp> results;
        public int selectedPos = -1;
        //ProgressBar waitCursor;

        int count = 0;

        //------------------------------------------------
        //
        //------------------------------------------------
        public AddapterListScan(ProgressBar _waitCursor)
        {
            //  waitCursor = _waitCursor;
            results = new ArrayList<Comp>();

        }

        //------------------------------------------------
        //
        //------------------------------------------------
        boolean fInFunc = false;

        public void FindHost(MDnsInfo info)
        {
            if (info == null)
            {
                //     waitCursor.setVisibility(android.view.View.GONE);
            }
            if (!fEnableScan || info == null) return;
            if (fInFunc)
            {
                final MDnsInfo _info = info;
                (new Handler()).post(new Runnable() {
                    @Override
                    public void run()
                    {
                        FindHost(_info);
                    }
                });
                return;
            }
            fInFunc = true;

            String name = info.name;
            for (Comp a : results)
            {
                if (a.Name.equals(name))
                {
                    fInFunc = false;
                    return;
                }
            }
            Comp sel = null;
            ArrayList<Comp> res = new ArrayList<Comp>();
            res.addAll(results);

            if (selectedPos >= 0)
                sel = results.get(selectedPos);
            res.add(new Comp(name, info.ip));
            UpdateList(res, sel);
            notifyDataSetChanged();
            fInFunc = false;

        }

        //------------------------------------------------
        //
        //------------------------------------------------
        public void Enable(boolean b)
        {
            if (fEnableScan == b) return;
            fEnableScan = b;
            if (!b)
            {
                MdnsUtils.StopMDNS();
            }

        }

        //------------------------------------------------
        //
        //------------------------------------------------
        void UpdateList(ArrayList<Comp> res, Comp sel)
        {

            int i, l;
            Collections.sort(res, cmp);
            if (sel != null)
            {
                for (i = 0, l = res.size(); i < l; ++i)
                {
                    Comp p = res.get(i);
                    if (sel.equals(p))
                    {
                        selectedPos = i;
                        sel = null;
                    }
                }
                if (sel != null)
                    selectedPos = -1;
            }
            if (res.size() == results.size())
            {
                try
                {
                    for (i = 0, l = res.size(); i < l; ++i)
                    {
                        if (!res.get(i).equals(results.get(i)))
                            break;
                    }
                    if (i == l) return;
                } catch (Exception e)
                {
                    ;
                }
            }
            results = res;
            notifyDataSetChanged();

        }

        //------------------------------------------------
        //
        //------------------------- -----------------------
        public void Clear()
        {
            results.clear();
            selectedPos = -1;
            notifyDataSetChanged();

        }

        //---------------------------------------
        //
        //---------------------------------------
        public Comp Result()
        {
            return selectedPos >= 0 && selectedPos < results.size() ? results.get(selectedPos) : null;
        }

        public void Scan(EventIdle idle)
        {
            if (this.fEnableScan)
            {
                MdnsUtils.Stop();
                MdnsUtils.StartMDNS(-1, new CHandler(idle.GetLooper()), new ActionCallback(this, "FindHost", MDnsInfo.class));
            }


        }

        //--------------------------------------------------------------------------------------
        //
        //--------------------------------------------------------------------------------------
        @Override
        public void onAttachedToRecyclerView(RecyclerView recyclerView)
        {
            super.onAttachedToRecyclerView(recyclerView);
            View = recyclerView;

        }

        @Override
        public void onDetachedFromRecyclerView(RecyclerView recyclerView)
        {
            MdnsUtils.StopMDNS();
            super.onDetachedFromRecyclerView(recyclerView);
        }
        //--------------------------------------------------------
        //
        //--------------------------------------------------------

        @NonNull
        @Override
        public AddapterListScan.AdapterListWiFiHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType)
        {
            android.view.View v = LayoutInflater.from(parent.getContext()).inflate(R.layout.dialog_host_list, parent, false);
            AddapterListScan.AdapterListWiFiHolder vh = new AddapterListScan.AdapterListWiFiHolder(v);
            return vh;

        }

        @Override
        public void onBindViewHolder(@NonNull AddapterListScan.AdapterListWiFiHolder holder, int position)
        {

            if (results != null && results.size() > position)
            {
                AddapterListScan.Comp comp = results.get(position);
                ImageView view = holder.imageView;
                holder.name.setText(comp.Name);
                holder.status.setText(comp.IP);
                holder.v.setSelected(position == selectedPos);
                String uri = "android:drawable/radiobutton_off_background";
                int imageResource = AGlobals.contextApp.getResources().getIdentifier(uri, null, AGlobals.contextApp.getPackageName());
                Drawable drawable = ContextCompat.getDrawable(AGlobals.contextApp, imageResource);

                drawable = DrawableCompat.wrap(drawable);
                ColorStateList csl = new ColorStateList(
                        new int[][]{new int[]{android.R.attr.state_enabled, android.R.attr.state_selected},
                                new int[]{}}, new int[]{0xFF00FF00, 0xFF00FF00});
                DrawableCompat.setTintList(drawable, csl);
                DrawableCompat.setTintMode(drawable, PorterDuff.Mode.SRC_IN);
                view.setImageDrawable(drawable);
            }
        }

        @Override
        public int getItemCount()
        {
            return results.size();
        }

        //--------------------------------------------------------
        //
        //--------------------------------------------------------
        class AdapterListWiFiHolder extends RecyclerView.ViewHolder {

            android.view.View v;
            TextView name;
            TextView status;
            ImageView imageView;

            //--------------------------------------------------------------------------------------
            //
            //--------------------------------------------------------------------------------------
            AdapterListWiFiHolder(View itemView)
            {
                super(itemView);
                v = itemView;
                name = (TextView) itemView.findViewById(R.id.nameDevice);
                status = (TextView) itemView.findViewById(R.id.statusDevice);
                imageView = itemView.findViewById(R.id.imageView);
                itemView.setOnClickListener(new OnClickCalback(this, "OnClick"));
            }

            public void OnClick(View v)
            {
                int oldpos = selectedPos;
                int pos = getLayoutPosition();
                if (selectedPos == pos)
                {
                    ;
                } else
                {
                    selectedPos = pos;
                    if (oldpos >= 0)
                        notifyItemChanged(oldpos);
                    notifyItemChanged(pos);
                }
            }

        }

        class Comp {
            public String Name;
            public String IP;

            public Comp()
            {
                Name = new String();
                IP = null;

            }

            public Comp(String s1, String s2)
            {
                Name = s1;
                IP = s2;
            }

            @Override
            public boolean equals(Object ob)
            {
                try
                {
                    Comp o = (Comp) ob;
                    if (Name.equals(o.Name))
                    {
                        return IP.equals(o.IP);
                    }

                } catch (Exception e)
                {
                    ;
                }
                return false;
            }


        }

        ;
        public Comparator<Comp> cmp = new Comparator<Comp>() {
            public int compare(Comp s1, Comp s2)
            {
                String t1 = s1.Name;
                String t2 = s2.Name;
                return t1.compareTo(t2);
            }
        };
    }

}
