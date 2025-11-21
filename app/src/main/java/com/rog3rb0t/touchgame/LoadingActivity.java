package com.rog3rb0t.touchgame;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.animation.Animation;
import android.view.animation.TranslateAnimation;
import android.widget.ImageView;

public class LoadingActivity extends Activity {
    private static final int LOADING_DURATION = 2500; // 2.5 seconds

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_loading);

        ImageView loadingImage = findViewById(R.id.loadingImage);
        
        // Animate the ball moving out of the phone
        TranslateAnimation animation = new TranslateAnimation(
                0, 100,  // X: from 0 to 100 pixels right
                0, 0     // Y: no vertical movement
        );
        animation.setDuration(1500);
        animation.setRepeatCount(Animation.INFINITE);
        animation.setRepeatMode(Animation.REVERSE);
        loadingImage.startAnimation(animation);

        // Transition to game activity after loading duration
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                Intent intent = new Intent(LoadingActivity.this, GameActivity.class);
                startActivity(intent);
                finish();
            }
        }, LOADING_DURATION);
    }
}
