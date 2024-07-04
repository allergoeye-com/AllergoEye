package lib.common;

import android.os.Looper;

import androidx.fragment.app.Fragment;
import android.view.View;
import android.widget.CheckBox;
import android.widget.TextView;

import lib.utils.FileUtils;

/**
 * Created by alex on 26.02.2018.
 */

public class CFragment extends Fragment {

    //---------------------------------------------------------------------
    //
    //---------------------------------------------------------------------
    public void checkButton(final int checkBoxId,final boolean checked) {

        View v = getView();
        if (v != null)
        {
            try {
            CheckBox c = (CheckBox) v.findViewById(checkBoxId);
            if (c != null)
                c.setChecked(checked);
            }
            catch (Exception e)
            {
                FileUtils.AddToLog(e);
            }


        }

    }
    //---------------------------------------------------------------------
    //
    //---------------------------------------------------------------------
    public void CheckButton(final int checkBoxId,final boolean checked) {
        if (Thread.currentThread() == Looper.getMainLooper().getThread())
            checkButton(checkBoxId, checked);
        else
            getActivity().runOnUiThread(new Runnable(){

                @Override
                public void run() {
                    checkButton(checkBoxId, checked);

                }});

    }
    //---------------------------------------------------------------------
    //
    //---------------------------------------------------------------------
    public void enableButton(final int buttonId,final boolean enabled)
    {
        try {
        View v = getView();
        if (v != null)
        {
            View c =v.findViewById(buttonId);
            if (c != null)
                c.setEnabled(enabled);
        }
        }
        catch (Exception e)
        {
            FileUtils.AddToLog(e);
        }

    }
    //---------------------------------------------------------------------
    //
    //---------------------------------------------------------------------
    public void EnableButton(final int buttonId,final boolean enabled)
    {
        if (Thread.currentThread() == Looper.getMainLooper().getThread())
            enableButton(buttonId, enabled);
        else
            getActivity().runOnUiThread(new Runnable(){

                @Override
                public void run() {
                    enableButton(buttonId, enabled);
                }});
    }
    //---------------------------------------------------------------------
    //
    //---------------------------------------------------------------------
    public void setText(final int text_view_id,final String text) {
        try {

        View v = getView();
        if (v != null)
        {
            TextView tv = v.findViewById(text_view_id);
            if (tv != null)
                tv.setText(text);
        }
        }
        catch (Exception e)
        {
            FileUtils.AddToLog(e);
        }

    }

    //---------------------------------------------------------------------
    //
    //---------------------------------------------------------------------
    public void SetText(final int text_view_id,final String text) {
        if (Thread.currentThread() == Looper.getMainLooper().getThread())
            setText(text_view_id, text);
        else
            getActivity().runOnUiThread(new Runnable(){

                public void run() {

                    setText(text_view_id, text);
                }});
    }
}
