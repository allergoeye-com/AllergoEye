package lib.common.grid;

import android.graphics.Bitmap;
import android.view.Gravity;
import android.view.ViewGroup;
import android.widget.LinearLayout;

import java.util.ArrayList;

import lib.common.ActionCallback;
import lib.explorer.common.DirEntry;
import lib.utils.FileUtils;

public class CCell extends Cell {

    public CCell(int _type, Cell [][] _param_grid)
    {
        super(_type, _param_grid);
    }

    public CCell(ArrayList<DirEntry> dir, int index, ActionCallback c)
    {
        super(dir, index, c);
    }

    public CCell(int _type, int _prop, String[] str, Bitmap[] bmp, String[] _bitmaps_path, int index, long _c_object, int _id)
    {
        super(_type, _prop, str, bmp, _bitmaps_path, index, _c_object, _id);
    }

    public CCell(int _type, int _prop, String[] str, int index)
    {
        super(_type, _prop, str, index);
    }

    public CCell(int _type, int _prop, String[] str, Bitmap[] bmp, int index)
    {
        super(_type, _prop, str, bmp, index);
    }

    public CCell(int _type, int _prop, String str, int index)
    {
        super(_type, _prop, str, index);
    }

    public CCell(int _type, Bitmap bmp)
    {
        super(_type, bmp);
    }

    public CCell(int _type, int _prop, String str)
    {
        super(_type, _prop, str);
    }

    public CCell(int cellGrid, int vertical, Cell[][] data)
    {
        super(cellGrid,vertical, data);
    }

    public void SetLayout(boolean b)
    {
        if (ctrl == null || type == CELL_SPACE) return;
        try {
            LinearLayout.LayoutParams params = null;
            ViewGroup.LayoutParams c =  ctrl.getLayoutParams();
            if (c != null)
                params = (LinearLayout.LayoutParams)c;
            else
                params = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);

            params.gravity = Gravity.CENTER_VERTICAL;
            if (!b)
                params.width = LinearLayout.LayoutParams.MATCH_PARENT;


            ctrl.setLayoutParams(params);


        }
        catch (Exception e)
        {
            FileUtils.AddToLog(e);

        }
    }
}
