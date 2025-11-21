package com.rog3rb0t.touchgame;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.LinearLayout;
import android.widget.Button;
import android.view.Gravity;
import android.view.View;
import android.view.WindowManager;
import android.graphics.drawable.GradientDrawable;
import android.graphics.Color;
import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.interstitial.InterstitialAd;
import com.google.android.gms.ads.interstitial.InterstitialAdLoadCallback;
import com.google.android.gms.ads.AdError;
import com.google.android.gms.ads.FullScreenContentCallback;
import com.google.android.gms.ads.LoadAdError;
import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.RequestConfiguration;
import androidx.annotation.NonNull;
import java.util.Arrays;

public class GameActivity extends Activity {
    private GameView gameView;
    private TextView scoreText;
    private TextView roundText;
    private LinearLayout layout;
    private InterstitialAd interstitialAd;
    private AdView bannerAdView;
    private int lastAdRound = 0; // Track the last round an ad was shown

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Keep screen on during gameplay
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        // Create layout programmatically
        layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        
        // Score and round display
        LinearLayout infoLayout = new LinearLayout(this);
        infoLayout.setOrientation(LinearLayout.HORIZONTAL);
        infoLayout.setGravity(Gravity.CENTER);
        infoLayout.setPadding(20, 20, 20, 20);
        
        scoreText = new TextView(this);
        scoreText.setText("Score: 0");
        scoreText.setTextSize(20);
        scoreText.setPadding(20, 0, 20, 0);
        
        roundText = new TextView(this);
        roundText.setText("Round: 1");
        roundText.setTextSize(20);
        roundText.setPadding(20, 0, 20, 0);
        
        infoLayout.addView(scoreText);
        infoLayout.addView(roundText);
        
