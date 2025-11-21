# Touch Game

An engaging Android game built with C++ and OpenGL ES featuring stunning 3D graphics, smooth touch detection, and progressive difficulty. Test your reflexes as you tap increasingly fast and numerous circles!

## 🎮 Game Description

Touch Game is a fast-paced reflex game where players must tap moving 3D spheres on the screen. Each level increases in difficulty with more circles, faster movement, and smaller targets.

### Key Features

- **Ultra-smooth 3D graphics** - Realistic sphere rendering with dynamic lighting, specular highlights, and glossy effects
- **Progressive difficulty** - Circles increase in number, speed, and decrease in size with each level
- **Enhanced touch detection** - 50% larger touch radius for better responsiveness
- **Visual feedback** - Particles explosion effects and flash animations on touch
- **Haptic feedback** - Vibration response for all touches
- **Dynamic backgrounds** - Randomly generated gradient backgrounds for each level
- **AdMob integration** - Banner and interstitial ads with configurable timing

### Gameplay Mechanics

- **Objective**: Clear all circles on screen to advance to the next level
- **Scoring**: 10 points per circle touched
- **Level progression**:
  - **Levels 1-10**: Number of circles equals level number (1 circle in level 1, 2 in level 2, etc.)
  - **Level 11+**: Random 2-10 circles per level
- **Difficulty scaling**:
  - Circle speed increases by 20% each level
  - Circle size shrinks by 5% each level (minimum 3% of screen)
  - Base speed: 500 px/s
  - Light source at screen center creates dynamic highlights on moving circles

### Game Controls

- **Tap** - Touch any circle to destroy it and score points
- **Reset Button** - Restart the game from level 1
- **Exit Button** - Close the game

## 🎨 Visual Features

- **3D Spheres**: 24 gradient rings with 48 segments for ultra-smooth appearance
- **Lighting System**: Dynamic highlight positioning based on screen center light source
- **Particle Effects**: 20-30 particles per explosion with gravity and fade-out
- **Color System**: Contrasting circle colors based on background brightness
- **Smooth Animations**: Flash effects on touch with 150ms duration

## 📱 Technical Details

### Architecture

- **Language**: C++ (game engine) with Android NDK 25.1.8937393
- **Graphics**: OpenGL ES 2.0 with custom shaders
- **Build System**: Gradle 8.2, AGP 8.1.0, CMake 3.18.1
- **Package**: com.rog3rb0t.touchgame
- **Min SDK**: 24 (Android 7.0)
- **Target SDK**: 34 (Android 14)

### Project Structure

```text
pointLessApp/
├── app/
│   ├── src/main/
│   │   ├── cpp/                          # Native C++ code
│   │   │   ├── game.h                    # Game class header
│   │   │   ├── game.cpp                  # Core game logic & OpenGL rendering
│   │   │   ├── native-lib.cpp            # JNI bridge to Java
│   │   │   └── CMakeLists.txt            # CMake build configuration
│   │   ├── java/com/rog3rb0t/touchgame/ # Java/Android code
│   │   │   ├── GameActivity.java         # Main game activity with ads
│   │   │   ├── GameView.java             # OpenGL surface view
│   │   │   └── LoadingActivity.java      # Splash screen
│   │   ├── res/                          # Android resources
│   │   │   ├── drawable/                 # Button styles, icons
│   │   │   ├── layout/                   # XML layouts
│   │   │   └── mipmap/                   # App icons
│   │   └── AndroidManifest.xml           # App manifest
│   ├── build.gradle                      # App build configuration
│   └── proguard-rules.pro                # ProGuard rules
├── build.gradle                          # Project build configuration
├── gradle.properties                     # Gradle properties
├── settings.gradle                       # Project settings
├── local.properties.example              # AdMob config template
└── README.md                             # This file
```

### Key Technologies

**C++ Game Engine:**
- Circle physics with wall bouncing
- Particle system with gravity
- 24-ring spherical gradient rendering
- Dynamic lighting calculations
- Collision detection with 50% tolerance

**OpenGL ES 2.0:**
- Custom vertex/fragment shaders
- Gradient background shader
- Orthographic projection
- Blend modes for transparency

**Android Integration:**
- JNI callbacks for toast messages
- Haptic feedback system
- AdMob (banner & interstitial ads)
- ProGuard optimization for release

### AdMob Configuration

The app uses environment-based configuration for AdMob IDs stored in `local.properties`:

```properties
ADMOB_APP_ID=your-app-id-here
ADMOB_BANNER_AD_UNIT_ID=your-banner-id-here
ADMOB_INTERSTITIAL_AD_UNIT_ID=your-interstitial-id-here
```

See `local.properties.example` for template. The `local.properties` file is gitignored for security.

## 🔧 Requirements

- **Android Studio**: Arctic Fox or later
- **Android SDK**: API level 24-34
- **Android NDK**: 25.1.8937393
- **CMake**: 3.18.1 or later
- **Gradle**: 8.2
- **Java**: 1.8 (compatibility level)

## 🚀 Building the Project

### Setup

1. **Clone the repository**

   ```bash
   git clone https://github.com/ROG3RB0T/touchGame.git
   cd touchGame
   ```

