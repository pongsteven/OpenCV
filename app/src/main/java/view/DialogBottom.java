package view;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.iview.opencv.R;
import com.orhanobut.logger.Logger;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;
import butterknife.Unbinder;

/**
 * Created by pengqin on 2016/8/15.
 */
public class DialogBottom extends Dialog {
    @BindView(R.id.ll_dialog_bottom_root)
    protected LinearLayout root;
    private int mHeight;
    private Unbinder unbinder;
    private ClickListener listener;
    @BindView(R.id.tv_capture)
    protected TextView tvCapture;
    @BindView(R.id.tv_album)
    protected TextView tvAlbum;

    public DialogBottom(Context context) {
        super(context);
        init();
    }

    public DialogBottom(Context context, int themeResId) {
        super(context, themeResId);
    }

    protected DialogBottom(final Context context, boolean cancelable, OnCancelListener cancelListener) {
        super(context, cancelable, cancelListener);

    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        root.getViewTreeObserver().addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener() {
            @Override
            public void onGlobalLayout() {
                Logger.d("onGlobalLayout()");
                FrameLayout.LayoutParams lp = (FrameLayout.LayoutParams) root.getLayoutParams();
                if (lp.height > mHeight / 3) {
                    lp.height = mHeight / 3;
                    root.setLayoutParams(lp);
                    root.invalidate();
                    Logger.d("Invalidate()");
                }
            }
        });

    }

    private void init() {
        setContentView(R.layout.diaolg_bottom_view);
        unbinder = ButterKnife.bind(this);
        Logger.init("DialogBottom");

        Window window = getWindow();
        window.setWindowAnimations(R.style.clickDialogStyle);

        WindowManager.LayoutParams lp = window.getAttributes();
        lp.width = WindowManager.LayoutParams.MATCH_PARENT;
        lp.height = WindowManager.LayoutParams.WRAP_CONTENT;
        lp.gravity = Gravity.BOTTOM;
        window.setAttributes(lp);
        this.setCanceledOnTouchOutside(true);

        WindowManager win = (WindowManager) getContext().getSystemService(Context.WINDOW_SERVICE);
        mHeight = win.getDefaultDisplay().getHeight();

    }

    @OnClick(R.id.tv_capture)
    protected void clickCapture() {
        Logger.d("clickCapture");
        if (listener != null) {
            listener.onAction();
        }
    }

    @OnClick(R.id.tv_album)
    protected void clickAlbum() {
        Logger.d("clickAlbum");
        if (listener != null) {
            listener.onNative();
        }
    }

    @OnClick(R.id.tv_cancel)
    protected void clickCancel() {
        Logger.d("clickCancel");
        this.cancel();
    }

    public void setTvCapture(String text) {
        tvCapture.setText(text);
    }

    public void setTvAlbum(String text) {
        tvAlbum.setText(text);
    }

    @Override
    protected void onStop() {
        super.onStop();
       // unbinder.unbind();
    }

    public void setOnClickListener(ClickListener listener) {
        this.listener = listener;

    }

    public interface ClickListener {

        void onAction();

        void onNative();
    }
}
