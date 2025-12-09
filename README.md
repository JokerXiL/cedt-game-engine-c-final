# VS3D

A 3D action game built on a custom OpenGL engine.

## Building

Requires CMake, a C++17 compiler, and vcpkg.

```bash
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build .
./game
```

## Disclaimer

This project does not include character models. The game will run without a player model, but to see the player character, provide your own model and save it as `player.glb` in the project root.

Samira character model is property of Riot Games. League of Legends and all related assets are trademarks of Riot Games, Inc. This project is not affiliated with or endorsed by Riot Games.