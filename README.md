# voxelProject

## About
Simple voxel game with a sandbox theme.

## Requirements
To run this program you will need:
- GLFW
- GLM
- STB Image
- OpenGL 3.3
- C++20
- FreeType 2

## Compiling and running
```bash
make run
```

## Controls

- Movement through WASD
- Jump with Space
- Run with Shift
- Left and Right Mouse Button for breaking and placing blocks.
- Scrollwheel scrolls through placable blocks
- G enables No-Clip and Flight

## General How-To-Use

- Render Distance and Resolution can be found in game.h

## Will do specifications:
- [x] A generated voxel world.
- [x] Pseudo-random seed-based world generation (small map at startup).
- [x] First-person movement along the terrain.
- [x] Multiple different block-types, distinguished by different textures.

## Might do specifications:
- [x] Object placement / destruction.
- [ ] Saving and loading the world state.
- [ ] Player animations.
- [x] Procedurally generated terrain.
- [ ] Loading / unloading of world data to improve performance.
- [x] Procedurally placed structures. (i.e Trees)
- [ ] Sound.
- [x] Ambient Occlusion
