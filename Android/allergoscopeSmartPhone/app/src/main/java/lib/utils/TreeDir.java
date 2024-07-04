package lib.utils;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParserFactory;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.StringTokenizer;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

import lib.utils.CDateTime;
import lib.utils.FileUtils;

import static lib.utils.FileUtils.AddToLog;

public class TreeDir {

    private DirNode root;
    public static Comparator<FileNode> cmp = new Comparator<FileNode>() {
        public int compare(FileNode s1, FileNode s2)
        {
            String t1 = s1.name; //.toUpperCase();
            String t2 = s2.name; //.toUpperCase();
            if (s1.IsDir() == s2.IsDir())
            {
                return t1.compareTo(t2);
            }
            if (s1.IsDir() && !s2.IsDir())
                return -1;
            if (!s1.IsDir() && s2.IsDir())
                return 1;
            return t1.compareTo(t2);

        }
    };
    public static Comparator<FileNode> cmp2 = new Comparator<FileNode>() {
        public int compare(FileNode s1, FileNode s2)
        {

            String t1 = s1.name; //.toUpperCase();
            String t2 = s2.name; //.toUpperCase();
            boolean b1 = s1.IsDir();
            boolean b2 = s2.IsDir();
            if (b1 == b2)
            {
                if (b1 && b2 && s1.Level() == 2)
                {
                    t1 = ((DirNode)s1).GetFirstFileFromTestDir();
                    if (t1 == null)
                        t1 = s1.date;
                    t2 = ((DirNode)s2).GetFirstFileFromTestDir();
                    if (t2 == null)
                        t2 = s2.date;
                    return t2.compareTo(t1);
                }
                return t1.compareTo(t2);


            }
            if (b1 && !b2)
            {
                return -1;
            }
            if (!b1 && b2)
            {
                return 1;
            }
            return t1.compareTo(t2);

        }
    };

    //--------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------
    public TreeDir(String path, boolean fDir)
    {
        root = new DirNode(null, null, new File(path), null, fDir);
    }

    //--------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------
    public TreeDir(String path)
    {
        root = new DirNode(null, null, new File(path), null, false);
    }

    //--------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------
    public TreeDir(String path, String[] mask)
    {
        root = new DirNode(null,  new File(path), mask, false);
    }

    //--------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------
    public TreeDir(Tree data)
    {
        root = null;
        Tree.Node node = data.root.FindFolder("file-area");
        if (node != null)
            root = new DirNode(null, node);
    }

    //--------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------
    public TreeDir()
    {
        root = null;
    }
    void AddDir(String s)
    {
        if (root == null)
            root = new DirNode();
        root.AddDir(s);
    }
    public void AddFile(String s) {
        if (root == null)
            root = new DirNode();
        root.AddFile(s);
    }


    //--------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------
    public FileNode get(int i)
    {
        return root == null ? null : root.get(i);
    }

    //--------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------
    public int size()
    {
        return root == null ? 0 : root.childs.size();
    }

    //--------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------
    public int NumDir(boolean fCheck)
    {
        return root == null ? 0 : root.NumDir(fCheck);
    }

    public FileNode GetFromPath(String path)
    {
        return root != null ? root.GetFromPath(path) : null;

    }

    //--------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------
    public DirNode Root()
    {
        return (DirNode) root;
    }
    public void CreateDirFromDisk(String ph) throws IOException
    {
        if (root != null)
        {
            root.CreateDirFromDisk(ph);
        }
    }
    //--------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------
    public void DeleteNode(FileNode node)
    {

        DirNode dir = (DirNode) node.parent;
        if (dir != null)
        {
            dir.DeleteNode(node);
            if (dir.childs == null || dir.childs.size() == 0)
                DeleteNode(dir);
        }
    }

    public ArrayList<String> GetFileList(String[] mask)
    {
        ArrayList<String> result = new ArrayList<String>();
        if (root != null)
        {
            ArrayList<FileNode> list = new ArrayList<FileNode>();
            root.GetFileListAll(list, mask);
            for (FileNode g : list)
                result.add(g.GetPath());
        }
        return result;
    }

