package com.allergoscope.app.roi;

import android.annotation.SuppressLint;
import android.graphics.Bitmap;
import android.os.Bundle;

import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.util.DisplayMetrics;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.ImageView;

import com.allergoscope.app.CView;
import com.allergoscope.app.R;
import com.allergoscope.app.ViewActivity;

import lib.colorpicker.ModalColorDialog;
import lib.common.ActionCallback;
import lib.common.IResultFragmentDlg;
import lib.common.button.CMenuButton;
import lib.common.layout.CPanelDialog;
import lib.utils.AGlobals;

import static lib.common.CDialog.IDOK;

@SuppressLint("ValidFragment")
public class FragmentROI extends Fragment implements IResultFragmentDlg {
    View self;
    SelectedBlockIcon data;
    SelectedList adapter;
    int Nstart;
    CView document;

    class MResult extends ActionCallback {
        SelectedBlockIcon data;
        CView document;

        MResult(CView doc, SelectedBlockIcon _data)
        {
            document = doc;
            data = _data;

        }

        @Override
        public void run()
        {
            document.UpdateRoi(data);
            document.Invalidate();
        }
    }

    MResult result = null;

    @Override
    public Runnable GetResult()
    {

        return result;
    }

    RecyclerView recyclerViewSelected;
    LinearLayoutManager linearLayoutManager;

    public FragmentROI(CView doc, SelectedBlockIcon _icons)
    {
        super();
        document = doc;

        data = _icons;
    }

    //---------------------------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------------------------
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
    {
        try
        {
            CPanelDialog test = ((CPanelDialog) container);
            test.SetOrient("Top");

            test.Start();
            test.SetCallbacks(new ActionCallback(this, "OnInitDialog"), new ActionCallback(this, "OnDestroyDialog"));

        } catch (Exception p)
        {
            p.printStackTrace();
        }
        self = inflater.inflate(R.layout.dlg_list_roi, container, false);
        DisplayMetrics metrics = AGlobals.GetDisplayMetrcs();
        float size = metrics.widthPixels / data.Count(); //TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 200, metrics);
        int n = (int) (metrics.widthPixels / size);
        Nstart = 0;
        if (n < data.names.length)
            Nstart = n;

        recyclerViewSelected = (RecyclerView) self.findViewById(R.id.recyclerViewHoops);
        linearLayoutManager = new LinearLayoutManager(getActivity(), LinearLayoutManager.HORIZONTAL, false);
        recyclerViewSelected.setLayoutManager(linearLayoutManager);
        adapter = new SelectedList();
        recyclerViewSelected.setAdapter(adapter);
        container.measure(0, 0);
        self.measure(0, 0);
        int w = self.getMeasuredWidth();
        int h = self.getMeasuredHeight();
        int l[] = new int[2];
        View v1 = getActivity().findViewById(R.id.viewMenuButton);
        v1.getLocationOnScreen(l);
        int x = 0;
        int y = v1.getBottom();
        container.setTranslationY(y);
        container.setTranslationX(x);
        Nstart = 0;
        return self;
    }

    //---------------------------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------------------------
    public void OnInitDialog()
    {
        InitControls();
        adapter.notifyDataSetChanged();


    }

    //---------------------------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------------------------
    public void OnDestroyDialog()
    {
        int i = 0;


        recyclerViewSelected.setAdapter(null);
        ((ViewActivity) getActivity()).CloseDialog();
    }


    //---------------------------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------------------------
    void InitControls()
    {
        CMenuButton button = getView().findViewById(R.id.idcancel);
        button.SetCallback(this, "OnCancel");
        button = getView().findViewById(R.id.idok);
        button.SetCallback(this, "OnOk");
    }

    public void OnOk()
    {
        result = new MResult(document, data);
        ((ViewActivity) getActivity()).DialogHide();

    }

    public void OnCancel()
    {
        for (int i = 0; i < data.fSel.length; ++i)
            data.fSel[i] = false;
        ((ViewActivity) getActivity()).DialogHide();

    }

    //--------------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------------
    void OnSelect(int iItem)
    {
        data.fSel[iItem] = !data.fSel[iItem];

    }