        // Game view
        gameView = new GameView(this, this);
        LinearLayout.LayoutParams gameParams = new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.MATCH_PARENT,
            0,
            1.0f
        );
        gameView.setLayoutParams(gameParams);
        
        // Banner ad at the bottom
        bannerAdView = new AdView(this);
        bannerAdView.setAdSize(AdSize.BANNER);
        // Banner ad unit ID from BuildConfig
        bannerAdView.setAdUnitId(BuildConfig.ADMOB_BANNER_AD_UNIT_ID);
        LinearLayout.LayoutParams adParams = new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.MATCH_PARENT,
            LinearLayout.LayoutParams.WRAP_CONTENT
        );
        bannerAdView.setLayoutParams(adParams);
        
        layout.addView(infoLayout);
        layout.addView(gameView);
        layout.addView(bannerAdView);
        
        setContentView(layout);
        
        // Initialize Mobile Ads SDK
        MobileAds.initialize(this, initializationStatus -> {
            loadInterstitialAd();
            loadBannerAd();
        });
        
        // Set initial gradient after a short delay to ensure native side is ready
        gameView.postDelayed(new Runnable() {
            @Override
            public void run() {
                updateGradient();
            }
        }, 200);
        
        // Show game instructions for 10 seconds
        final Toast toast = Toast.makeText(this, 
            "Tap the moving circle to score points!\n" +
            "Score = 10 + round number\n" +
            "Circle gets smaller and faster each round!", 
            Toast.LENGTH_LONG);
        toast.setGravity(Gravity.TOP | Gravity.CENTER_HORIZONTAL, 0, 120);
        toast.show();
        
        // Keep showing the toast for 10 seconds
        new Thread(new Runnable() {
            @Override
            public void run() {
                for (int i = 0; i < 3; i++) {
                    try {
                        Thread.sleep(3500);
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                toast.show();
                            }
                        });
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        }).start();
    }
    
    private void updateGradient() {
        try {
            int[] colors = gameView.getBackgroundColors();
            GradientDrawable gradient = new GradientDrawable(
                GradientDrawable.Orientation.TOP_BOTTOM,
                colors
            );
            layout.setBackground(gradient);
            
            // Calculate average luminance of both gradient colors
            float lum1 = (0.299f * Color.red(colors[0]) + 
                         0.587f * Color.green(colors[0]) + 
                         0.114f * Color.blue(colors[0])) / 255.0f;
            float lum2 = (0.299f * Color.red(colors[1]) + 
                         0.587f * Color.green(colors[1]) + 
                         0.114f * Color.blue(colors[1])) / 255.0f;
            float avgLuminance = (lum1 + lum2) / 2.0f;
            
            // Set text color based on background brightness
            int textColor = avgLuminance > 0.5f ? Color.BLACK : Color.WHITE;
            scoreText.setTextColor(textColor);
            roundText.setTextColor(textColor);
            
            // Add shadow for better readability
            if (avgLuminance > 0.5f) {
                scoreText.setShadowLayer(2, 1, 1, Color.WHITE);
                roundText.setShadowLayer(2, 1, 1, Color.WHITE);
            } else {
                scoreText.setShadowLayer(2, 1, 1, Color.BLACK);
                roundText.setShadowLayer(2, 1, 1, Color.BLACK);
            }
        } catch (Exception e) {
            // Fallback to white if native side not ready
            layout.setBackgroundColor(0xFFFFFFFF);
            scoreText.setTextColor(Color.BLACK);
            roundText.setTextColor(Color.BLACK);
        }
    }

    public void updateScore(int score, int round) {
        scoreText.setText("Score: " + score);
        roundText.setText("Round: " + round);
        updateGradient();
        
        // Show interstitial ad every 5 levels
        if (round > 1 && round % 5 == 0 && round != lastAdRound) {
            showInterstitialAd();
            lastAdRound = round;
        }
    }
    
    private void loadInterstitialAd() {
        // Interstitial ad unit ID from BuildConfig
        String adUnitId = BuildConfig.ADMOB_INTERSTITIAL_AD_UNIT_ID;
        
        AdRequest adRequest = new AdRequest.Builder().build();
        
        InterstitialAd.load(this, adUnitId, adRequest, new InterstitialAdLoadCallback() {
            @Override
            public void onAdLoaded(@NonNull InterstitialAd ad) {
                interstitialAd = ad;
                
                // Set callback for ad events
                interstitialAd.setFullScreenContentCallback(new FullScreenContentCallback() {
                    @Override
                    public void onAdDismissedFullScreenContent() {
                        // Ad dismissed, load next ad
                        interstitialAd = null;
                        loadInterstitialAd();
                    }
                    
                    @Override
                    public void onAdFailedToShowFullScreenContent(@NonNull AdError adError) {
                        // Ad failed to show, continue game
                        interstitialAd = null;
                        loadInterstitialAd();
                    }
                    
                    @Override
                    public void onAdShowedFullScreenContent() {
                        // Ad is showing
                        interstitialAd = null;
                    }
                    
                    @Override
                    public void onAdClicked() {
                        // User clicked the ad
                    }
                });
            }
            
            @Override
            public void onAdFailedToLoad(@NonNull LoadAdError loadAdError) {
                interstitialAd = null;
                // Retry loading after a delay
                layout.postDelayed(() -> loadInterstitialAd(), 5000);
            }
        });
    }
    
    private void showInterstitialAd() {
        if (interstitialAd != null) {
            runOnUiThread(() -> {
                try {
                    interstitialAd.show(this);
                } catch (Exception e) {
                    // If ad fails to show, just continue the game
                    interstitialAd = null;
                    loadInterstitialAd();
                }
            });
        }
        // If ad is not ready, just skip it and continue the game
    }
    
    private void loadBannerAd() {
        // Add test device configuration for testing
        RequestConfiguration requestConfiguration = new RequestConfiguration.Builder()
            .setTestDeviceIds(Arrays.asList(AdRequest.DEVICE_ID_EMULATOR))
            .build();
        MobileAds.setRequestConfiguration(requestConfiguration);
        
        AdRequest adRequest = new AdRequest.Builder().build();
        bannerAdView.loadAd(adRequest);
        
        // Add listener to debug ad loading
        bannerAdView.setAdListener(new AdListener() {
            @Override
            public void onAdLoaded() {
                android.util.Log.d("TouchGame", "Banner ad loaded successfully");
            }
            
            @Override
            public void onAdFailedToLoad(@NonNull LoadAdError loadAdError) {
                android.util.Log.e("TouchGame", "Banner ad failed to load: " + 
                    loadAdError.getMessage() + " (Code: " + loadAdError.getCode() + ")");
            }
        });
    }

    public void showGameOver() {
        Toast toast = Toast.makeText(this, "Game Over! You reached 25 points!", Toast.LENGTH_LONG);
        toast.setGravity(Gravity.TOP | Gravity.CENTER_HORIZONTAL, 0, 120);
        toast.show();
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (bannerAdView != null) {
            bannerAdView.pause();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (bannerAdView != null) {
            bannerAdView.resume();
        }
    }
    
    @Override
    protected void onDestroy() {
        if (bannerAdView != null) {
            bannerAdView.destroy();
        }
        super.onDestroy();
    }
}