    //--------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------
    public ArrayList<FileNode> GetFileNodeList()
    {
        ArrayList<FileNode> res = new ArrayList<FileNode>();
        GetFileListAll(res);
        return res;

    }
    //--------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------
    public int GetFileList(String dir, String [] mask, ArrayList<FileNode> dirs, ArrayList<FileNode> files)
    {
        if (root != null)
        {
            StringTokenizer st = new StringTokenizer(dir, "/");
            if (st.hasMoreTokens())
            {
                FileNode node = root.GetNode(st);
                if (node != null)
                {
                    if (node instanceof DirNode)
                        ((DirNode)node).GetFileList(mask, dirs, files);
                }
            }
            else
                ((DirNode)root).GetFileList(mask, dirs, files);

        }
        return files.size() + dirs.size();

    }
    //--------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------
    public ArrayList<FileNode> GetFileNodeList(String[] mask)
    {
        ArrayList<FileNode> res = new ArrayList<FileNode>();
        if (root != null)
            root.GetFileList(res, mask);
        return res;
    }

    //--------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------
    public ArrayList<String> ReadXml(String xml)
    {
        ArrayList<String> result = new ArrayList<String>();
        try
        {
            Tree tree = null;
            tree = _ReadXML(xml);
            root = null;
            Tree.Node node = tree.root.FindFolder("file-area");
            if (node != null)
                root = new DirNode(null, node);

            ArrayList<FileNode> list = new ArrayList<FileNode>();
            GetFileListAll(list);
            for (FileNode g : list)
                result.add(g.GetPath());
        } catch (Exception e)
        {
            result = null;
        }
        return result;
    }


    //--------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------
    public String MakeXml()
    {
        String s = null;
        if (root != null)
        {
            s = "<file-area>";

            s += root.MakeXml(false);
            s += "</file-area>";
        }
        return s;
    }

    public String MakeDirXml()
    {
        String s = null;
        if (root != null)
        {
            s = "<file-area>";

            s += root.MakeXml(true);
            s += "</file-area>";
        }
        return s;
    }

    //-------------------------------------
    //
    //-------------------------------------
    Tree _ReadXML(String outp) throws Exception
    {
        Tree tree = new Tree();
        XmlPullParserFactory factory = null;
        try
        {
            factory = XmlPullParserFactory.newInstance();
            factory.setNamespaceAware(true);
            XmlPullParser xpp = factory.newPullParser();
            String nm;
            xpp.setInput(new StringReader(outp));
            int eventType = xpp.getEventType();
            while (eventType != XmlPullParser.END_DOCUMENT)
            {
                switch (eventType)
                {
                    case XmlPullParser.START_TAG:
                        nm = xpp.getName();
                        tree.CreateNode(nm);
                        if (xpp.getAttributeCount() > 0)
                            tree.AddData(xpp.getAttributeValue(0));

                        break;

                    case XmlPullParser.END_TAG:
                        nm = xpp.getName();
                        tree.CloseNode(nm);
                        break;
                    case XmlPullParser.TEXT:
                        nm = xpp.getText();
                        tree.AddData(nm);
                        break;
                }
                eventType = xpp.next();

            }
        } catch (XmlPullParserException e)
        {
            AddToLog(e);
        } catch (IOException e)
        {
            AddToLog(e);
        }
        return tree;
    }

    void GetFileListAll(ArrayList<FileNode> l)
    {
        root.GetFileListAll(l);
    }

    //***********************************************
    public static class FileNode {
        public String name = null;
        public String date = null;
        public FileNode parent = null;

        //-----------------------------------
        //
        //-----------------------------------
        public FileNode(FileNode _parent, Tree.Node file)
        {
            parent = _parent;
            Tree.Node n = file.Get("name");
            name = n == null ? null : n.data;
            n = file.Get("date");
            date = n == null ? null : n.data;

        }

