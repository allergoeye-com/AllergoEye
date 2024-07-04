package lib;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;


import lib.common.CTimer;


/**
 * Created by alex on 11.11.17.
 */

public class ExitProgramActivity extends OrientActivity
{
    CTimer timer = new CTimer(this, "onExit");
    @Override protected void onCreate(Bundle savedInstanceState)
    {


        super.onCreate(savedInstanceState);

        if(android.os.Build.VERSION.SDK_INT >= 21)
        {
            finishAndRemoveTask();
        }
        else
        {
            finish();
        }
        System.exit(1);
    }
    public void onExit()
    {
        timer.Stop();

    }
    public static void exitApplication(Context context)
    {

        Intent intent = new Intent(context, ExitProgramActivity.class);

        intent.addFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT | Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK | Intent.FLAG_ACTIVITY_NO_ANIMATION | Intent.FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);

        context.startActivity(intent);
    }
}
