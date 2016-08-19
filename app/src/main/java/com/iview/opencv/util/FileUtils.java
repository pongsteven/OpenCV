package com.iview.opencv.util;

import android.net.Uri;
import android.os.Environment;

import com.orhanobut.logger.Logger;

import java.io.File;
import java.io.IOException;
import java.util.Random;

/**
 * Created by pengqin on 2016/8/15.
 */
public class FileUtils {

    public static Uri getStoreImageUri() {
        String fileName = new Random().nextInt() + "";
        if (Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState())) {
            File file = new File(Environment.getDataDirectory() + fileName);
            if (!file.exists()) {
                try {
                    file.mkdir();
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


}