        //-----------------------------------
        //
        //-----------------------------------
        public FileNode(FileNode _parent, String _name)
        {
            parent = _parent;
            name = _name;
        }
        //-----------------------------------
        //
        //-----------------------------------
        public FileNode(FileNode _parent, File f)
        {
            parent = _parent;
            if (f != null && parent != null)
            {
                name = f.getName();
                Long last_modi = f.lastModified();
                date = CDateTime.formatDate.format(last_modi);

            }
        }
        public FileNode(FileNode _parent, String _name, File f)
        {
            parent = _parent;
            if (f != null && (_name != null || parent != null))
            {
                name = f.getName();
                Long last_modi = f.lastModified();
                date = CDateTime.formatDate.format(last_modi);

            }
        }

        //-----------------------------------
        //
        //-----------------------------------
        public FileNode()
        {
            name = null;

            parent = null;
        }

        //-----------------------------------
        //
        //-----------------------------------
        int Level(int l)
        {
            l += parent != null && parent.name != null ? 1 : 0;
            if (parent != null)
                return parent.Level(l);
            return l;
        }

        //-----------------------------------
        //
        //-----------------------------------
        public int Level()
        {
            int l = 0;
            if (parent != null)
                return Level(l);
            return l;
        }

        //-----------------------------------
        //
        //-----------------------------------
        FileNode GetRoot()
        {
            if (parent != null && parent.name != null)
                return parent.GetRoot();
            return this;
        }

        //-----------------------------------
        //
        //-----------------------------------
        public boolean IsDir()
        {
            return this instanceof DirNode;
        }

        //-----------------------------------
        //
        //-----------------------------------
        private String GetPath(String s)
        {
            if (parent != null)
                return parent.GetPath(name + "/" + s);

            if (name != null)
                return "/" + name + "/" + s;
            return "/" + s;
        }

        //-----------------------------------
        //
        //-----------------------------------
        public String GetPath()
        {
            if (parent != null && name != null)
                return parent.GetPath(name);
            return name == null ? "/" : "/" + name;

        }

        //-----------------------------------
        //
        //-----------------------------------
        boolean equals(FileNode n)
        {
            return name.equals(n.name);
        }
    }

    public static class DirNodeVisible {
        public boolean fExpanded = false;
        public boolean fVisible = false;
        public boolean fBox = false;
        public int level = 0;
        public DirNode dir = null;
    }

    //***********************************************
    public static class DirNode extends FileNode {
        public ArrayList<FileNode> childs = null;
        public DirNodeVisible ref = null;

        DirNode()
        {
            super();
            childs = null;

        }
        //-----------------------------------
        //
        //-----------------------------------
        DirNode(FileNode _parent, String _n)
        {
            super(_parent, _n);
        }

        //-----------------------------------
        //
        //-----------------------------------
        DirNode(FileNode _parent, Tree.Node folder)
        {
            super(_parent, folder);
            childs = null;
            Tree.Node dir = folder.Get("folders");
            if (dir != null && dir.child != null)
            {
                if (childs == null)
                    childs = new ArrayList<FileNode>();
                for (Tree.Node file : dir.child)
                {
                    if (file.name.equals("folder"))
                        childs.add(new DirNode(this, file));
                }
            }
            Tree.Node files = folder.Get("files");
            if (files != null && files.child != null)
            {
                if (childs == null)
                    childs = new ArrayList<FileNode>();
                for (Tree.Node file : files.child)
                {
                    if (file.name.equals("file"))
                        childs.add(new FileNode(this, file));
                }
            }

            if (childs != null)
                Collections.sort(childs, cmp);

        }

        public FileNode get(int i)
        {
            return childs.get(i);
        }

