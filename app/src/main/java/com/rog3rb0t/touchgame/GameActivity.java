package com.rog3rb0t.touchgame;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.LinearLayout;
import android.widget.Button;
import android.view.Gravity;
import android.view.View;
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
import androidx.annotation.NonNull;
import java.util.Random;

public class GameActivity extends Activity {
    private GameView gameView;
    private TextView scoreText;
    private TextView roundText;
    private Button resetButton;
    private Button exitButton;
    private LinearLayout layout;
    private InterstitialAd interstitialAd;
    private AdView bannerAdView;
    private Random random = new Random();
    private boolean firstLevelCompleted = false;
    private long lastAdTime = 0;
    private long nextAdDelay = 0; // milliseconds until next ad

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

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
        
        // Reset button at bottom
        resetButton = new Button(this);
        resetButton.setText("🔄 Reset Game");
        resetButton.setTextColor(0xFFFFFFFF);
        resetButton.setTextSize(16);
        resetButton.setBackgroundResource(R.drawable.button_reset);
        resetButton.setAllCaps(false);
        resetButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                gameView.resetGame();
                updateScore(0, 1);
                updateGradient();
            }
        });
        LinearLayout.LayoutParams buttonParams = new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.MATCH_PARENT,
            LinearLayout.LayoutParams.WRAP_CONTENT
        );
        buttonParams.setMargins(20, 10, 20, 10);
        resetButton.setLayoutParams(buttonParams);
        
        // Exit button
        exitButton = new Button(this);
        exitButton.setText("✖ Exit");
        exitButton.setTextColor(0xFFFFFFFF);
        exitButton.setTextSize(16);
        exitButton.setBackgroundResource(R.drawable.button_exit);
        exitButton.setAllCaps(false);
        exitButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });
        LinearLayout.LayoutParams exitParams = new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.MATCH_PARENT,
            LinearLayout.LayoutParams.WRAP_CONTENT
        );
        exitParams.setMargins(20, 0, 20, 20);
        exitButton.setLayoutParams(exitParams);
        
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
        layout.addView(resetButton);
        layout.addView(exitButton);
        layout.addView(bannerAdView);
        
        setContentView(layout);
        
        // Initialize Mobile Ads SDK
        MobileAds.initialize(this, initializationStatus -> {
            loadInterstitialAd();
            loadBannerAd();
        });
        
        // Set initial random ad delay (10-60 seconds in milliseconds)
        nextAdDelay = (10 + random.nextInt(51)) * 1000L; // 10-60 seconds
        lastAdTime = System.currentTimeMillis();
        
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
        
        // After completing first level, enable time-based ads
        if (round > 1) {
            firstLevelCompleted = true;
        }
        
        // Check if enough time has passed since last ad
        if (firstLevelCompleted) {
            long currentTime = System.currentTimeMillis();
            long timeSinceLastAd = currentTime - lastAdTime;
            
            if (timeSinceLastAd >= nextAdDelay) {
                showInterstitialAd();
                lastAdTime = currentTime;
                // Set next random delay (10-60 seconds)
                nextAdDelay = (10 + random.nextInt(51)) * 1000L;
            }
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
        AdRequest adRequest = new AdRequest.Builder().build();
        bannerAdView.loadAd(adRequest);
    }

    public void showGameOver() {
        Toast.makeText(this, "Game Over! You reached 25 points!", Toast.LENGTH_LONG).show();
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
