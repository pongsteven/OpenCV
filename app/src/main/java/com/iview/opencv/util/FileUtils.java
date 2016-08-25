package com.iview.opencv.util;

import android.net.Uri;
import android.os.Environment;

import com.orhanobut.logger.Logger;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * Created by pengqin on 2016/8/15.
 */
public class FileUtils {

    public static Uri getStoreImageUri() {
        if (Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState())) {

            File file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/picture", getFileName());
            if (!file.exists()) {
                try {
                    file.createNewFile();
                    Logger.d("path----->" + file.getAbsolutePath() + ",name------->" + file.getName());
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
            return Uri.fromFile(file);
        } else {
            Logger.d("SD卡不存在");
            return null;
        }
    }

    private static String getFileName() {
        Date data = new Date(System.currentTimeMillis());
        SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd");
        return format.format(data);
    }

}