        //-----------------------------------
        //
        //-----------------------------------
        public DirNode(FileNode _parent, File path, String[] mask, boolean fDir)
        {
            super(_parent, path);
            ArrayList<File> fdir = new ArrayList<File>();
            ArrayList<File> ffile = new ArrayList<File>();
            FileUtils.GetFileList(path, mask, fdir, ffile);
            int i, l;
            for (i = 0, l = fdir.size(); i < l; ++i)
            {
                if (childs == null)
                    childs = new ArrayList<FileNode>();

                childs.add(new DirNode(this,  fdir.get(i), mask, fDir));
            }
            if (!fDir)
                for (i = 0, l = ffile.size(); i < l; ++i)
                {
                    if (childs == null)
                        childs = new ArrayList<FileNode>();

                    childs.add(new FileNode(this, ffile.get(i)));
                }
            if (childs != null)
                Collections.sort(childs, cmp);

        }
  //-----------------------------------
        //
        //-----------------------------------
        public DirNode(FileNode _parent, String _name, File path, String[] mask, boolean fDir)
        {
            super(_parent, _name, path);
		    ArrayList<File> fdir = new ArrayList<File>();
            ArrayList<File> ffile = new ArrayList<File>();
            FileUtils.GetFileList(path, mask, fdir, ffile);
            int i, l;
            for (i = 0, l = fdir.size(); i < l; ++i)
            {
                if (childs == null)
                    childs = new ArrayList<FileNode>();

                childs.add(new DirNode(this,  fdir.get(i), mask, fDir));
            }
            if (!fDir)
                for (i = 0, l = ffile.size(); i < l; ++i)
                {
                    if (childs == null)
                        childs = new ArrayList<FileNode>();

                    childs.add(new FileNode(this, ffile.get(i)));
                }
            if (childs != null)
                Collections.sort(childs, cmp);

        }
        //-----------------------------------
        //
        //-----------------------------------

        DirNode AddDir(ArrayList<String> st, int i)
        {

            String s = st.get(i);
            DirNode dir = null;
            FileNode node = GetChild(s);
            if (node == null || !node.IsDir())
            {
                dir = new DirNode(this, st.get(i));
                if (childs == null)
                    childs = new ArrayList<FileNode>();
                childs.add(dir);
            }
            else
                dir = (DirNode)node;
            if (st.size() > i + 1)
            {

                dir = dir.AddDir(st, ++i);
            }
            return dir;
        }
        FileNode AddFile(ArrayList<String> st)
        {
            String sfile = st.get(st.size() - 1);
            st.remove(st.size() - 1);
            DirNode dir = AddDir(st, 0);
            FileNode file = null;
            if (dir != null)
            {
                if ((file = dir.GetChild(sfile)) == null)
                {
                    file = new FileNode(dir, sfile);
                    if (dir.childs == null)
                        dir.childs = new ArrayList<FileNode>();
                    dir.childs.add(file);
                }
            }
            return file;

        }
        void AddFile(String s)
        {
            if (childs == null)
                childs = new ArrayList<FileNode>();
            ArrayList<String> st = new ArrayList<String>();
            StringTokenizer toc = new StringTokenizer(s, "/");
            while (toc.hasMoreTokens())
                st.add(toc.nextToken());
            AddFile(st);
        }
        void AddDir(String s)
        {
            if (childs == null)
                childs = new ArrayList<FileNode>();
            ArrayList<String> st = new ArrayList<String>();
            StringTokenizer toc = new StringTokenizer(s, "/");
            while (toc.hasMoreTokens())
                st.add(toc.nextToken());
            AddDir(st, 0);
        }
        //-----------------------------------
        //
        //-----------------------------------
        public FileNode GetNode(StringTokenizer st)
        {

            String s = st.nextToken();
            FileNode node = GetChild(s);
            if (st.hasMoreTokens())
                if (node != null)
                {
                    if (node instanceof DirNode)
                        node = ((DirNode)node).GetNode(st);
                    else
                        node = null;
                }
            return node;
        }
        //-----------------------------------
        //
        //-----------------------------------
        public int GetFileList(String [] mask, ArrayList<FileNode> dirs, ArrayList<FileNode> files)
        {
            if (childs == null) return 0;
            int i = 0;
            for (FileNode node:childs)
            {
                boolean fIsDir = node.IsDir();
                if (fIsDir)
                    dirs.add(node);
                else
                {
                    String n = node.name;
                    if (n.length() > 4)
                    {
                        String ext = n.substring(n.length() - 4, n.length());
                        if (mask == null)
                        {
                            files.add(node);
                        }
                        else
                            for (String g : mask)
                                if (ext.equalsIgnoreCase(g))
                                {
                                    files.add(node);
                                    break;
                                }
                    }
                }
            }
            return childs.size();
        }
        //-----------------------------------
        //
        //-----------------------------------
        public void DeleteNode(FileNode n)
        {
            for (int i = 0, l = childs.size(); i < l; ++i)
            {
                if (childs.get(i).equals(n))
                {
                    childs.remove(i);
                    break;
                }
            }
            if (childs.size() == 0)
                childs = null;

        }
        public String GetFirstFileFromTestDir()
        {
            if (childs != null)
            {
                for (int i = 0, l = childs.size(); i < l; ++i)
                {
                    return childs.get(i).date;
                }
            }
             return null;
        }
        //-----------------------------------
        //
        //-----------------------------------
        public FileNode Find(String str, boolean fDir)
        {
            FileNode tmp = fDir ? new DirNode(null, str) : new FileNode(null, str);

            int i = Collections.binarySearch(childs, tmp, cmp);
            return i > -1 ? childs.get(i) : null;
        }

