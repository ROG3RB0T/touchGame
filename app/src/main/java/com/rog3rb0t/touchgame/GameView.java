package com.rog3rb0t.touchgame;

import android.content.Context;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.MotionEvent;
import android.view.HapticFeedbackConstants;
import android.widget.Toast;
import android.util.Log;

public class GameView extends SurfaceView implements SurfaceHolder.Callback, Runnable {
    private static final String TAG = "GameView";
    private Thread renderThread;
    private volatile boolean running = false;
    private SurfaceHolder holder;
    private GameActivity activity;

    static {
        try {
            System.loadLibrary("touchgame");
            Log.i(TAG, "Native library loaded successfully");
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Failed to load native library", e);
        }
    }

    public GameView(Context context, GameActivity activity) {
        super(context);
        this.activity = activity;
        holder = getHolder();
        holder.addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.i(TAG, "Surface created");
        running = true;
        renderThread = new Thread(this);
        renderThread.start();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        running = false;
        try {
            if (renderThread != null) {
                renderThread.join();
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        nativeDestroy();
    }

    @Override
    public void run() {
        // Initialize OpenGL on the render thread
        try {
            Log.i(TAG, "Calling nativeInit");
            nativeInit(holder.getSurface());
            Log.i(TAG, "nativeInit completed");
            Thread.sleep(100);
        } catch (Exception e) {
            Log.e(TAG, "Error in nativeInit", e);
            e.printStackTrace();
            return;
        }
        
        while (running) {
            try {
                nativeRender();
                
                // Update UI on main thread
                activity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        activity.updateScore(nativeGetScore(), nativeGetRound());
                        
                        if (nativeIsGameOver()) {
                            activity.showGameOver();
                            running = false;
                        }
                    }
                });
                
                Thread.sleep(16); // ~60 FPS
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (event.getAction() == MotionEvent.ACTION_DOWN) {
            Log.i(TAG, "Touch event: " + event.getX() + ", " + event.getY());
            
            // Provide immediate haptic feedback for all touches
            performHapticFeedback(HapticFeedbackConstants.VIRTUAL_KEY);
            
            boolean result = nativeTouch(event.getX(), event.getY());
            Log.i(TAG, "Touch result: " + result);
            
            // Stronger haptic feedback for successful hits
            if (result) {
                performHapticFeedback(HapticFeedbackConstants.LONG_PRESS);
            }
        }
        return true;
    }

    public void resetGame() {
        nativeReset();
    }
    
    // Called from native code to show toast messages
    public void showToast(final String message) {
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(getContext(), message, Toast.LENGTH_SHORT).show();
            }
        });
    }

    // Native methods
    private native void nativeInit(Surface surface);
    private native void nativeRender();
    private native boolean nativeTouch(float x, float y);
    private native int nativeGetScore();
    private native int nativeGetRound();
    private native boolean nativeIsGameOver();
    private native void nativeDestroy();
    private native void nativeReset();
    private native float nativeGetBgColorR1();
    private native float nativeGetBgColorG1();
    private native float nativeGetBgColorB1();
    private native float nativeGetBgColorR2();
    private native float nativeGetBgColorG2();
    private native float nativeGetBgColorB2();
    
    public int[] getBackgroundColors() {
        int color1 = android.graphics.Color.rgb(
            (int)(nativeGetBgColorR1() * 255),
            (int)(nativeGetBgColorG1() * 255),
            (int)(nativeGetBgColorB1() * 255)
        );
        int color2 = android.graphics.Color.rgb(
            (int)(nativeGetBgColorR2() * 255),
            (int)(nativeGetBgColorG2() * 255),
            (int)(nativeGetBgColorB2() * 255)
        );
        return new int[]{color1, color2};
    }
}
