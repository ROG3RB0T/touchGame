# Add project specific ProGuard rules here.

# Keep Google Mobile Ads SDK
-keep class com.google.android.gms.ads.** { *; }
-dontwarn com.google.android.gms.ads.**

# Keep native methods
-keepclasseswithmembernames class * {
    native <methods>;
}

# Keep AdMob classes
-keep public class com.google.android.gms.ads.**{
   public *;
}

# Keep game activity and view
-keep class com.rog3rb0t.touchgame.GameActivity { *; }
-keep class com.rog3rb0t.touchgame.GameView { *; }
-keep class com.rog3rb0t.touchgame.LoadingActivity { *; }

# Keep annotations
-keepattributes *Annotation*
-keepattributes Signature
-keepattributes InnerClasses