        //-----------------------------------
        //
        //-----------------------------------
        public FileNode GetFromPath(String path)
        {
            DirNode node = this;
            StringTokenizer st = new StringTokenizer(path, "/");
            if (st.hasMoreTokens())
                if (name != null && !st.nextToken().equals(name))
                    return null;
            while (st.hasMoreTokens())
            {
                String s = st.nextToken();
                FileNode f = node.Find(s, st.hasMoreTokens());
                if (f == null && !st.hasMoreTokens())
                    f = node.Find(s, true);
                if (f != null)
                {
                    if (f.IsDir())
                        node = (DirNode)f;
                    else
                    {
                        if (st.hasMoreTokens())
                            return null;
                        else
                            return f;
                    }
                }
                else
                    return null;
            }
            return node;
        }

        //-----------------------------------
        //
        //-----------------------------------
        public void ReloadDir(String path)
        {

            ArrayList<FileNode> res = new ArrayList<FileNode>();
            ArrayList<File> fdir = new ArrayList<File>();
            ArrayList<File> ffile = new ArrayList<File>();
            FileUtils.GetFileList(new File(path), (String[]) null, fdir, ffile);
            int i, l;
            FileNode tmp = new FileNode();
            DirNode tmp_dir = new DirNode();

            for (i = 0, l = fdir.size(); i < l; ++i)
            {
                if (childs == null)
                    childs = new ArrayList<FileNode>();
                tmp_dir.name = fdir.get(i).getName();
                int pos = Collections.binarySearch(childs,  tmp_dir, cmp);
                if (pos < 0)
                    res.add(new DirNode(this, fdir.get(i), null, false));
            }
            for (i = 0, l = ffile.size(); i < l; ++i)
            {
                if (childs == null)
                    childs = new ArrayList<FileNode>();
                tmp.name = ffile.get(i).getName();
                int pos = Collections.binarySearch(childs, tmp, cmp);
                if (pos < 0)
                    res.add(new FileNode(this, ffile.get(i)));
            }
            for (i = 0, l = res.size(); i < l; ++i)
                childs.add(res.get(i));
            if (l > 0)
                Collections.sort(childs, cmp);
        }
        public void ReloadFiles(String path, String [] mask)
        {
            try {
            ArrayList<FileNode> res = new ArrayList<FileNode>();
            ArrayList<File> fdir = new ArrayList<File>();
            ArrayList<File> ffile = new ArrayList<File>();
            String curph =  GetPath();
            if (!curph.equals("/"))
                FileUtils.GetFileList(new File(path + curph), mask, fdir, ffile);
            if (childs != null)
            for (FileNode node:childs)
            {
                boolean fIsDir = node.IsDir();
                if (fIsDir)
                    ((DirNode)node).ReloadFiles(path, mask);
            }

            int i, l;
            FileNode tmp = new FileNode();
            for (i = 0, l = ffile.size(); i < l; ++i)
            {
                if (childs == null)
                    childs = new ArrayList<FileNode>();
                tmp.name = ffile.get(i).getName();
                int pos = Collections.binarySearch(childs, tmp, cmp);
                if (pos < 0)
                    res.add(new FileNode(this, ffile.get(i)));
            }
            for (i = 0, l = res.size(); i < l; ++i)
                childs.add(res.get(i));
            if (l > 0)
                Collections.sort(childs, cmp);
            }
            catch(Exception e)
            {
                AddToLog(e);
            }
        }
        public void CreateDirFromDisk(String path) throws IOException
        {
            int i = 0;
            if (name != null)
            {

                String dir = path + GetPath();
                FileUtils.MakeDir(dir);

            }
            if (childs == null) return;
            for (FileNode node : childs)
            {
                if (node.IsDir())
                    ((DirNode)node).CreateDirFromDisk(path);
            }
            return;

        }
        //-----------------------------------
        //
        //-----------------------------------
        public int NumDir(boolean fChecAll)
        {
            if (childs == null) return 0;
            int i = 0;
            for (FileNode node : childs)
            {
                if (node.IsDir())
                    ++i;
            }
            return i;
        }