    boolean OnColor(int iItem)
    {
        int color = data.colors[iItem];
        ModalColorDialog dlg = new ModalColorDialog(color);
        dlg.SetCurrent(color);
        if (dlg.DoModal() == IDOK)
        {
            data.colors[iItem] = dlg.colorResult;
            document.UpdateRoiIcon(data.bmp[iItem], data.names[iItem], dlg.colorResult);
            return true;
        }
        return false;

    }


    //*****************************************************************************
    class SelectedList extends RecyclerView.Adapter<SelectedList.SelectedHolder> {
        int lastVisibleFirst = 0;
        int lastVisibleLast = 0;

        //--------------------------------------------------------------------------------------
        //
        //--------------------------------------------------------------------------------------
        @Override
        public void onAttachedToRecyclerView(RecyclerView recyclerView)
        {
            super.onAttachedToRecyclerView(recyclerView);

        }

        public void SetVisibleRange(int position)
        {
            {
                lastVisibleFirst = linearLayoutManager.findFirstVisibleItemPosition();
                lastVisibleLast = linearLayoutManager.findLastVisibleItemPosition();

            }
        }

        //--------------------------------------------------------------------------------------
        //
        //--------------------------------------------------------------------------------------
        @Override
        public SelectedHolder onCreateViewHolder(ViewGroup parent, int viewType)
        {


            View v = LayoutInflater.from(parent.getContext()).inflate(R.layout.selected_view_item, parent, false);
            SelectedHolder vh = new SelectedHolder(v);

            vh.SetListner();
            return vh;
        }

        //--------------------------------------------------------------------------------------
        //
        //--------------------------------------------------------------------------------------
        @Override
        public void onBindViewHolder(SelectedHolder holder, int position)
        {
            if (data.bmp.length <= position) return;
            SetVisibleRange(position);


            Bitmap o = data.bmp[position];
            if (o != null)
            {
                holder.imgView.setImageBitmap(o);
                //     holder.imgView.setAlpha(0.8f);
            }
            holder.v.setSelected(data.fSel[position]);
            holder.box.setChecked(data.fSel[position]);
            holder.box.setText(data.names[position]);
            holder.bcolor.SetColorUp(data.colors[position]);


        }

        //--------------------------------------------------------------------------------------
        //
        //--------------------------------------------------------------------------------------
        @Override
        public void onDetachedFromRecyclerView(RecyclerView recyclerView)
        {
            super.onDetachedFromRecyclerView(recyclerView);
            for (int i = 0; i < data.bmp.length; ++i)
                if (data.bmp[i] != null)
                {
                    data.bmp[i].recycle();
                    data.bmp[i] = null;
                }

        }

        //--------------------------------------------------------------------------------------
        //
        //--------------------------------------------------------------------------------------
        @Override
        public int getItemCount()
        {
            return Nstart > 0 ? Nstart : data.bmp.length;
        }


        //***********************************************************************
        class SelectedHolder extends RecyclerView.ViewHolder {

            View v;
            ImageView imgView;
            CheckBox box;
            CMenuButton bcolor;

            //--------------------------------------------------------------------------------------
            //
            //--------------------------------------------------------------------------------------
            SelectedHolder(View itemView)
            {
                super(itemView);
                v = itemView;
                imgView = itemView.findViewById(R.id.imageView);
                box = itemView.findViewById(R.id.textView);
                bcolor = itemView.findViewById(R.id.textColor);
                bcolor.SetCallback(this, "OnBtnColor");

            }

            public void OnBtnColor()
            {
                if (OnColor(getLayoutPosition()))
                    notifyDataSetChanged();
            }

            //--------------------------------------------------------------------------------------
            //
            //--------------------------------------------------------------------------------------
            void SetListner()
            {
                v.setOnClickListener(new COnClick());
            }

            //--------------------------------------------------------------------------------------
            //
            //--------------------------------------------------------------------------------------
            void ResetrListner()
            {
                v.setOnClickListener(null);
                v.setOnLongClickListener(null);

            }

            //      //******************************************************************
            public class COnClick implements View.OnClickListener {
                @Override
                public void onClick(View v)
                {
                    OnSelect(getLayoutPosition());
                    notifyDataSetChanged();
                }

            }

        }

    }
}


