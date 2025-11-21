# Touch Game - Deployment Guide

## APK Location
**Signed Release APK (Ready for Physical Phones):**
```
TouchGame-v1.0-signed.apk
```
Size: ~2.9 MB

This APK is **signed and ready** to install on any physical Android device.

## Installation on Physical Phone

### Option 1: Direct Installation (Easiest)
1. Copy `TouchGame-v1.0-signed.apk` to your phone (via USB, email, cloud storage, etc.)
2. On your phone, go to **Settings → Security**
3. Enable **"Install from Unknown Sources"** or **"Install Unknown Apps"**
4. Use a file manager to locate the APK file
5. Tap the APK file and follow the installation prompts

### Option 2: ADB Installation (Developer Mode)
1. Enable **Developer Options** on your phone:
   - Go to Settings → About Phone
   - Tap "Build Number" 7 times
2. Enable **USB Debugging**:
   - Go to Settings → Developer Options
   - Enable "USB Debugging"
3. Connect your phone to computer via USB
4. Run the installation command:
```bash
adb install -r TouchGame-v1.0-signed.apk
```

### Option 3: Install via Android Studio
1. Connect your phone via USB
2. In Android Studio, select your device from the device dropdown
3. Click Run (Green play button)

## Current Configuration

### Ad Unit IDs (Currently Test IDs)
**IMPORTANT**: Replace these with your real AdMob IDs before publishing!

In `AndroidManifest.xml`:
- App ID: `ca-app-pub-3940256099942544~3347511713`

In `GameActivity.java`:
- Interstitial Ad: `ca-app-pub-3940256099942544/1033173712`
- Banner Ad: `ca-app-pub-3940256099942544/6300978111`

### To Get Real Ad IDs:
1. Sign up at https://admob.google.com/
2. Create a new app in AdMob console
3. Create ad units (Interstitial and Banner)
4. Replace the test IDs with your real IDs
5. Rebuild the app

## App Features
- Moving circle game with progressive difficulty
- Random gradient backgrounds that change each level
- Interstitial ads (show randomly every 5-60 seconds)
- Banner ad at bottom of screen
- Score tracking with bonus points per round
- Circle shrinks 5% and speed increases 20% each round

## Build Variants
- **Debug**: `./gradlew assembleDebug` - For testing
- **Release**: `./gradlew assembleRelease` - For production (optimized, minified)

## Code Protection
The release build includes:
- ProGuard obfuscation (enabled)
- Resource shrinking (enabled)
- Code minification (enabled)

## System Requirements
- Android 7.0 (API 24) or higher
- ~3 MB storage space
- Internet connection for ads

## Permissions
- `INTERNET` - Required for ads
- `ACCESS_NETWORK_STATE` - Required for ads

## Notes
- The current APK is **unsigned** (debug signed only)
- For Google Play Store publishing, you'll need to create a proper signing key
- Test ads show "Test Ad" watermark - real ads will appear with your AdMob account

## Creating a Signed APK for Google Play Store

### Step 1: Generate a Keystore
```bash
keytool -genkey -v -keystore release-key.jks -keyalg RSA -keysize 2048 -validity 10000 -alias my-app-key
```

### Step 2: Configure signing in build.gradle
Add your keystore details to `app/build.gradle`:
```gradle
signingConfigs {
    release {
        storeFile file("path/to/release-key.jks")
        storePassword "your-password"
        keyAlias "my-app-key"
        keyPassword "your-key-password"
    }
}
```

### Step 3: Build signed APK
```bash
./gradlew assembleRelease
```

## Support
For issues or questions, check the code in:
- Main game: `app/src/main/cpp/game.cpp`
- UI: `app/src/main/java/com/example/touchgame/GameActivity.java`
- Native bridge: `app/src/main/cpp/native-lib.cpp`
