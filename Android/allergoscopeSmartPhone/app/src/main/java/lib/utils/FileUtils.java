package lib.utils;


import android.Manifest;
import android.annotation.SuppressLint;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.os.Build;
import android.os.Environment;

import androidx.core.content.ContextCompat;

import android.util.DisplayMetrics;
import android.util.Log;
import android.util.TypedValue;

import com.allergoscope.app.App;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.nio.file.Files;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.List;
import java.util.StringTokenizer;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;
import java.util.zip.ZipOutputStream;

import lib.common.ActionCallback;

/**
 * Created by alex on 07.11.17.
 */

public class FileUtils {
    public static ActionCallback log = null;

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public static byte[] Load(String filename)
    {
        File file = new File(filename);
        if (file.exists())
        {
            int size = (int) file.length();
            byte[] bytes = new byte[size];
            FileInputStream buf = null;
            try
            {
                buf = new FileInputStream(file);
                int n = 0, k = 0;
                int dwRead = bytes.length;
                while ((k = buf.read(bytes, n, dwRead)) > 0)
                {
                    n += k;
                    dwRead = size - n;
                }
                buf.close();
                return n == size ? bytes : null;
            } catch (FileNotFoundException e)
            {
                FileUtils.AddToLog(e);
            } catch (IOException e)
            {
                FileUtils.AddToLog(e);
            }
        }
        return null;

    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public static boolean FileExists(String filename)
    {
        File file = new File(filename);
        return file.exists();
    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public static boolean DeleteFile(String filename)
    {
        File file = new File(filename);
        try
        {
            if (file.exists())
                file.delete();
            return true;
        } catch (Exception e)
        {
            FileUtils.AddToLog(e);
        }
        return false;
    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    static Object syncLog = new Object();

    public static boolean AddTextLine(String filename, String text)
    {
        if (Build.VERSION.SDK_INT >= 23)
        {
            boolean fOk = AGlobals.contextApp != null && ContextCompat.checkSelfPermission(AGlobals.contextApp, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED;
            if (!fOk)
                return false;
        }
        File file = new File(filename);
        try
        {
            if (!file.exists())
                file.createNewFile();
            FileWriter fw = new FileWriter(file, true);
            BufferedWriter bw = new BufferedWriter(fw);
            bw.write(text);
            bw.newLine();
            bw.close();
            fw.close();
            return true;
        } catch (IOException e)
        {
            e.printStackTrace();

        }
        return false;
    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public static String GetLogFileName()
    {
        File f = new File(App.defaultPath);
        String path = f.getAbsolutePath();
        path += File.separator;
        return path + "allergoscope.log";

    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public static void DeleteLog()
    {
        String filename = GetLogFileName();
        File f = new File(filename);
        if (f.exists())
            f.delete();

    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public static boolean AddToLog(String text)
    {
        synchronized (syncLog)
        {
            String filename = GetLogFileName();
            if (!text.contains("\n"))
                text += "\r\n";
            final String t = text;
            if (AGlobals.currentActivity != null)
                AGlobals.currentActivity.runOnUiThread(new Runnable() {
                    //-----------------------------------------
                    @Override
                    public void run()
                    {
                        if (log != null)
                            log.run(t);
                    }
                });

            Log.e(">>", text);
            return AddTextLine(filename, text);
        }
    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public static boolean AddToLog(Exception ex)
    {
        StringWriter errors = new StringWriter();
        ex.printStackTrace(new PrintWriter(errors));
        String s = errors.toString();
        ex.printStackTrace();
        Log.e("===Error=====", s);
        //  if (log == null)
        return AddToLog(s);
        // return true;
    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public static boolean SaveText(String filename, ArrayList<String> lines)
    {
        File file = new File(filename);
        try
        {
            if (!file.exists())
                file.createNewFile();
            FileWriter fw = new FileWriter(file, true);
            BufferedWriter bw = new BufferedWriter(fw);
            for (String text : lines)
            {
                bw.write(text);
                bw.newLine();

            }
            bw.close();
            fw.close();
            return true;
        } catch (IOException e)
        {
            FileUtils.AddToLog(e);

        }
        return false;
    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public static boolean ReadText(String filename, ArrayList<String> text)
    {
        File file = new File(filename);
        try
        {
            if (!file.exists()) return false;
            FileReader fr = new FileReader(file);
            BufferedReader br = new BufferedReader(fr);
            while (br.ready())
            {

                String line = br.readLine();
                text.add(line);
            }
            br.close();
            fr.close();
            return true;
        } catch (IOException e)
        {
            FileUtils.AddToLog(e);

        }
        return false;


    }
    //------------------------------------------------------
    //
    //------------------------------------------------------
    public static boolean Save(String filename, byte [] bytes, Date date)
    {
        return Save(new File(filename), bytes, date);
    }
    //------------------------------------------------------
    //
    //------------------------------------------------------
    public static boolean Save(File file, byte [] bytes)
    {
        return Save(file, bytes, null);
    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public static boolean Save(File file, byte [] bytes, Date date)
    {
        FileOutputStream out = null;
        boolean fRet = false;
        try {
           //if (file.exists())
                file.createNewFile();
            out = new FileOutputStream(file);
            if (bytes != null)
                out.write(bytes);
            out.flush();
            out.close();

            if (date != null)
                file.setLastModified(date.getTime());
            fRet = true;
        } catch (FileNotFoundException e) {
            FileUtils.AddToLog(e);
        }
        catch (IOException e) {
            FileUtils.AddToLog(e);
        }
        return fRet;
    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public static boolean Copy(String src, String dst) throws IOException
    {
        File source = new File(src);
        File dest = new File(dst);
        return Copy(source, dest);
    }
    //------------------------------------------------------
    //
    //------------------------------------------------------
    public static boolean CopyDir(File source, File dest)
    {
        File[] files = source.listFiles();
        if (files != null)
        {
            for (File file : files)
            {
                if (file.isDirectory())
                {
                    String ph = dest.getAbsolutePath();
                    String str = file.getAbsolutePath();
                    int i = str.lastIndexOf("/");
                    File dir = new File(ph + "/" + str.substring(i + 1, str.length()));
                    if (!dir.exists())
                    {
                        dir.mkdirs();
                    }
                    if (!CopyDir(file, dir)) return false;


                }
                else
                if (!Copy(file, dest)) return false;
            }
        }

        return true;

    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public static boolean Copy(File source, File _dest)
    {
        InputStream is = null;
        OutputStream os = null;
        try
        {
            File dest = null;
            if (source.isDirectory() && _dest.isDirectory())
            {
                return CopyDir(source, _dest);
            }
            if (!_dest.isDirectory() && source.isDirectory())
                return false;
            else
            if (_dest.isDirectory() && !source.isDirectory())
            {
                String ph = _dest.getAbsolutePath();
                String str = source.getAbsolutePath();
                int i = str.lastIndexOf("/");
                File file = new File(ph + "/" + str.substring(i + 1, str.length()));

                file.createNewFile();
                dest = file;
            }
            else
                dest = _dest;
            is = new FileInputStream(source);

            os = new FileOutputStream(dest);
            byte[] buffer = new byte[1024];
            int length;
            while ((length = is.read(buffer)) > 0)
            {
                os.write(buffer, 0, length);
            }
            is.close();
            os.close();
            return true;
        }
        catch(IOException e)
        {
            AddToLog(e);
        }
        return false;
    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public static boolean Save(String filename, byte[] bytes)
    {
        return Save(filename, bytes, null);
    }
    //------------------------------------------------------
    //
    //------------------------------------------------------
    public static long GetDirSize(String _dir)
    {
        long size = 0;
        try {
            File dir =  new File(_dir);
            if (dir.exists())
                 size = GetDirSize(dir);
        }
        catch(Exception e)
        {
            FileUtils.AddToLog(e);
        }

        return size;

    }
    //------------------------------------------------------
    //
    //------------------------------------------------------
    public static long GetDirSize(File dir)
    {
        long size = 0;
        if (dir.exists())
        {
            try
            {
                File[] files = dir.listFiles();
                if (files != null)
                {
                    for (File file : files)
                    {
                        if (file.isDirectory())
                            size += GetDirSize(file);
                        else
                            size += file.length();
                    }
                }

            } catch (Exception e)
            {
                FileUtils.AddToLog(e);
            }
        }
        return size;

    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public static boolean DeleteDir(String _dir)
    {
        File dir = new File(_dir);
        return DeleteDir(dir);

    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public static boolean DeleteDir(File dir)
    {
        try
        {
            if (dir.exists())
            {
                File[] files = dir.listFiles();

                if (files != null)
                {
                    for (File file : files)
                    {
                        if (file.isDirectory())
                            DeleteDir(file);
                        else
                        file.delete();
                    }
                }
                dir.delete();
            }
        }
       catch(Exception e)
            {
                return false;
            }
        return true;
    }
    //----------------------------------------------
    //
    //----------------------------------------------
    public static boolean Clear(File dir)
    {

        try
        {
            if (dir.exists())
            {
                File[] files = dir.listFiles();

                if (files != null)
                {
                    for (File file : files)
                    {
                        if (file.isDirectory())
                            DeleteDir(file);
                        else
                            file.delete();
                    }
                }
            }
        }
        catch(Exception e)
        {
            return false;

        }
        return true;

    }
    //----------------------------------------------
    //
    //----------------------------------------------
    public static void Clear(File dir, String[] mask) throws IOException
    {
        String ext, name;
        if (dir.exists())
        {
            long size = GetDirSize(dir);
            File[] files = dir.listFiles();

            if (files != null)
            {
                for (File file : files)
                {
                    if (file.isDirectory())
                        DeleteDir(file);
                    else
                    {
                        if (mask == null)
                            file.delete();
                        else
                        {
                            name = file.getName();;
                            if (name.length() > 4)
                            {
                                ext = name.substring(name.length() - 4, name.length());
                                for (String g : mask)
                                    if (ext.equalsIgnoreCase(g))
                                    {
                                        file.delete();
                                        break;
                                    }
                        }
                        }
                    }


                }
            }
        }

    }
    //----------------------------------------------
    //
    //----------------------------------------------
    public static void MakeDir(String _dir) throws IOException
    {
        File dir = new File(_dir);
        if (!dir.exists())
            dir.mkdirs();
    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public static void DeleteFilesDir(File dir) throws IOException
    {
        if (dir.exists())
        {
            long size = GetDirSize(dir);
            File[] files = dir.listFiles();

            if (files != null)
            {
                for (File file : files)
                {
                    if (file.isDirectory())
                        DeleteFilesDir(file);
                    else
                        file.delete();
                }
            }

        }
    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public static int GetFileList(String path, List<String> fls, String[] mask, boolean fFindSubdir)
    {
        File currentDir = new File(path);
        GetFileList(currentDir, mask, fFindSubdir, fls);
        return fls.size();
    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public static void GetFileList(File f, String[] mask, boolean fFindSubdir, List<String> result)
    {
        if (f == null) return;
        String ext, name = f.getName();
        //     if (f.isHidden() || name.indexOf('.') != -1)
        //       return;

        File [] files = f.listFiles();
        if (files == null) return;
        for(File ff: files)
        {
            try {
            name = ff.getName();
            if (ff.isHidden() || name.getBytes()[0] == '.')
                continue;

            ext = name.substring(0, 1);

            if (ff.isDirectory())
            {

                if (fFindSubdir)
                    GetFileList(ff, mask, fFindSubdir, result);
            } else
            {

                if (name.length() > 4)
                {
                    ext = name.substring(name.length() - 4, name.length());
                    if (mask == null)
                    {
                        result.add(ff.getAbsolutePath());
                    } else
                        for (String g : mask)
                            if (ext.equalsIgnoreCase(g))
                            {
                                result.add(ff.getAbsolutePath());
                                break;
                            }
                }
            }
            }catch(Exception e)
            {
                FileUtils.AddToLog(e);
            }
        }
    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public static void GetFileList(File f, String[] mask, ArrayList<File> dir, ArrayList<File> file)
    {
        if (f == null) return;
        String ext, name = f.getName();
        // if (f.isHidden() || name.indexOf('.') != -1)
        //    return;

        File[] files = f.listFiles();
        if (files == null) return;
        for (File ff : files)
        {
            name = ff.getName();
            if (ff.isHidden() || name.getBytes()[0] == '.')
                continue;

            ext = name.substring(0, 1);

            if (ff.isDirectory())
            {
                dir.add(ff);
            } else
            {

                if (name.length() > 4)
                {
                    ext = name.substring(name.length() - 4, name.length());
                    if (mask == null)
                    {
                        file.add(ff);
                    } else
                        for (String g : mask)
                            if (ext.equalsIgnoreCase(g))
                            {
                                file.add(ff);
                                break;
                            }
                }
            }
        }
    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public static void GetDirList(File f, List<String> dirs)
    {
        File[] files = f.listFiles();
        if (files != null)
        {
            for (File ff : files)
            {
                if (ff.isDirectory())
                    dirs.add(ff.getAbsolutePath());
            }
        }
    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public static void GetDirList(String path, List<String> dirs)
    {
        GetDirList(new File(path), dirs);
    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public static File[] GetListDirs(String path)
    {
        ArrayList<String> dirs = new ArrayList<String>();
        GetDirList(path, dirs);
        int sz = dirs.size();
        File[] f = new File[sz];
        for (int i = 0; i < sz; ++i)
            f[i] = new File(dirs.get(i));
        return f;
    }
    //----------------------------------------------
    //
    //----------------------------------------------
    @SuppressLint("SimpleDateFormat")
    public static String StringLastModify(String url)
    {
        File file = new File(url);
        Long last_modi = file.lastModified();
        String date = "";
        if (last_modi != 0)
        {
            SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH@mm@ss");
            date = sdf.format(last_modi);
        }
        return date;
    }
    //----------------------------------------------
    //
    //----------------------------------------------
    public static boolean UnZip(String zipFile, String  dstDir)
    {
        return UnZip(new File(zipFile), new File (dstDir));
    }
    //----------------------------------------------
    //
    //----------------------------------------------
    public static int ReadZip(File zipFile, String[] mask, ArrayList<String>  files) throws IOException
    {
        ZipInputStream zip = null;
        InputStream is = new FileInputStream(zipFile);
       zip = new ZipInputStream(new BufferedInputStream(is));
        ZipEntry entry = zip.getNextEntry();
        if (entry == null)
        {
            zip.close();
            throw  new IOException();

        }
        while (entry != null)
        {
            if (!entry.isDirectory())
            {
                String name = entry.getName();
                if (mask == null)
                files.add(name);
                else
                {
                    if (name.length() > 4)
                    {
                        String ext = name.substring(name.length() - 4, name.length());
                        for (String g : mask)
                            if (ext.equalsIgnoreCase(g))
                            {
                                files.add(name);
                                break;
                            }
                    }
                }
            }
            entry = zip.getNextEntry();
        }
        zip.close();
        if (files.size() > 1)
            Collections.sort(files);
        return files.size();

    }
    public static File CreateTempDir(String path, String prefix)
    {
        return CreateTempDir(new File(path), prefix);
    }
    public static File CreateTempDir(File path, String prefix)
    {
        File dir = null;
        try {
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O)
            dir = new File(Files.createTempDirectory(path.toPath(), prefix).toAbsolutePath().toString());
        else
        {
            dir = File.createTempFile(prefix, prefix, path);
            if(!(dir.delete()))
                throw new IOException("error delete file ->" + dir.getAbsolutePath());
            if(!(dir.mkdir()))
                throw new IOException("error mkdir ->" + dir.getAbsolutePath());
        }
        }catch (IOException e)
        {
            AddToLog(e);
            dir = null;
        }
        return dir;

    }


    public static boolean CheckPath(File dstDir)
    {

        String dir = dstDir.getAbsolutePath();
        StringTokenizer st = new StringTokenizer(dir, "/");
        int n = st.countTokens();
        String ph = "/";
        String s;
        File file;
        try {


            for (int i = 0; i < n - 1; ++i)
         if (st.hasMoreTokens())
        {
            s = ph + st.nextToken();
            file = new File(s);
            if (!file.exists())
                file.mkdirs();
            ph = s + "/";
        }
        }
    catch(Exception e)
        {
            return false;

        }
        return true;
    }

    public static boolean UnZip(File zipFile, File dstDir, ArrayList<String>  files)
    {
        ZipInputStream zip = null;
        if (!dstDir.exists())
            dstDir.mkdirs();
        try {

            zip = new ZipInputStream(new BufferedInputStream(new FileInputStream(zipFile)));
            byte[] buffer = new byte[1024];
            ZipEntry entry;

            while ((entry = zip.getNextEntry()) != null)
            {
                File file = new File(dstDir, entry.getName());
                if (entry.isDirectory())
                {
                    if (!file.exists())
                        file.mkdirs();
                }
                else
                {
                    int index = 0;
                    if ((index = Collections.binarySearch(files, entry.getName())) >= 0)
                    {
                        int count;
                        CheckPath(file);
                        FileOutputStream fout = new FileOutputStream(file);
                        while ((count = zip.read(buffer)) != -1)
                            fout.write(buffer, 0, count);
                        fout.close();
                        files.set(index, file.getAbsolutePath());
                    }
                }
            }
            zip.close();

        }
        catch(Exception e)
        {
            if (zip != null)
            {
                try
                {
                    zip.close();
                } catch (IOException ex)
                {
                    ex.printStackTrace();
                }
            }
            return false;
        }
        return true;
    }
    //----------------------------------------------
    //
    //----------------------------------------------
    public static boolean UnZip(File zipFile, File dstDir)
    {
        ZipInputStream zip = null;
        if (!dstDir.exists())
            dstDir.mkdirs();

        try {
            zip = new ZipInputStream(new BufferedInputStream(new FileInputStream(zipFile)));
            byte[] buffer = new byte[1024];
            ZipEntry entry;
            while ((entry = zip.getNextEntry()) != null)
            {
                File file = new File(dstDir, entry.getName());
                if (entry.isDirectory())
                {
                    if (!file.exists())
                        file.mkdirs();
                }
                else
                {
                    int count;
                    CheckPath(file);

                    FileOutputStream fout = new FileOutputStream(file);
                     while ((count = zip.read(buffer)) != -1)
                        fout.write(buffer, 0, count);
                     fout.close();

                }
            }
            zip.close();

        }
        catch(Exception e)
        {
            if (zip != null)
            {
                try
                {
                    zip.close();
                } catch (IOException ex)
                {
                    ex.printStackTrace();
                }
            }
           return false;
        }
        return true;
    }
    //----------------------------------------------
    //
    //----------------------------------------------
    public static boolean Zip(String zipFile, File dir)
    {
        ZipOutputStream out = null;
        try
        {
            out = new ZipOutputStream(new BufferedOutputStream(new FileOutputStream(zipFile)));
            boolean res =  Zip(out, dir, dir);
            out.close();
            if (!res)
                (new File(zipFile)).delete();
            return res;

        }
        catch (IOException e)
        {
            AddToLog(e);
        }
        return false;

    }
    //----------------------------------------------
    //
    //----------------------------------------------
    public static boolean Zip(String zipFile, File [] files)
    {
        ZipOutputStream out = null;
        try
        {
            out = new ZipOutputStream(new BufferedOutputStream(new FileOutputStream(zipFile)));
            byte data[] = new byte[1024];
            if (files != null)
            {
                for (int i = 0; i < files.length; i++)
                {
                    ZipEntry entry = new ZipEntry(files[i].getName());
                    entry.setTime(files[i].lastModified());
                    out.putNextEntry(entry);
                    FileInputStream fi = new FileInputStream(files[i]);
                    BufferedInputStream origin = new BufferedInputStream(fi, 1024);

                    int count;
                    while ((count = origin.read(data, 0, 1024)) != -1)
                        out.write(data, 0, count);
                    origin.close();
                }
            }
            out.close();
            return true;

        }
        catch (IOException e)
        {
            AddToLog(e);
        }
        return false;

    }

    //----------------------------------------------
    //
    //----------------------------------------------
    private static boolean Zip(ZipOutputStream out, File dir, File rootDir)
    {

        try {
            byte data[] = new byte[1024];

            File [] files = dir.listFiles();
            if (files != null)
            {
                for (int i = 0; i < files.length; i++)
                {
                    ZipEntry entry = new ZipEntry(rootDir.toURI().relativize(files[i].toURI()).getPath());
                    entry.setTime(files[i].lastModified());
                    out.putNextEntry(entry);
                    if(files[i].isDirectory())
                    {
                        out.closeEntry();
                        if (!Zip(out, files[i], rootDir))
                            return false;
                    }
                    else
                    {
                        FileInputStream fi = new FileInputStream(files[i]);
                        BufferedInputStream origin = new BufferedInputStream(fi, 1024);

                        int count;
                        while ((count = origin.read(data, 0, 1024)) != -1)
                            out.write(data, 0, count);
                        origin.close();
                    }
                }
            }

        } catch(Exception e) {

            AddToLog(e);
            return false;
        }
        return true;

    }
    //----------------------------------------------
    //
    //----------------------------------------------
    public static boolean ReadFromAssets(String name, ArrayList<String> text)
    {
        int old = text.size();
        try
        {
            InputStream is= AGlobals.currentActivity.getAssets().open(name);
            InputStreamReader fr = new InputStreamReader(is);
            BufferedReader br = new BufferedReader(fr);
            StringBuilder builder = new StringBuilder();
            if (br.ready())
                br.readLine();
            while (br.ready())
            {
                String line = br.readLine();
                if (line == null)
                    break;
                text.add(line);
            }
            br.close();
            fr.close();
            is.close();


        } catch (IOException e)
        {
            e.printStackTrace();
        }
        return text.size() != old;
    }
    //----------------------------------------------
    //
    //----------------------------------------------
    public static boolean ReadFromAssets(String name, String text)
    {
        try
        {
            InputStream is= AGlobals.currentActivity.getAssets().open(name);
            InputStreamReader fr = new InputStreamReader(is);
            BufferedReader br = new BufferedReader(fr);
            StringBuilder builder = new StringBuilder();
            if (br.ready())
                br.readLine();
            while (br.ready())
            {
                String line = br.readLine();
                if (line == null)
                    break;
                text += line;
                if (br.ready())
                    text += "\r\n";
            }
            br.close();
            fr.close();
            is.close();


        } catch (IOException e)
        {
            e.printStackTrace();
            return false;
        }
        return true;
    }


    //----------------------------------------------
    //
    //----------------------------------------------
    public static byte[] ReadFromAssets(String name)
    {
        byte[] fileBytes = null;
        try
        {
            InputStream is= AGlobals.currentActivity.getAssets().open(name);
            fileBytes = new byte[is.available()];
            is.read( fileBytes);
            is.close();
        } catch (IOException e)
        {
            e.printStackTrace();
            fileBytes = null;
        }
        return fileBytes;
    }
    //----------------------------------------------
    //
    //----------------------------------------------
    public static boolean ExtractAssetsFile(String path, String name)
    {
        byte[] fileBytes = null;
        try
        {
            File file2 = new File(path + "/" +  name);
            if (!file2.exists())
            {
                InputStream is= AGlobals.currentActivity.getAssets().open(name);
                fileBytes = new byte[is.available()];
                is.read( fileBytes);
                is.close();
                CheckPath(file2);

                file2.createNewFile();
                FileOutputStream fout = new FileOutputStream(file2);
                fout.write(fileBytes);
                fout.close();
            }
        } catch (IOException e)
        {
            e.printStackTrace();
            return false;
        }
        return true;
    }
    //----------------------------------------------
    //
    //----------------------------------------------
    public static boolean ExtractAssetsDir(String pathOut, String pathIn)
    {
        Object[] dirs = null;
        String Dir = pathIn;
        try {
            dirs = AGlobals.App.getAssets().list(Dir);
            int i = 0;
            if (dirs != null)
                for(Object dir : dirs)
                {
                    ExtractAssetsFile(pathOut, pathIn + "/" + (String)dir);
                }
        }
        catch (IOException e)
        {
            FileUtils.AddToLog(e);
            return false;

        }
        return true;
    }
    //----------------------------------------------
    //
    //----------------------------------------------
    static public  ArrayList<String> GetAssetsList(String Dir, String [] mask)
    {
        ArrayList<String> sfile = new ArrayList<String>();
        try {
            Object[] files = AGlobals.App.getAssets().list(Dir);
            int i = 0;
            if (files != null)
                for(Object file : files)
                 {
                     String name = (String) file;
                     if (mask != null)
                     {
                         if (name.length() > 4)
                         {
                             String ext = name.substring(name.length() - 4, name.length());
                             if (mask == null)
                             {
                                 sfile.add(name);
                             }
                             else
                                 for (String g : mask)
                                     if (ext.equalsIgnoreCase(g))
                                     {
                                         sfile.add(name);
                                         break;
                                     }
                         }
                     }
                }
        }
        catch (IOException e)
        {
            FileUtils.AddToLog(e);
            return null;

        }
        return sfile.size() > 0 ? sfile : null;
    }

}
