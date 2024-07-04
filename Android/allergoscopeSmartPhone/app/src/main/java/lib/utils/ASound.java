package lib.utils;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.ContentResolver;
import android.content.Context;
import android.media.AudioAttributes;
import android.media.Ringtone;
import android.media.RingtoneManager;
import android.net.Uri;
import android.os.Build;
import android.os.Vibrator;

import androidx.core.app.NotificationCompat;


import static androidx.core.app.NotificationCompat.DEFAULT_ALL;
import static androidx.core.app.NotificationCompat.DEFAULT_SOUND;
import static androidx.core.app.NotificationCompat.DEFAULT_VIBRATE;
import static androidx.core.app.NotificationCompat.FLAG_SHOW_LIGHTS;

public class ASound {

    public static void PlayAudio(String text)
    {
        Uri uri = Uri.parse(ContentResolver.SCHEME_ANDROID_RESOURCE + "://"
                + AGlobals.contextApp.getPackageName() +
                "/raw/" + text); //error_message");

        Ringtone r = RingtoneManager.getRingtone(AGlobals.contextApp, uri);
        r.play();
    }

    static void Test(String text)
    {
//        .setSound(Uri.parse("android.resource://"
        //          + context.getPackageName() + "/"
        //        + R.raw.alert))
        AudioAttributes audioAttributes = new AudioAttributes.Builder()
                .setContentType(AudioAttributes.CONTENT_TYPE_SONIFICATION)
                .setUsage(AudioAttributes.USAGE_ALARM)
                .build();
        Uri uri = Uri.parse(ContentResolver.SCHEME_ANDROID_RESOURCE + "://"
                + AGlobals.contextApp.getPackageName() +
                "/raw/error_message");
        if (true)
        {
            Ringtone r = RingtoneManager.getRingtone(AGlobals.contextApp, uri);
            r.play();
//            MediaPlayer mp =MediaPlayer.create(AGlobals.currentActivity, R.raw.error_message);
            //          mp.start();
            return;
        }
        NotificationManager notificationManager = (NotificationManager) AGlobals.contextApp.getSystemService(Context.NOTIFICATION_SERVICE);
        NotificationCompat.Builder builder = null;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
        {
            int importance = NotificationManager.IMPORTANCE_HIGH;
            NotificationChannel notificationChannel = new NotificationChannel("IDZ", "NameZDC", importance);
            notificationChannel.setSound(uri, audioAttributes);

            notificationManager.createNotificationChannel(notificationChannel);
            builder = new NotificationCompat.Builder(AGlobals.contextApp, notificationChannel.getId());
        } else
        {
            builder = new NotificationCompat.Builder(AGlobals.contextApp);
        }
        //   Intent intent = new Intent(instance, NotificationReceiver.class);
        //   PendingIntent pIntent = PendingIntent.getActivity(instance, 0, intent, 0);

        builder //.setSmallIcon(R.drawable.connect_to_machine)
                .setContentText(text)
                // .setDefaults(DEFAULT_VIBRATE | FLAG_SHOW_LIGHTS)
                .setPriority(NotificationCompat.PRIORITY_HIGH)
                .setVisibility(NotificationCompat.VISIBILITY_PUBLIC)
                .setCategory(Notification.CATEGORY_STATUS)
                .setSound(uri)
                .setAutoCancel(true);

        Notification notification = builder.build();
//                notification.defaults = Notification.DEFAULT_SOUND |
        //                      Notification.DEFAULT_VIBRATE;


        notificationManager.notify(0, notification);
        AGlobals.rootHandler.Send(new Runnable() {
            @Override
            public void run()
            {
                NotificationManager notifManager = (NotificationManager) (NotificationManager) AGlobals.contextApp.getSystemService(Context.NOTIFICATION_SERVICE);
                notifManager.cancelAll();
            }
        }, 1000);


    }

    static public void PlayVibrate()
    {
        Vibrator vibrator = (Vibrator) AGlobals.contextApp.getSystemService(Context.VIBRATOR_SERVICE);

        vibrator.vibrate(2000);
        // vibrator.cancel();
    }

    static public void Play(String text)
    {
        String rs = AGlobals.ReadIni("ANDROID_SOUND", "SOUND", null);
        if (rs == null || rs.equals("YES"))
            PlayAudio(text);

        rs = AGlobals.ReadIni("ANDROID_SOUND", "VIBRATE", null);
        if (rs == null || rs.equals("YES"))
            PlayVibrate();

    }

}
