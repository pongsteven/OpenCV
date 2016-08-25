package com.iview.opencv.util;

import android.graphics.Bitmap;

/**
 * Created by pengqin on 2016/8/15.
 */
public class OpenCVHelper {
    static {
        System.loadLibrary("OpenCV");
    }

    public native Bitmap detectPalmFromRawData(Bitmap bitmap, int w, int h, String path);

    public static native int[] gray(int[] buf, int w, int h);

    public static native int[] getDealBitmap(int[] buf, int imgHeight, int imgWidth);


}