        //-----------------------------------
        //
        //-----------------------------------
        public int GetFileList(ArrayList<FileNode> list, String[] mask)
        {
            if (childs == null) return 0;
            int i = 0;

            for (FileNode node : childs)
            {
                if (!node.IsDir())
                {
                    int l = node.name.length();
                    String ext = node.name.substring(l - 4, l);
                    if (mask != null)
                    {
                        for (String m : mask)
                            if (ext.equalsIgnoreCase(m))
                            {
                                list.add(node);
                                ++i;
                                break;
                            }
                    } else
                    {
                        list.add(node);
                        ++i;

                    }
                }
            }
            return i;
        }

        //-----------------------------------
        //
        //-----------------------------------
        public int GetDirList(int[] index, DirNodeVisible[] isDir, boolean fAddSubDir)
        {

            int i = 0;
            isDir[index[0]].level = 0;
            isDir[index[0]].fVisible = true;
            isDir[index[0]].dir = this;
            ref = isDir[index[0]];
            int cur_i = index[0];
            ++index[0];
            if (childs == null) return 0;
            for (FileNode node : childs)
            {
                if (node.IsDir())
                {
                    isDir[cur_i].fBox = true;
                    if (fAddSubDir)
                        i += ((DirNode) node).GetDirList(index, isDir, 1);
                    ++i;
                }


            }
            return i;
        }
        //-----------------------------------
        //
        //-----------------------------------
        public boolean MakeZip(File zipFile, String rootDir)
        {
            File dir = new File(rootDir);
            ZipOutputStream out = null;
            boolean res = false;
            try
            {
                out = new ZipOutputStream(new BufferedOutputStream(new FileOutputStream(zipFile)));
                res =  Zip(out, rootDir);
                out.close();
                return res;

            }
            catch (IOException e)
            {
                AddToLog(e);
            }
            return res;

        }
        private boolean Zip(ZipOutputStream out, String rootDir)
        {
            try {
            String ph = GetPath();
                ZipEntry entry = null;
            if (!ph.equals("/"))
            {
                File ff = new File(rootDir);
                String nm = ff.toURI().relativize(new File(rootDir + ph).toURI()).getPath();
                entry = new ZipEntry(nm);
                out.putNextEntry(entry);
                out.closeEntry();
            }
            for (FileNode node : childs)
            {
                if (node.IsDir())
                {
                    ((DirNode)node).Zip(out, rootDir);
                }
                else
                {
                    ph = node.GetPath();
                    File ff = new File(rootDir);
                    String nm = ff.toURI().relativize(new File(rootDir + ph).toURI()).getPath();
                    entry = new ZipEntry(nm);
                    out.putNextEntry(entry);
                    byte data[] = new byte[1024 * 64];
                    FileInputStream fi = new FileInputStream(new File(rootDir + ph));
                    BufferedInputStream origin = new BufferedInputStream(fi, data.length);
                    int count;
                    while ((count = origin.read(data, 0, data.length)) != -1)
                        out.write(data, 0, count);
                    origin.close();

                }

            }


            } catch(Exception e) {

                AddToLog(e);
                return false;
            }
            return true;

        }