2. **Configure AdMob (Optional)**

   ```bash
   cp local.properties.example local.properties
   # Edit local.properties with your AdMob IDs
   ```

3. **Install Android Studio**
   - Download from <https://developer.android.com/studio>
   - Install Android SDK and NDK through SDK Manager

4. **Open the project**

   ```bash
   # Open Android Studio and select "Open an Existing Project"
   # Navigate to the touchGame directory
   ```

5. **Install NDK and CMake**
   - Go to Tools → SDK Manager → SDK Tools
   - Check "NDK (Side by side)" and "CMake"
   - Click "Apply" to install

### Build Commands

**Debug build:**

```bash
./gradlew assembleDebug
# APK: app/build/outputs/apk/debug/app-debug.apk
```

**Release build:**

```bash
./gradlew assembleRelease
# APK: app/build/outputs/apk/release/app-release.apk
# AAB: app/build/outputs/bundle/release/app-release.aab
```

## 📲 Running the App

### Android Studio

1. Connect an Android device or start an emulator
2. Click the "Run" button (green play icon) or press Shift+F10
3. Select your device/emulator
4. The app will install and launch automatically

### Command Line

```bash
# Install debug APK
./gradlew installDebug

# Install to specific device
adb -s DEVICE_ID install app/build/outputs/apk/debug/app-debug.apk

# Launch the app
adb shell am start -n com.rog3rb0t.touchgame/.LoadingActivity
```

## 🎯 Game Implementation Details

### Rendering Pipeline

1. **Gradient Background**: Full-screen quad with vertical gradient
2. **3D Spheres**: 24 concentric rings × 48 segments with:
   - Spherical brightness calculation
   - Dynamic light direction from screen center
   - Specular highlights at light-facing surface
   - Rim lighting at edges
   - Flash effect on touch
3. **Particle System**: Explosions with 20-30 particles
   - Random velocity vectors
   - Gravity simulation (600 px/s²)
   - Alpha fade-out over lifetime

### Physics Engine

- **Movement**: Velocity-based with deltaTime interpolation
- **Collision**: Circle-to-wall bounce detection
- **Touch**: Expanded hit radius (1.5x visual radius)
- **Speed scaling**: `baseSpeed * (1 + (round-1) * 0.2)`

### Performance Optimizations

- 60 FPS target with delta time smoothing
- VBO reuse for circle geometry
- Particle culling after lifetime expires
- ProGuard minification for release builds

## 🎮 Controls Reference

- **Tap circles**: Touch any moving circle to destroy it and score points
- **Reset button** (🔄): Restart game from level 1
- **Exit button** (✖): Close the game
- **Automatic progression**: Level advances when all circles are cleared
- **Toast notifications**: Shows "Level complete! Advancing to round X" message

## 🐛 Troubleshooting

### Build Errors

- **NDK not found**: Install NDK 25.1.8937393 through SDK Manager
- **CMake errors**: Ensure CMake 3.18.1+ is installed
- **Gradle sync failed**: Check internet connection and gradle.properties
- **BuildConfig error**: Make sure `buildFeatures { buildConfig = true }` is in app/build.gradle

### Runtime Issues

- **App crashes on startup**: 
  - Check that device supports OpenGL ES 2.0
  - Verify AdMob configuration in `local.properties`
  - Review logcat for JNI or native library loading errors
- **Black screen**: Verify EGL initialization in logcat
- **No touch response**: Check touch event handling in GameView logs
- **Circles not visible**: Ensure background gradient is rendering properly

### AdMob Configuration

- Copy `local.properties.example` to `local.properties`
- Add your AdMob IDs or use test IDs for development
- Test ad IDs are provided as fallback in build.gradle

### Viewing Logs

```bash
# View all logs
adb logcat

# Filter for game logs
adb logcat -s TouchGame:I GameView:I

# Filter for errors
adb logcat *:E
```

## 📚 Additional Documentation

- [DEPLOYMENT.md](DEPLOYMENT.md) - Deployment instructions
- [PLAY_STORE_DEPLOYMENT.md](PLAY_STORE_DEPLOYMENT.md) - Play Store submission guide
- [INSTALL.txt](INSTALL.txt) - Installation guide

## 🤝 Contributing

This is an educational project. Feel free to fork and experiment with:

- Different particle effects
- Alternative game modes
- Enhanced 3D rendering techniques
- Additional power-ups or obstacles
- Multiplayer features
- Sound effects and music

## 📄 License

This project is for educational purposes. Feel free to use and modify for learning.

## 👤 Author

**rog3rb0t**
- GitHub: [@ROG3RB0T](https://github.com/ROG3RB0T)
- Repository: [touchGame](https://github.com/ROG3RB0T/touchGame)

## 🎯 Version History

- **v1.0.0** (2025-11-20)
  - Initial release
  - 3D sphere rendering with dynamic lighting
  - Progressive difficulty system
  - Touch detection with 50% tolerance
  - Particle explosion effects
  - AdMob integration
  - Level completion toast messages

---

**Built with ❤️ using Android NDK, C++, and OpenGL ES**
