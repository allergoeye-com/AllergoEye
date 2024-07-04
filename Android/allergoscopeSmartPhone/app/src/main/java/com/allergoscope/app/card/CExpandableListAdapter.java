package com.allergoscope.app.card;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseExpandableListAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.core.content.ContextCompat;

import com.allergoscope.app.R;
import lib.utils.TreeDir;

import java.util.ArrayList;
import java.util.StringTokenizer;

public class CExpandableListAdapter extends BaseExpandableListAdapter {
    public ArrayList<TreeDir.DirNode> list;
    public ArrayList<Boolean> isDir;

    public class Sel {
        int group;
        int sub;
    }

    Context context;
    public int iSelected = -1;
    public int DefaultTextColor;

    public CExpandableListAdapter(Context cnx, TreeDir _data)
    {

        context = cnx;
        DefaultTextColor = ContextCompat.getColor(context, R.color.theme_dlg_text_color);
        Update(_data);
    }

    public void Update(TreeDir _data)
    {
        list = new ArrayList<TreeDir.DirNode>();
        isDir = new ArrayList<Boolean>();
        TreeDir.DirNode root = _data.Root();
        if (root != null)
        {
            root.GetDirList(list);
            ArrayList<TreeDir.FileNode> info = new ArrayList<TreeDir.FileNode>();
            String[] mask_info = new String[]{".ini", ".jpg"};
            for (int i = 0, l = list.size(); i < l; ++i)
            {
                TreeDir.DirNode dir = list.get(i);
                info.clear();
                dir.GetFileList(info, mask_info);
                boolean err = true;
                boolean cards = false;
                for (TreeDir.FileNode file : info)
                {
                    if (file.name != null)
                    {
                        if (err)
                        {
                            if (file.name.equals("info.ini"))
                            {
                                err = false;
                            }
                        }

                        if (!cards)
                        {
                            int ll = file.name.length();
                            String ext = file.name.substring(ll - 4, ll);
                            if (ext.equalsIgnoreCase(".jpg"))
                                cards = true;

                        }
                    }
                    if (cards && !err)
                        break;
                }
                if (err)
                {
                    list.remove(i);
                    root.childs.remove(i);
                    --i;
                    --l;
                } else
                    isDir.add(cards);
            }
        }

    }

    @Override
    public int getGroupCount()
    {
        return list.size();
    }

    @Override
    public int getChildrenCount(int groupPosition)
    {
        try
        {
            return list.get(groupPosition).NumDir(false);
        } catch (Exception e)
        {
            return 0;
        }
    }

    public Sel GetSel()
    {
        if (iSelected == -1) return null;
        Sel sel = new Sel();

        int sub;

        sel.sub = (iSelected & 0x0000FFFF);
        sel.group = ((iSelected >> 16) & 0x0000FFFF);
        return sel;

    }

    @Override
    public Object getGroup(int groupPosition)
    {
        try
        {
            return list.get(groupPosition);
        } catch (Exception e)
        {
            return 0;
        }

    }

    @Override
    public Object getChild(int groupPosition, int childPosition)
    {
        try
        {
            TreeDir.DirNode f = list.get(groupPosition);
            return f == null ? null : f.get(childPosition);
        } catch (Exception e)
        {
            return 0;
        }

    }

    @Override
    public long getGroupId(int groupPosition)
    {
        return 0xFFFF0000 & ((groupPosition + 1) << 16);
    }

    @Override
    public long getChildId(int groupPosition, int childPosition)
    {
        return (0xFFFF0000 & ((groupPosition + 1) << 16)) | (childPosition + 1);
    }

    @Override
    public boolean hasStableIds()
    {
        return true;
    }

    @Override
    public View getGroupView(int groupPosition, boolean isExpanded, View convertView, ViewGroup parent)
    {
        try
        {

            if (convertView == null)
            {
                LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
                convertView = inflater.inflate(R.layout.group_view, null);
            }
            TextView textGroup = (TextView) convertView.findViewById(R.id.groupName);
            String s = (list.get(groupPosition)).name;

            if ((list.get(groupPosition)).Level() == 0)
            {
                StringTokenizer toc = new StringTokenizer(s, "_");
                if (toc.hasMoreTokens())
                {
                    ArrayList<String> st = new ArrayList<String>();
                    s = toc.nextToken();
                    if (toc.hasMoreTokens())
                        s = s + toc.nextToken();

                }
            }

            textGroup.setText(s);

            boolean fselCards = (iSelected & 0x0000FFFF) > 0;
            boolean fSelGroup = ((iSelected >> 16) & 0x0000FFFF) == groupPosition + 1;
            if (fSelGroup && fselCards)
            {
                convertView.setBackgroundColor(0x00000000);
                if (((iSelected >> 16) & 0x0000FFFF) == groupPosition + 1)
                    textGroup.setTextColor(0xFF10A0FF);
                else
                    textGroup.setTextColor(DefaultTextColor);
            } else
            {
                textGroup.setTextColor(DefaultTextColor);

                if (fSelGroup)
                    convertView.setBackgroundColor(0xFF10A0FF);
                else
                    convertView.setBackgroundColor(0x00000000);

            }

        } catch (Exception e)
        {
            ;
        }
        return convertView;
    }

    @Override
    public View getChildView(int groupPosition, int childPosition, boolean isLastChild, View convertView, ViewGroup parent)
    {
        try
        {
            if (convertView == null)
            {
                LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
                convertView = inflater.inflate(R.layout.child_item_view, null);
            }
            TextView textChild = (TextView) convertView.findViewById(R.id.textView);
            ImageView imageView = (ImageView) convertView.findViewById(R.id.imageView);
            Bitmap b = null;
            String uri = "android:drawable/radiobutton_off_background";
            int imageResource = context.getResources().getIdentifier(uri, null, context.getPackageName());
            b = ((BitmapDrawable) ContextCompat.getDrawable(context, imageResource)).getBitmap();

            if (iSelected == ((0xFFFF0000 & ((groupPosition + 1) << 16)) | (childPosition + 1)))
                convertView.setBackgroundColor(0xFF10A0FF);
            else
                convertView.setBackgroundColor(0x00000000);
            textChild.setTextColor(ContextCompat.getColor(context, R.color.theme_dlg_text_color));
            imageView.setImageBitmap(b);
            String s = (list.get(groupPosition)).get(childPosition).name;
            textChild.setText(s);

        } catch (Exception e)
        {
            ;
        }
        return convertView;
    }

    @Override
    public boolean isChildSelectable(int groupPosition, int childPosition)
    {
        return true;
    }
}
