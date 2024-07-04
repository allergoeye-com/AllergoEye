package lib.explorer.common;

import java.util.Comparator;

import lib.utils.FileUtils;

public class DirEntry {
    public class Entry
    {
        public String url = null;
        public int width = 0;
        public int height = 0;
    }

    public boolean fSelect = false;
    public boolean fIsDir = false;
    public Object any = null;
    public String thumb_img = null;
    public String title = new String();
    public String info = new String();
    public Entry thumb  = new Entry();
    public Entry img = new Entry();
    public Entry GetTrue() { return fSelect ? img : thumb; }
    public Entry GetFalse() { return !fSelect ? img : thumb; }
    public static Comparator<DirEntry> cmp = new Comparator<DirEntry>() {
        public int compare(DirEntry s1, DirEntry s2) {
            String t1 = s1.title.toUpperCase();
            String t2 = s2.title.toUpperCase();
            Object b ;
            return t1.compareTo(t2);
        }};

    public String MakeTrumbName()
    {
        String thrumb = title;
        if (img.url != null)
            thrumb += FileUtils.StringLastModify(img.url);
        return thrumb;
    }

    public Integer self = -1;
}
