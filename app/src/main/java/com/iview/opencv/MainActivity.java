package com.iview.opencv;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.os.AsyncTask;
import android.os.Bundle;
import android.widget.ImageView;
import android.widget.Toast;

import com.iview.opencv.util.ImageUtils;
import com.iview.opencv.util.OpenCVHelper;
import com.orhanobut.logger.Logger;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;
import butterknife.Unbinder;
import view.DialogBottom;

public class MainActivity extends Activity {
    private Unbinder unbinder;
    private DialogBottom bottom;
    @BindView(R.id.iv_picture_before)
    protected ImageView ivBefore;
    @BindView(R.id.iv_picture_after)
    protected ImageView ivAfter;
    private Bitmap bitmap;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        unbinder = ButterKnife.bind(this);
        Logger.init("Activity");
    }

    @OnClick(R.id.bt_picture)
    public void clickPicture() {
        Logger.d("clickCapture");
        if (bottom == null) {
            bottom = new DialogBottom(this);
            bottom.setOnClickListener(new DialogBottom.ClickListener() {
                @Override
                public void onAction() {
                    ImageUtils.getImageForCaptrue(MainActivity.this, ImageUtils.IMAGE_FROM_CAPTRUE);
                }

                @Override
                public void onNative() {
                    ImageUtils.getImageFromAlbum(MainActivity.this, ImageUtils.IMAGE_FROM_ALBUM);
                }
            });
        }
        bottom.show();
    }

    @OnClick(R.id.bt_video)
    public void clickVideo() {
        Logger.d("clickVideo");
        DialogBottom dlVideo = new DialogBottom(this);
        dlVideo.setTvCapture("录制");
        dlVideo.setTvAlbum("本地");
        dlVideo.setOnClickListener(new DialogBottom.ClickListener() {
            @Override
            public void onAction() {

            }

            @Override
            public void onNative() {

            }
        });

        dlVideo.show();

    }

    @OnClick(R.id.bt_count)
    public void clickCount() {
        Logger.d("clickCount");
        if (bitmap != null) {
            new AsyncTask<String, Integer, Bitmap>() {
                @Override
                protected Bitmap doInBackground(String... strings) {
                    Bitmap map = ImageUtils.comp(bitmap);
                    int w = map.getWidth();
                    int h = map.getHeight();
                    return doTestTask(map, w, h);
                }

                @Override
                protected void onPostExecute(Bitmap bitmap) {

                    if (null != bitmap) {
                        ivAfter.setImageBitmap(bitmap);
                        Logger.e("width-------->" + bitmap.getWidth() + "height----->" + bitmap.getHeight());
                    } else {
                        Logger.e("bitmap-------->" + bitmap);
                    }
                }
            }.execute();
        } else {
            Toast.makeText(this, "请先选择图像", Toast.LENGTH_SHORT).show();
        }

    }

    private Bitmap doTestTask(Bitmap map, int w, int h) {
        return new OpenCVHelper().detectPalmFromRawData(map, w, h, "");
    }

    private Bitmap doGray(Bitmap map, int w, int h) {
        int[] pix = new int[w * h];
        map.getPixels(pix, 0, w, 0, 0, w, h);
        int[] resultPixes = OpenCVHelper.gray(pix, w, h);
        Bitmap result = Bitmap.createBitmap(w, h, Bitmap.Config.RGB_565);
        result.setPixels(resultPixes, 0, w, 0, 0, w, h);
        return result;
    }

    private Bitmap getImage(Bitmap map, int w, int h) {
        int[] pix = new int[w * h];
        map.getPixels(pix, 0, w, 0, 0, w, h);
        int[] a = OpenCVHelper.getDealBitmap(pix, w, h);
        Bitmap b = Bitmap.createBitmap(w, h, Bitmap.Config.RGB_565);
        b.setPixels(a, 0, w, 0, 0, w, h);
        return b;
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unbinder.unbind();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == ImageUtils.IMAGE_FROM_CAPTRUE || requestCode == ImageUtils.IMAGE_FROM_ALBUM) {
            bitmap = ImageUtils.getBitMapByUri(this, data);
            if (bitmap != null) {
                ivBefore.setImageBitmap(bitmap);
                bottom.cancel();
            } else {
                Logger.d("bitmap------>null");
            }
        }

    }

    @Override
    public void finish() {
        super.finish();
        if (bottom != null) {
            bottom.cancel();
        }
    }
}
