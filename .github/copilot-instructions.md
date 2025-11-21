# Android NDK C++ Touch Game Project

This is an Android NDK project written in C++ for a touch-based game where users tap a moving circle.

## Project Structure
- Native C++ code using Android NDK
- OpenGL ES for rendering
- JNI bridge for Android integration
- Gradle build system with CMake

## Game Mechanics
- Moving circle that users must tap
- Score increases by 10 points per successful tap
- Speed increases with each round
- Bonus points: 10 + round number
- Game continues until 25 points are reached

## Development Guidelines
- Use C++ for core game logic
- OpenGL ES for graphics rendering
- Follow Android NDK best practices
- Keep JNI calls minimal for performance
