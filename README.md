# Touch Game - Android NDK C++ Project

A simple Android game built with C++ and OpenGL ES where players tap a moving circle to score points.

## Game Description

- Tap the moving red circle to score points
- First tap: 10 points + 1 (round number) = 11 points
- Each successful tap increases the round number and circle speed
- Points per tap: 10 + current round number
- Game ends when you reach 25 points

## Project Structure

```
pointLessApp/
├── app/
│   ├── src/
│   │   └── main/
│   │       ├── cpp/              # C++ source files
│   │       │   ├── game.h        # Game class header
│   │       │   ├── game.cpp      # Game logic implementation
│   │       │   ├── native-lib.cpp # JNI bridge
│   │       │   └── CMakeLists.txt # CMake build configuration
│   │       ├── java/             # Java source files
│   │       │   └── com/example/touchgame/
│   │       │       ├── GameActivity.java
│   │       │       └── GameView.java
│   │       └── AndroidManifest.xml
│   └── build.gradle              # App-level build configuration
├── build.gradle                   # Project-level build configuration
├── settings.gradle               # Project settings
└── gradle.properties             # Gradle properties
```

## Requirements

- Android Studio (Arctic Fox or later)
- Android SDK (API level 24 or higher)
- Android NDK (version 21 or later)
- CMake 3.18.1 or later
- Gradle 8.2

## Building the Project

1. **Install Android Studio**
   - Download from https://developer.android.com/studio
   - Install Android SDK and NDK through SDK Manager

2. **Open the project**
   ```bash
   # Open Android Studio and select "Open an Existing Project"
   # Navigate to the pointLessApp directory
   ```

3. **Install NDK and CMake**
   - Go to Tools → SDK Manager → SDK Tools
   - Check "NDK (Side by side)" and "CMake"
   - Click "Apply" to install

4. **Build the project**
   ```bash
   # From command line:
   ./gradlew build
   
   # Or use Android Studio:
   # Build → Make Project (Ctrl+F9)
   ```

## Running the App

### Using Android Studio

1. Connect an Android device or start an emulator
2. Click the "Run" button (green play icon) or press Shift+F10
3. Select your device/emulator
4. The app will install and launch automatically

### Using Command Line

```bash
# Build and install debug APK
./gradlew installDebug

# Or build release APK
./gradlew assembleRelease
# APK will be in: app/build/outputs/apk/release/
```

## Game Controls

- **Touch**: Tap anywhere on the screen to interact
- The game automatically detects if you hit the moving circle
- Score and round number are displayed at the top of the screen
- Game ends automatically when you reach 25 points

## Technical Details

### C++ Components

- **game.h/game.cpp**: Core game logic
  - Circle physics and movement
  - Collision detection
  - Score and round management
  - OpenGL ES rendering

- **native-lib.cpp**: JNI interface
  - EGL/OpenGL initialization
  - Bridge between Java and C++
  - Native method implementations

### Java Components

- **GameActivity.java**: Main activity
  - UI setup and management
  - Score display updates
  - Game over notification

- **GameView.java**: Custom SurfaceView
  - Render loop (60 FPS)
  - Touch event handling
  - Native library loading

### Graphics

- Uses OpenGL ES 2.0 for rendering
- Custom vertex and fragment shaders
- Orthographic projection for 2D rendering
- 60 FPS target frame rate

## Troubleshooting

### Build Errors

- **NDK not found**: Install NDK through SDK Manager
- **CMake errors**: Ensure CMake 3.18.1+ is installed
- **Gradle sync failed**: Check internet connection and gradle.properties

### Runtime Issues

- **App crashes on startup**: Check that device supports OpenGL ES 2.0
- **Black screen**: Verify EGL initialization in logcat
- **No touch response**: Check touch event handling in GameView

### Viewing Logs

```bash
# View all logs
adb logcat

# Filter for game logs
adb logcat -s TouchGame:I
```

## Development Notes

- Minimum SDK: API 24 (Android 7.0)
- Target SDK: API 34 (Android 14)
- C++ Standard: C++17
- OpenGL ES Version: 2.0

## License

This is a sample educational project.
