package lib.utils;

import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Point;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.provider.MediaStore;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.HashMap;

/**
 * Created by alex on 09.11.17.
 */

public class BitmapUtils {

    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    static public Bitmap Create(int w, int h)
    {
        Bitmap bmp = null;
        Bitmap.Config b = Bitmap.Config.valueOf("ARGB_8888");
        return Bitmap.createBitmap(w, h, b);
    }
    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    public static Bitmap ResizeBitmap(Bitmap bm, int newWidth, int newHeight)
    {
        if (bm != null && !bm.isRecycled())
            return bm.getWidth() == newWidth && bm.getHeight() == newHeight ? bm.copy(Bitmap.Config.ARGB_8888, true) :
                                                                    Bitmap.createScaledBitmap(bm,  newWidth, newHeight, true);
        return null;

    }
    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    public static Bitmap Copy(Bitmap bm)
    {
        return bm != null && !bm.isRecycled() ? bm.copy(Bitmap.Config.ARGB_8888, true) : null;
    }

    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    public static Bitmap DrawableToBitmap (Drawable drawable)
    {
        if (drawable instanceof BitmapDrawable) {
            return ((BitmapDrawable)drawable).getBitmap();
        }

        int width = drawable.getIntrinsicWidth();
        width = width > 0 ? width : 1;
        int height = drawable.getIntrinsicHeight();
        height = height > 0 ? height : 1;

        Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(bitmap);
        drawable.setBounds(0, 0, canvas.getWidth(), canvas.getHeight());
        drawable.draw(canvas);

        return bitmap;
    }
    static public Bitmap LoadAssets(String filePath)
    {
        InputStream stream;
        Bitmap bitmap = null;
        try {

            stream = AGlobals.currentActivity.getAssets().open(filePath);
            bitmap = BitmapFactory.decodeStream(stream);
            stream.close();
        } catch (IOException e) {
                   // FileUtils.AddToLog(e);
            bitmap = null;
        }
        return bitmap;
    }

    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    static public Bitmap Load(String filename, int scale)
    {
       Bitmap bmp = null;
        BitmapFactory.Options options;
        options = new BitmapFactory.Options();
        options.inPreferredConfig = Bitmap.Config.ARGB_8888;
        if (scale > 1)
            options.inSampleSize = scale;
     //   options.inTempStorage = new byte[32*1024];
       // options.inPurgeable = true;

        try {
       bmp = BitmapFactory.decodeFile(filename, options);
        }
        catch (Exception e)
        {
            return null;
        }
       return bmp;
       /* File pictureFile = new File(filename);
        InputStream input = null;
        try {
            input = AGlobals.contentResolver.openInputStream(Uri.fromFile(pictureFile));
        } catch (FileNotFoundException e) {
            FileUtils.AddToLog(e);
        }
        if (input != null)
        {
            BitmapFactory.Options options;
            options = new BitmapFactory.Options();
            options.inPreferredConfig = Bitmap.Config.ARGB_8888;
            if (scale > 1)
                options.inSampleSize = scale;

            bmp = BitmapFactory.decodeStream(input, null, options);
            try {
                input.close();
            } catch (IOException e) {
                FileUtils.AddToLog(e);
            }
            try {
                input.close();
            } catch (IOException e) {
                FileUtils.AddToLog(e);
            }
        }
        return bmp;
        */
     /*

     Bitmap bmp = null;
        File pictureFile = new File(filename);
        FileInputStream pStream = null;
        try {
        if (pictureFile.exists())
        {
            pStream = new FileInputStream(filename);
            if (pStream != null)
            {
                FileDescriptor fd = null;
                fd = pStream.getFD();
                if (fd != null)
                {
                    BitmapFactory.Options options;
                    options = new BitmapFactory.Options();
                    options.inPreferredConfig = Bitmap.Config.ARGB_8888;
                    options.inSampleSize = scale;
                    bmp = BitmapFactory.decodeFileDescriptor(fd, null, options);
                }



                pStream.close();
                pStream = null;

            }
        }
        }catch (Exception e) {
        FileUtils.AddToLog(e);
        } finally {
            if (pStream != null)
                try {
                    pStream.close();
                } catch (IOException e) {
                    FileUtils.AddToLog(e);
                }
        }

        return bmp;
        */
    }
    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    static public Bitmap Load(byte [] b, int scale) throws Exception
    {
        BitmapFactory.Options options;
        options = new BitmapFactory.Options();
        options.inPreferredConfig = Bitmap.Config.ARGB_8888;
        options.inSampleSize = scale;
      //  options.inJustDecodeBounds = true;
        return BitmapFactory.decodeByteArray(b, 0, b.length, options);
    }

    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    static public boolean Save(File pictureFile, Bitmap bitmap)
    {
        boolean fRet = false;
        if (pictureFile != null && bitmap != null)
        {
            String str = pictureFile.getAbsolutePath();
            int i = str.lastIndexOf("/");
            File dir = new File(str.substring(0, i));
            if (!dir.exists())
                dir.mkdirs();

            OutputStream stream = null;
            try {
                stream = AGlobals.contentResolver.openOutputStream(Uri.fromFile(pictureFile));
                bitmap.compress(Bitmap.CompressFormat.PNG, 20, stream);
                stream.close();
                fRet = true;
            } catch (FileNotFoundException e) {
                FileUtils.AddToLog(e);
            }
            catch (IOException e) {
            FileUtils.AddToLog(e);
            }
        }

        return fRet;
    }
    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    static public boolean Save(String filename, Bitmap bitmap)
    {
        return filename != null && bitmap != null && Save(new File(filename), bitmap);
    }
    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    public static Point Dimensions(String filename) throws IOException
    {
        InputStream input = null;
        Point size = null;
        try {
        BitmapFactory.Options options;
        options = new BitmapFactory.Options();
        File pictureFile = new File(filename);
        input = AGlobals.contentResolver.openInputStream(Uri.fromFile(pictureFile));
        if (input != null)
        {
            options.inJustDecodeBounds = true;
            BitmapFactory.decodeStream(input, null, options);
            size = new  Point(options.outWidth, options.outHeight);
            input.close();
        }
        }
        catch (Exception e)
        {
            if (input != null)
                input.close();
            return new Point (0, 0);
        }
        return size;
    }
    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    public static Point Dimensions(byte [] data)
    {
        Point size = null;
        BitmapFactory.Options options;
        options = new BitmapFactory.Options();
        options.inJustDecodeBounds = true;
        BitmapFactory.decodeByteArray(data, 0, data.length, options);
        size = new  Point(options.outHeight, options.outWidth);
        return size;
    }
    //--------------------------------------------------
    //
    //--------------------------------------------------
    public static HashMap<String, String> getLocalThumbs()
    {
        try {
        HashMap<String, String> result = new HashMap<String, String>();
        String[] proj = {MediaStore.Images.Media.DATA, MediaStore.Images.Thumbnails.IMAGE_ID };
        Cursor cursor = AGlobals.contentResolver.query( MediaStore.Images.Thumbnails.EXTERNAL_CONTENT_URI, proj, null, null, null);
        if (cursor != null)
        {
            int columnIndex1 = cursor.getColumnIndex(MediaStore.Images.Thumbnails.DATA);
            int columnIndex2 = cursor.getColumnIndex(MediaStore.Images.Thumbnails.IMAGE_ID);
            int count = cursor.getCount();
            for(int i =0; i< count; ++i)
            {
                String s2 = null, s1 = null;
                cursor.moveToPosition(i);
                long id = Long.parseLong(cursor.getString(columnIndex2));
                s1 = cursor.getString(columnIndex1);
                s2 = null;
                if (s1 != null)
                {
                    String[] column = { MediaStore.Images.Media.DATA };
                    String query = MediaStore.Images.Media._ID + " LIKE ?";
                    String val[] = {"%" + id};
                    Cursor mCursor = AGlobals.contextApp.getContentResolver().query(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, column, query, val, null);
                    if (mCursor != null)
                    {
                        int rawDataPath= mCursor.getColumnIndex(MediaStore.Images.Media.DATA);
                        for (mCursor.moveToFirst();!mCursor.isAfterLast(); mCursor.moveToNext())
                            s2 = mCursor.getString(rawDataPath);
                        mCursor.close();
                    }
                }
                if (s1 != null && s2 != null)
                    result.put(s2, s1);

            }
            cursor.close();
        }
        return result;
        }
        catch(Exception e)
        {
            return null;
        }

    }
}
