package com.dryeye.app.card;

import com.dryeye.app.App;
import com.dryeye.app.R;

import lib.camera.CameraCalculatePreview;
import lib.utils.TreeDir;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;

import java.util.StringTokenizer;

import lib.common.CDialog;
import lib.utils.AGlobals;
import lib.utils.FileUtils;

import static lib.common.CDialog.IDCANCEL;

public class CurrentPath {
    TreeDir.DirNode root;
    TreeDir.FileNode current;
    public static int LEVEL_FIO = 0;
    public static int LEVEL_DATE = 1;
    public static int LEVEL_TEST = 2;

    //--------------------------------------
    //
    //--------------------------------------
    public CurrentPath()
    {
        root = null;
        current = null;

    }

    public void Init(String path) throws CException
    {
        StringTokenizer st = new StringTokenizer(path, "/");
        String s = st.nextToken();
        root = new TreeDir.DirNode(null, s, new File(App.defaultPath + "/" + s), null, false);

        SelectRel(path);
    }

    public void Reset()
    {
        root = null;
        current = null;

    }

    //--------------------------------------
    //
    //--------------------------------------
    public void Select(String path) throws CException
    {
        SelectRel(path.substring(App.defaultPath.length(), path.length()));
    }

    //--------------------------------------
    //
    //--------------------------------------
    public void SelectRel(String path) throws CException
    {
        current = root.GetFromPath(path);
    }

    public TreeDir.DirNode Root() throws CException
    {
        return root;
    }

    public TreeDir.DirNode Current() throws CException
    {
        return (TreeDir.DirNode) current;
    }

    public String RootDir() throws CException
    {
        return root.GetPath();
    }


    //--------------------------------------
    //
    //--------------------------------------
    public boolean CreateNew(String[] result_info) throws CException
    {
        root = null;
        current = null;
        String path = App.defaultPath;
        File dir = new File(path);
        if (!dir.exists())
            dir.mkdirs();

        String pathinfo, name = (result_info[1] + "_" + result_info[0]).trim() + "_" + result_info[3];
        path += "/" + name;
        dir = new File(path);
        if (!dir.exists())
            dir.mkdirs();
        pathinfo = path + "/info.ini";
        AGlobals.SaveIniW("CARD", "NAME", result_info[0], pathinfo);
        AGlobals.SaveIniW("CARD", "FAMILY", result_info[1], pathinfo);
        AGlobals.SaveIniW("CARD", "DATE", result_info[2], pathinfo);
        AGlobals.SaveIniW("CARD", "ID", result_info[3], pathinfo);

        root = new TreeDir.DirNode(null, name, new File(App.defaultPath + path), null, false);
        CreateNewDate();

        return true;

    }

    public boolean CreateNewDate() throws CException
    {
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
        Calendar c = Calendar.getInstance();
        String sname = sdf.format(c.getTime());
        String path = App.defaultPath + root.GetPath();
        String cur = path + "/" + sname;
        File dir = new File(cur);
        if (!dir.exists())
            dir.mkdirs();
        root.ReloadDir(path);
        Select(cur);
        return true;

    }

    //--------------------------------------
    //
    //--------------------------------------
    String Path() throws CException
    {
        return current != null ? App.defaultPath + current.GetPath() : null;
    }

    String RelPath() throws CException
    {
        return current != null ? current.GetPath() : null;
    }

    //--------------------------------------
    //
    //--------------------------------------
    public String Dir() throws CException
    {
        TreeDir.FileNode cur = current == null || current.IsDir() ? current : current.parent;
        return cur == null ? null : App.defaultPath + cur.GetPath();


    }

    public String RelDir() throws CException
    {
        TreeDir.FileNode cur = current == null || current.IsDir() ? current : current.parent;
        return cur == null ? null : cur.GetPath();


    }

    //--------------------------------------
    //
    //--------------------------------------
    public int Level() throws CException
    {
        return current.Level();
    }

    //--------------------------------------
    //
    //--------------------------------------
    public void CurrentDirList(ArrayList<TreeDir.DirNode> list)
    {
        ((TreeDir.DirNode) current).GetDirList(list);
    }

