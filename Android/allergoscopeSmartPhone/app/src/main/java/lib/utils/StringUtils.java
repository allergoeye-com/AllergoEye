package lib.utils;

import java.util.StringTokenizer;

public class StringUtils {
    public static String ReplaceAll(String s, String p, String d)
    {
        StringTokenizer st = new StringTokenizer(s, p);
        String nstr = s;

        if (st.hasMoreTokens())
        {
            nstr = st.nextToken();
            while (st.hasMoreTokens())
                nstr += d + st.nextToken();
        }
        return nstr;

    }
}
