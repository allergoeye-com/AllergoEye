package lib.print;

import android.content.Context;
import android.graphics.pdf.PdfDocument;
import android.os.Bundle;
import android.os.CancellationSignal;
import android.print.PrintAttributes;
import android.print.PrintDocumentAdapter;
import android.print.PrintDocumentInfo;
import android.print.PrintManager;
import android.print.pdf.PrintedPdfDocument;

import androidx.print.PrintHelper;


import java.io.FileOutputStream;
import java.io.IOException;

import lib.common.ActionCallback;
import lib.utils.AGlobals;

public class PrintAndPreview extends PrintDocumentAdapter {


    PrintedPdfDocument mPdfDocument;
    ActionCallback mDraw;
    ActionCallback mError;
    ActionCallback mInit;
    Context mContext;

    private PrintAndPreview(Context context, ActionCallback onInit, ActionCallback draw, ActionCallback onError)
    {
        mDraw = draw;
        mContext = context;
        mInit = onInit;
        mError = onError;
    }

    @Override
    public void onStart()
    {
        super.onStart();
    }

    static public PrintAndPreview InitInstance(String jobName, Context context, PrintAttributes.MediaSize orient, ActionCallback onInit, ActionCallback draw, ActionCallback onError)
    {
        PrintAndPreview adapter = null;
        if (PrintHelper.systemSupportsPrint())
        {
            adapter = new PrintAndPreview(context, onInit, draw, onError);
            PrintManager printManager = (PrintManager) AGlobals.currentActivity.getSystemService(Context.PRINT_SERVICE);
            PrintAttributes attrib = new PrintAttributes.Builder().setMediaSize(orient).setMinMargins(PrintAttributes.Margins.NO_MARGINS).setColorMode(PrintAttributes.COLOR_MODE_COLOR).build(); // PrintAttributes.MediaSize.UNKNOWN_PORTRAIT, PrintAttributes.MediaSize.UNKNOWN_LANDSCAPE
            /*PrintAttributes attrib = new PrintAttributes.Builder()
                    .setMediaSize(PrintAttributes.MediaSize.ISO_A4)
                    .setResolution(new PrintAttributes.Resolution("pdf", "pdf", 600, 600))
                    .setMinMargins(PrintAttributes.Margins.NO_MARGINS).build();*/
            printManager.print(jobName, adapter, attrib);
        } else
            onError.run("NotSupports");
        return adapter;
    }

    @Override
    public void onFinish()
    {
        super.onFinish();
        mInit.run((PrintAttributes) null);
    }

    @Override
    public void onLayout(PrintAttributes oldAttributes, PrintAttributes newAttributes, CancellationSignal cancellationSignal, PrintDocumentAdapter.LayoutResultCallback callback, Bundle extras)
    {
        try
        {
            mPdfDocument = new PrintedPdfDocument(mContext, newAttributes);
            if (cancellationSignal.isCanceled())
            {
                callback.onLayoutCancelled();
                mError.run("cancel");
                return;
            }
            int pages = 1;

            mInit.run(newAttributes);
            PrintDocumentInfo info = new PrintDocumentInfo.Builder("redeye.pdf")
                    .setContentType(PrintDocumentInfo.CONTENT_TYPE_DOCUMENT)
                    .setPageCount(pages).build();

            boolean changed = !newAttributes.equals(oldAttributes);
            callback.onLayoutFinished(info, changed);
        } catch (Exception e)
        {
            mError.run("exception");
        }
    }

    @Override
    public void onWrite(android.print.PageRange[] pages,
                        android.os.ParcelFileDescriptor destination,
                        CancellationSignal cancellationSignal, PrintDocumentAdapter.WriteResultCallback callback)
    {
        if (mPdfDocument == null)
        {
            mError.run("write error");
            mInit.run((PrintAttributes) null);

            return;
        }
        PdfDocument.Page page;
        try
        {
            page = mPdfDocument.startPage(1);
            if (cancellationSignal.isCanceled())
            {
                callback.onWriteCancelled();
                mPdfDocument.close();
                mPdfDocument = null;
                mInit.run((PrintAttributes) null);
                return;
            }
        } catch (Exception e)
        {
            mInit.run((PrintAttributes) null);
            return;
        }
        mDraw.run(page.getCanvas());
        mPdfDocument.finishPage(page);

        try
        {
            mPdfDocument.writeTo(new FileOutputStream(destination
                    .getFileDescriptor()));
        } catch (IOException e)
        {
            callback.onWriteFailed(e.toString());
            return;
        } finally
        {
            mPdfDocument.close();
            mPdfDocument = null;
            mInit.run((PrintAttributes) null);
        }
        callback.onWriteFinished(pages);
    }
}