        public int GetDirList(int[] index, DirNodeVisible[] isDir, int level)
        {

            int i = 0;
            isDir[index[0]].level = level;
            isDir[index[0]].dir = this;
            ref = isDir[index[0]];

            int cur_i = index[0];
            ++index[0];
            ++level;

            if (childs == null) return 0;
            boolean flg = level  == 2;
            if (flg)
                Collections.sort(childs, cmp2);

            for (FileNode node : childs)
            {
                if (node.IsDir())
                {

                    isDir[cur_i].fBox = true;
                    i += ((DirNode) node).GetDirList(index, isDir, level);
                    ++i;
                }


            }
            if (flg)
                Collections.sort(childs, cmp);
            return i;
        }

        public int SetChildsVisible(boolean f)
        {

            if (childs == null) return 0;
            int i = 0;

            for (FileNode node : childs)
            {
                if (node.IsDir())
                {
                    ((DirNode) node).ref.fVisible = f;
                    if (((DirNode) node).ref.fExpanded)
                        i += ((DirNode) node).SetChildsVisible(f);
                    ++i;
                }


            }
            return i;
        }

        //-----------------------------------
        //
        //-----------------------------------
        public int GetNumDirNodes(boolean fAddSubDir)
        {
            if (childs == null) return 0;
            int i = 0;
            for (FileNode node : childs)
            {
                if (node.IsDir())
                {
                    if (fAddSubDir)
                        i += ((DirNode) node).GetNumDirNodes(true);
                    ++i;
                }
            }
            return i;
        }

        //-----------------------------------
        //
        //-----------------------------------
        public int GetNumFile(String[] mask)
        {
            if (childs == null) return 0;
            int i = 0;
            for (FileNode node : childs)
            {
                if (!node.IsDir())
                {
                    int l = node.name.length();
                    String ext = node.name.substring(l - 4, l);
                    if (mask != null)
                    {
                        for (String m : mask)
                            if (ext.equalsIgnoreCase(m))
                            {
                                ++i;
                                break;
                            }
                    } else
                        ++i;
                }
            }
            return i;
        }

        //-----------------------------------
        //
        //-----------------------------------
        public int GetDirList(ArrayList<DirNode> list)
        {
            if (childs == null) return 0;
            int i = 0;
            for (FileNode node : childs)
            {
                if (node.IsDir())
                {
                    list.add((DirNode) node);
                    ++i;

                }
            }
            return i;
        }

        //-----------------------------------
        //
        //-----------------------------------
        public int GetFileListAll(ArrayList<FileNode> list, String[] mask)
        {
            if (childs == null) return 0;
            int i = 0;
            for (FileNode node : childs)
            {
                if (node.IsDir())
                    i += ((DirNode) node).GetFileListAll(list, mask);
                else
                {
                    int l = node.name.length();
                    String ext = node.name.substring(l - 4, l);
                    if (mask != null)
                    {
                        for (String m : mask)
                            if (ext.equalsIgnoreCase(m))
                            {
                                list.add(node);
                                ++i;
                                break;
                            }
                    } else
                    {
                        list.add(node);
                        ++i;

                    }
                }
            }
            return i;
        }

