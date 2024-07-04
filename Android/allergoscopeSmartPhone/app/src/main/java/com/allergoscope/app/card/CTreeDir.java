package com.allergoscope.app.card;

import static com.allergoscope.app.MainActivity.mainActivity;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.core.content.ContextCompat;

import com.allergoscope.app.MainActivity;
import com.allergoscope.app.R;

import lib.camera.CameraCalculatePreview;
import lib.utils.AGlobals;
import lib.utils.StringUtils;
import lib.utils.TreeDir;

import java.util.ArrayList;
import java.util.StringTokenizer;

import lib.common.ActionCallback;
import lib.utils.CDateTime;
import lib.utils.FileUtils;

public class CTreeDir extends ListView {
    ListBoxAdapter adapter;
    ActionCallback action;
    int iSelected;
    TreeDir root = null;
    ArrayList<ArrayList<String> > cur = MainActivity.CurTest();
    ArrayList<ArrayList<String> > def = MainActivity.DefTest();

    public TreeDir.DirNodeVisible[] data = null;

    //---------------------------------------------------------
    //
    //---------------------------------------------------------
    public CTreeDir(Context context)
    {
        super(context);
        Init(context);
    }

    //---------------------------------------------------------
    //
    //---------------------------------------------------------
    public CTreeDir(Context context, AttributeSet attrs)
    {
        super(context, attrs);
        Init(context);
    }


    //---------------------------------------------------------
    //
    //---------------------------------------------------------
    public CTreeDir(Context context, AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
        Init(context);
    }

