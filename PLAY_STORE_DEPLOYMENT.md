# Play Store Deployment Guide

## Files Ready for Upload

### App Bundle (Recommended for Play Store)
**Location:** `app/build/outputs/bundle/release/app-release.aab`
- This is the Android App Bundle (AAB) format required by Google Play Store
- File size: ~2-3 MB
- Signed with your release keystore

### APK (Alternative/Testing)
**Location:** `app/build/outputs/apk/release/app-release.apk`
- Traditional APK format
- Can be used for direct distribution or testing
- Signed with your release keystore

## Build Commands

```bash
# Build App Bundle (for Play Store)
./gradlew bundleRelease

# Build APK (for testing)
./gradlew assembleRelease
```

## Current App Configuration

### Package Information
- **Package Name:** `com.rog3rb0t.touchgame`
- **App Name:** Touch Game
- **Version Code:** 1
- **Version Name:** 1.0

### Keystore Details
- **Location:** `~/touchgame-release-key.jks`
- **Alias:** touchgame
- **Password:** android123
- **Key Password:** android123

⚠️ **IMPORTANT:** Keep your keystore file safe! If you lose it, you cannot update your app.

## AdMob Configuration

### Current Ad Unit IDs
- **Banner Ad:** `ca-app-pub-3144400218741849/8293189496`
- **Interstitial Ad:** `ca-app-pub-3144400218741849/3790288967`

### AdMob Application ID
- Set in `AndroidManifest.xml`: `ca-app-pub-3940256099942544~3347511713` (TEST ID - needs update)

⚠️ **TODO:** Update the AdMob Application ID in AndroidManifest.xml with your real app ID from AdMob console.

## Pre-Deployment Checklist

### 1. Update Version for Each Release
Edit `app/build.gradle`:
```gradle
versionCode 1        // Increment for each release (1, 2, 3...)
versionName "1.0"    // Update as needed (1.0, 1.1, 2.0...)
```

### 2. Verify AdMob Application ID
Edit `app/src/main/AndroidManifest.xml` and replace test ID:
```xml
<meta-data
    android:name="com.google.android.gms.ads.APPLICATION_ID"
    android:value="YOUR_REAL_ADMOB_APP_ID"/>
```

### 3. Review ProGuard Rules
✅ Already configured in `app/proguard-rules.pro`
- Native methods preserved
- AdMob classes kept
- Game classes protected

### 4. Test the Release Build
```bash
# Install on device
adb install app/build/outputs/apk/release/app-release.apk

# Test all features:
- Touch interaction works
- Ads load (may take time for new apps)
- Score/level progression
- Multiple circles after level 11
- Explosion effects
- Reset button
- Exit button
```

### 5. Required Play Store Assets

#### Screenshots (Required)
- **Phone:** Minimum 2 screenshots (320x480 to 3840x2160)
- **7-inch Tablet:** Minimum 2 screenshots
- **10-inch Tablet:** Minimum 2 screenshots

#### Graphics (Required)
- **App Icon:** 512x512 PNG (32-bit)
- **Feature Graphic:** 1024x500 JPG or PNG

#### Optional
- **Promo Video:** YouTube URL
- **TV Banner:** 1280x720 PNG

### 6. Store Listing Information

Prepare the following:

**Title:**
```
Touch Game - Fast Reaction Challenge
```

**Short Description (80 chars max):**
```
Tap the moving circle! Test your speed and reflexes in this addictive game.
```

**Full Description (4000 chars max):**
```
🎯 Touch Game - The Ultimate Reaction Test!

Challenge yourself with this fast-paced, addictive tap game! A colorful circle bounces around the screen - can you catch it?

FEATURES:
✨ Beautiful gradient backgrounds that change each round
🎯 Progressive difficulty - circles get smaller and faster
💥 Explosive effects when you miss (after level 11)
🎮 Multiple decoy circles in higher levels
📊 Score tracking and level progression
🎨 Smooth animations and particle effects

GAMEPLAY:
- Tap the moving circle to score points
- Each level increases speed by 20%
- Circle size decreases by 5% per round
- From level 11: Face multiple circles - only one is real!
- Tap decoys and watch them explode
- Clear all decoys to win the round

Perfect for:
- Quick gaming sessions
- Improving hand-eye coordination
- Competing with friends for high scores
- Killing time during breaks

How far can you go? Download now and test your reflexes!
```

**Category:**
- Primary: Games > Casual or Games > Arcade

**Content Rating:**
- Fill out content rating questionnaire (likely rated "Everyone")

**Privacy Policy:**
- You may need to create a privacy policy URL if collecting any user data
- Required if using AdMob

### 7. Upload to Play Console

1. Go to [Google Play Console](https://play.google.com/console)
2. Create a new application
3. Fill in store listing details
4. Upload screenshots and graphics
5. Complete content rating questionnaire
6. Set up pricing (Free with ads)
7. Upload the AAB file (`app-release.aab`)
8. Submit for review

### 8. Post-Deployment

**Monitor:**
- Crash reports in Play Console
- AdMob earnings and performance
- User reviews and ratings
- Download statistics

**Updates:**
- Increment `versionCode` and `versionName` for each update
- Build new AAB with `./gradlew bundleRelease`
- Upload to Play Console

## Common Issues & Solutions

### Issue: App rejected for missing privacy policy
**Solution:** Create a simple privacy policy page and add URL to Play Console

### Issue: Ads not showing
**Solution:** 
- Verify AdMob account is approved (takes 24-48 hours)
- Check ad unit IDs are correct
- Allow 24 hours for ad serving to activate

### Issue: Can't update app (signature mismatch)
**Solution:** 
- Always use the same keystore file for updates
- Never lose your keystore!

### Issue: App crashes on some devices
**Solution:**
- Check Play Console crash reports
- Test on multiple Android versions
- Review ProGuard rules if obfuscation causes issues

## Support & Maintenance

**Keystore Backup:**
```bash
# Create backup
cp ~/touchgame-release-key.jks ~/touchgame-release-key-backup.jks

# Store in multiple secure locations:
- External hard drive
- Cloud storage (encrypted)
- USB drive in safe place
```

**Build History:**
Keep track of releases:
- Version 1.0 (Build 1) - Initial release
- Document changes for each version

## Quick Reference

```bash
# Clean build
./gradlew clean

# Build release AAB
./gradlew bundleRelease

# Build release APK  
./gradlew assembleRelease

# List signing configs
./gradlew signingReport

# Check for lint issues
./gradlew lintRelease
```

## Next Steps

1. ✅ Update AdMob Application ID in AndroidManifest.xml
2. ✅ Create screenshots of your app
3. ✅ Design app icon and feature graphic
4. ✅ Write privacy policy if needed
5. ✅ Create Play Store account ($25 one-time fee)
6. ✅ Upload app-release.aab to Play Console
7. ✅ Complete store listing
8. ✅ Submit for review
9. ✅ Wait for approval (typically 1-3 days)
10. ✅ Publish!

---

**Your app is ready for deployment! Good luck! 🚀**