        //-----------------------------------
        //
        //-----------------------------------
        String MakeXml(boolean fDirs)
        {
            if (childs == null) return "";
            String str = new String();
            boolean fStart = true;
            for (FileNode node : childs)
            {
                boolean fIsDir = node.IsDir();
                if (fIsDir)
                {
                    if (fStart)
                    {
                        fStart = false;
                        str += "<folders>";
                    }
                    str += "<folder>";
                    str += "<name>" + node.name + "</name>";
                    if (node.date != null)
                    {
                        str += "<date>" + node.date + "</date>";
                    }
                    str += ((DirNode) node).MakeXml(fDirs);
                    str += "</folder>";
                }

            }
            if (!fStart)
                str += "</folders>";
            if (!fDirs)
            {
                fStart = true;
                for (FileNode node : childs)
                {
                    boolean fIsDir = node.IsDir();
                    if (!fIsDir)
                    {
                        if (fStart)
                        {
                            fStart = false;
                            str += "<files>";
                        }
                        str += "<file>";
                        str += "<name>" + node.name + "</name>";
                        if (node.date != null)
                            str += "<date>" + node.date + "</date>";
                        str += "</file>";
                    }

                }
                if (!fStart)
                    str += "</files>";
            }
            return str;

        }


        //-----------------------------------
        //
        //-----------------------------------
        int GetFileListAll(ArrayList<FileNode> list)
        {
            if (childs == null) return 0;
            int i = 0;
            for (FileNode node : childs)
            {
                if (node.IsDir())
                    i += ((DirNode) node).GetFileListAll(list);
                else
                {
                    list.add(node);
                    ++i;
                }
            }
            return i;
        }
        //-----------------------------------
        //
        //-----------------------------------
        int GetFileList(ArrayList<FileNode> list)
        {
            if (childs == null) return 0;
            int i = 0;
            for (FileNode node : childs)
            {
                if (node.IsDir())
                    i += ((DirNode) node).GetFileListAll(list);
                else
                {
                    list.add(node);
                    ++i;
                }
            }
            return i;
        }


        //-----------------------------------
        //
        //-----------------------------------
        private FileNode GetChild(String name)
        {
            if (childs == null) return null;
            for (FileNode node : childs)
            {
                if (node.name.equals(name))
                    return node;
            }
            return null;
        }

    }


    //************************************************************
    //
    //************************************************************
    class Tree {
        Node root = new Node(null, null);
        Node last = root;

        //------------------------------------------------------
        //
        //------------------------------------------------------
        void CreateNode(String name)
        {
            if (last.child == null)
                last.child = new ArrayList<Tree.Node>();
            Node node = new Node(last, name);
            last.child.add(node);
            last = node;
        }

        //------------------------------------------------------
        //
        //------------------------------------------------------
        void AddData(String data)
        {
            last.data = data;
        }

        //------------------------------------------------------
        //
        //------------------------------------------------------
        void CloseNode(String name) throws Exception
        {
            if (!name.equals(last.name))
                throw new Exception("Error");
            last = last.parent;
        }

        //****************************************************
        class Node {
            String name;
            String data = null;
            ArrayList<Node> child = null;
            boolean fClosed = false;
            Node parent;

            //------------------------------------------------------
            //
            //------------------------------------------------------
            Node(Node _parent, String n)
            {
                parent = _parent;
                name = n;
            }

            //------------------------------------------------------
            //
            //------------------------------------------------------
            Node GetChild(String nm)
            {
                if (child != null)
                {
                    for (Node c : child)
                    {
                        if (c.name.equals(nm))
                            return c;
                    }
                }
                return null;
            }

            //------------------------------------------------------
            //
            //------------------------------------------------------
            Node Get(String path)
            {
                Node node = this;
                StringTokenizer st = new StringTokenizer(path, "/");
                while (st.hasMoreTokens())
                {
                    String s = st.nextToken();
                    node = node.GetChild(s);
                    if (node == null)
                        return null;
                }
                return node;
            }

            //------------------------------------------------------
            //
            //------------------------------------------------------
            Node FindFolder(String nm)
            {

                if (child != null)
                {
                    for (Node c : child)
                    {
                        if (c.name.equals(nm))
                            return c;
                        else
                        {
                            Node d = c.FindFolder(nm);
                            if (d != null)
                                return d;
                        }
                    }
                }
                return null;
            }

        }
    }
}