    public TreeDir.DirNodeVisible GetSelect()
    {
        return iSelected > -1 ? data[iSelected] : null;
    }
    //------------------------------------------------------
    //
    //------------------------------------------------------
    private void Init(Context context)
    {
        action = null;
        adapter = new ListBoxAdapter(context);
        iSelected = -1;
        setAdapter(adapter);
        this.setOnItemClickListener(adapter);
        setDividerHeight(0);
        setClickable(true);
        setChoiceMode(ListView.CHOICE_MODE_SINGLE);
        setScrollContainer(true);
        setVerticalScrollBarEnabled(true);

        setOnTouchListener(new OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event)
            {
                v.getParent().requestDisallowInterceptTouchEvent(true);
                return false;
            }
        });
    }

    public void SetSelect(String LastPath)
    {
        adapter.SelectDir(LastPath);
    }
    //------------------------------------------------------
    //
    //------------------------------------------------------
    public void Init(TreeDir dir, ActionCallback _action)
    {
        iSelected = -1;
        root = dir;
        adapter.Init(dir);
        action = _action;
    }


    //---------------------------------------------------------
    //
    //---------------------------------------------------------
    class ListBoxAdapter extends ArrayAdapter<TreeDir.DirNodeVisible> implements OnItemClickListener {
        Context context;

        public ListBoxAdapter(@NonNull Context _context)
        {
            super(_context, R.layout.list_box);
            context = _context;
            data = null;
        }

        public void Init(TreeDir _data)
        {
            cur = MainActivity.CurTest();
            def = MainActivity.DefTest();

            int I = iSelected;
            TreeDir.DirNode root = _data.Root();

            if (root != null)
            {
                ArrayList<TreeDir.DirNode> list = new ArrayList<TreeDir.DirNode>();
                root.GetDirList(list);
                ArrayList<TreeDir.FileNode> info = new ArrayList<TreeDir.FileNode>();
                String[] mask_info = new String[]{".ini", ".jpg"};
                int N = 0;
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
                    {
                        N += dir.GetNumDirNodes(true);

                    }
                }
                N += list.size();
                data = new TreeDir.DirNodeVisible[N];
                for (int i = 0; i < N; ++i)
                {
                    data[i] = new TreeDir.DirNodeVisible();
                }
                int[] index = new int[1];
                index[0] = 0;
                for (TreeDir.DirNode dr : list)
                    dr.GetDirList(index, data, true);
                if (index[0] != N)
                    ++N;
            }

            this.clear();
            this.addAll(data);
            if (I >= data.length)
                --I;
            iSelected = I;

            notifyDataSetChanged();

        }

        public void SelectDir(String path)
        {
            try
            {

                TreeDir.FileNode d = root.GetFromPath(path);
                if (d != null)
                {
                    if (d.IsDir())
                    {
                        TreeDir.DirNode dirNode = (TreeDir.DirNode) d;
                        if (dirNode.ref != null)
                        {
                            do
                            {
                                if (dirNode.ref != null)
                                {
                                    if (dirNode.ref.fBox)
                                        dirNode.ref.fExpanded = true;
                                    dirNode.ref.fVisible = true;
                                    dirNode.SetChildsVisible(true);
                                }

                                dirNode = (TreeDir.DirNode) dirNode.parent;

                            }
                            while (dirNode != null);

                            dirNode = (TreeDir.DirNode) d;
                            for (int i = 0; i < data.length; ++i)
                                if (data[i].equals(dirNode.ref))
                                {
                                    iSelected = i;
                                    break;
                                }
                            notifyDataSetChanged();
                            action.run(dirNode.ref);

                            smoothScrollToPosition(iSelected);
                        }

                    }

                }
            } catch (Exception e)
            {
                FileUtils.AddToLog(e);
            }

        }

        //---------------------------------------------------------
        //
        //---------------------------------------------------------
        @Override
        public View getView(int position, View convertView, ViewGroup parent)
        {
            if (convertView == null)
            {
                LayoutInflater inflater = LayoutInflater.from(context);
                convertView = inflater.inflate(R.layout.tree_box, null);
            }
            TextView text = (TextView) convertView.findViewById(R.id.textView);
            ImageView image = (ImageView) convertView.findViewById(R.id.imageView);

            Bitmap b = null;
            String uri;
            if (data[position].fBox)
            {

                uri = !data[position].fExpanded ? "drawable/collapsed" : "drawable/expanded";
                text.setTextAppearance(context, android.R.style.TextAppearance_Medium);
            } else
            {
                uri = "android:drawable/radiobutton_off_background";
                text.setTextAppearance(context, android.R.style.TextAppearance_Small);

            }
            try
            {
                int imageResource = context.getResources().getIdentifier(uri, null, context.getPackageName());
                b = ((BitmapDrawable)ContextCompat.getDrawable(context, imageResource)).getBitmap();
                text.setTextColor(ContextCompat.getColor(context, R.color.theme_dlg_text_color));
                image.setImageBitmap(b);
                int padding = b.getWidth();
                //  int padding = getResources().getDimensionPixelOffset(R.dimen.generic_cell_padding);
                convertView.setPadding(data[position].level * padding, 0, 0, 0);
                if (data[position].level == 0)
                {
                    String s = data[position].dir.name;
                    StringTokenizer toc = new StringTokenizer(s, "_");
                    if (toc.hasMoreTokens())
                    {
                        ArrayList<String> st = new ArrayList<String>();
                        s = toc.nextToken();
                        if (toc.hasMoreTokens())
                            s = s + " " + toc.nextToken();

                    }
                    text.setText(s);
                }
                else
                    if (data[position].level == 1)
                    text.setText(CDateTime.ConvertDate(data[position].dir.name));
                else

                if (data[position].level == 2)
                {

                    String nstr = StringUtils.ReplaceAll(data[position].dir.name, "-", ":");
                    String sname = AGlobals.currentActivity.getString(R.string.prick);; // + sdf.format(c.getTime());

                    if (!nstr.equals(sname))
                    {
                        nstr = StringUtils.ReplaceAll(nstr, "_", "/");
                        ArrayList<String> st1 = new ArrayList<String> ();
                        StringTokenizer st = new StringTokenizer(nstr, "/");
                        while (st.hasMoreTokens())
                        {
                            st1.add(st.nextToken());
                        }
                        if (st1.size() > 0 && st1.size() <= 4)
                        {
                            String res = "";
                            int i = 0;
                            for (i = 0; i < st1.size(); ++i)
                            {
                                int k = def.get(i).indexOf(st1.get(i));
                                if (k == -1)
                                    break;
                                else
                                {
                                    res += cur.get(i).get(k);

                                }
                                if (i != st1.size() - 1)
                                    res += "/";
                            }
                            if (i == st1.size())
                                nstr = res;
                        }
                    }
                    text.setText(nstr);


                }
                else
                    text.setText(data[position].dir.name);
                text.setVisibility(data[position].fVisible ? VISIBLE : GONE);
                image.setVisibility(data[position].fVisible ? VISIBLE : GONE);
                if (iSelected == position)
                    convertView.setBackgroundColor(ContextCompat.getColor(getContext(), R.color.middle_grey));
                else
                    convertView.setBackgroundColor(0x00000000);


                convertView.setVisibility(data[position].fVisible ? VISIBLE : INVISIBLE);
            } catch (Exception e)
            {
            }
            return convertView;
        }

        //---------------------------------------------------------
        //
        //---------------------------------------------------------
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id)
        {
            iSelected = position;
            if (data[position].fBox)
            {
                data[position].fExpanded = !data[position].fExpanded;
                boolean b = data[position].fExpanded;
                TreeDir.DirNode self = data[position].dir;
                self.SetChildsVisible(b);
            }
            if (action != null)
                action.run(data[position]);
            notifyDataSetChanged();
            if (data[position].fExpanded)
            {
                int pos = data[position].dir.childs.size() + position;
                if (pos != position)
                    smoothScrollToPosition(pos);
            }
        }
    }
}