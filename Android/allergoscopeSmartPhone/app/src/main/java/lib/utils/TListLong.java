package lib.utils;

public class TListLong {
    public class TListLongEntry {
        public void Init(TListLongEntry a)
        {
            next = prev = null;
            if (a != null)
            {
                prev = a;
                a.next = this;
            }
        }

        public void Reset()
        {
            if (prev != null)
                prev.next = next;
            if (next != null)
                next.prev = prev;
            next = prev = null;
        }

        public void Init(int _val, TListLongEntry _prev, TListLongEntry _next)
        {
            val = _val;
            next = _next;
            prev = _prev;

        }

        public void Init(int _val, TListLongEntry _prev)
        {
            val = _val;
            next = null;
            prev = _prev;

        }

        public void Init(int _val)
        {
            val = _val;
            prev = next = null;

        }

        public TListLongEntry prev = null;
        public TListLongEntry next = null;
        public int val;

    }


    public int Add(int a, TListLongEntry entry)
    {
        entry.Init(last);
        entry.val = a;
        if (first == null) first = entry;
        last = entry;
        ++num;
        return entry.val;
    }

    public int AddFirst(TListLongEntry entry)
    {
        entry.prev = null;
        entry.next = first;
        first = entry;
        if (entry.next != null)
            entry.next.prev = entry;
        else
            last = entry;
        ++num;
        return entry.val;
    }

    public int Add(TListLongEntry entry)
    {
        entry.Init(last);
        if (first == null) first = entry;
        last = entry;
        ++num;
        return entry.val;
    }

    public void Reset()
    {
        first = last = null;
        num = 0;
    }

    public void MoveTo(TListLongEntry entry, TListLong dst)
    {
        TListLongEntry tmp = entry;
        if (last == tmp)
            last = tmp.prev;

        if (first == tmp)
            first = tmp.next;
        tmp.Reset();
        --num;

        dst.Add(tmp);
    }

    public void MoveTo(TListLong dst)
    {
        TListLongEntry tmp, entry = first;
        while (entry != null)
        {
            tmp = entry.next;
            MoveTo(entry, dst);
            entry = tmp;
        }
    }

    public void Del(TListLongEntry entry)
    {
        TListLongEntry tmp = entry;
        if (last == tmp) last = tmp.prev;
        if (first == tmp) first = tmp.next;
        tmp.Reset();
        --num;
    }

    public TListLongEntry first = null;
    public TListLongEntry last = null;
    public int num = 0;
}