    //--------------------------------------
    //
    //--------------------------------------
    public boolean CreateNewTest() throws CException
    {

    boolean fRet = false;
        if (current.Level() == LEVEL_FIO)
            CreateNewDate();
        if (!CameraCalculatePreview.fPirke)
        {
            DlgNewTest dlgNewTest = new DlgNewTest(AGlobals.currentActivity);
            if (dlgNewTest.DoModal() == CDialog.IDOK)
            {
                try
                {
                    //SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH-mm-ss");
                    //Calendar c = Calendar.getInstance();
                    String sname = dlgNewTest.result; // + sdf.format(c.getTime());

                    String currentPath = App.defaultPath + current.GetPath() + "/" + sname;
                    File dir = new File(currentPath);
                    if (!dir.exists())
                        dir.mkdirs();
                    ((TreeDir.DirNode) current).ReloadDir(App.defaultPath + current.GetPath());
                    SelectRel(current.GetPath() + "/" + sname);
                    fRet = true;
                } catch (Exception e)
                {
                    return false;
                }

            }
        }
        else
        {
            String sname = AGlobals.currentActivity.getString(R.string.prick);; // + sdf.format(c.getTime());

            String currentPath = App.defaultPath + current.GetPath() + "/" + sname;
            File dir = new File(currentPath);
            if (!dir.exists())
                dir.mkdirs();
            ((TreeDir.DirNode) current).ReloadDir(App.defaultPath + current.GetPath());
            SelectRel(current.GetPath() + "/" + sname);
            fRet = true;


        }
        return fRet;
    }

    public String RenameTest(TreeDir.DirNodeVisible cur) throws CException
    {
        String ph = cur.dir.GetPath();
        if (root == null)
            Init(ph);
        current = root.GetFromPath(ph);

        if (current == null) return null;
        File oldPath = new File(App.defaultPath + (cur == null ? current.GetPath() : cur.dir.GetPath()));

        DlgNewTest dlgNewTest = new DlgNewTest(AGlobals.currentActivity);
        dlgNewTest.SetText(AGlobals.currentActivity.getString(R.string.rename_test));
        if (dlgNewTest.DoModal() == CDialog.IDOK)
        {
            try
            {
                String sname = dlgNewTest.result;
                if (current.name.equals(sname)) return null;
                String oldname = current.name;
                current.name = sname;
                String currentPath = App.defaultPath + current.parent.GetPath() + "/" + sname;
                File dir = new File(currentPath);
                if (!dir.exists())
                    oldPath.renameTo(dir);
                else
                {
                    DlgDeleteMerge merg = new DlgDeleteMerge();
                    if (merg.DoModal() == IDCANCEL)
                    {
                        current.name = oldname;
                        return null;
                    }
                    if (merg.pos == 0)
                    {
                          if (FileUtils.Copy(oldPath, dir))
                            FileUtils.DeleteDir(oldPath);
                    }
                    else
                    {
                        FileUtils.DeleteDir(dir);
                        oldPath.renameTo(dir);
                    }
                }

                ph = current.GetPath();
                TreeDir.DirNode parent = (TreeDir.DirNode) current.parent;
                parent.childs = null;
                String p1 = current.parent.GetPath();
                parent.ReloadDir(App.defaultPath + p1);
                parent = (TreeDir.DirNode) cur.dir.parent;
                parent.childs = null;
                parent.ReloadDir(App.defaultPath + p1);
                SelectRel(ph);
                return ph;
            } catch (Exception e)
            {
                FileUtils.AddToLog(e);
            }

        }
        return null;
    }

    //--------------------------------------
    //
    //--------------------------------------
    public String[] ReadCardInfo() throws CException
    {
        String[] info = null;
        if (root != null)
        {
            info = new String[4];
            String path = App.defaultPath + root.GetPath();
            String s = path + "/info.ini";
            info[0] = AGlobals.ReadIniW("CARD", "NAME", s);
            info[1] = AGlobals.ReadIniW("CARD", "FAMILY", s);
            info[2] = AGlobals.ReadIniW("CARD", "DATE", s);
            info[3] = AGlobals.ReadIniW("CARD", "ID", s);

            int isNull = 0;

            for (int i = 0; i < 3; ++i)
            {
                if (info[i] == null)
                {
                    ++isNull;
                }
            }
            if (isNull != 3)
            {
                for (int i = 0; i < 3; ++i)
                {
                    if (info[i] == null)
                    {
                        info[i] = "";
                    }
                }
            }
        }
        return info;
    }

}
