# Testing various Autodesk .FBX parsing libraries

I was working on [new FBX importer for Blender 4.5](https://projects.blender.org/blender/blender/pulls/132406),
and while that one is based on [ufbx](https://github.com/ufbx/ufbx) library, I wanted to do a quick
comparison between various FBX format parsing libraries.

### Libraries

* ufbx: https://github.com/ufbx/ufbx (v0.18.2, 2025 Apr, 5b5494b). MIT or Public Domain license.
* FBX SDK: [Autodesk FBX SDK](https://aps.autodesk.com/developer/overview/fbx-sdk), 2020.3.7. Proprietary license.
* AssImp: https://github.com/assimp/assimp (5.4.3, 2024 Aug, c35200e). 3-clause BSD license.
* OpenFBX: https://github.com/nem0/OpenFBX (2024 Dec, 82a43d9). MIT license.


### Performance

Importing 9 large-ish FBX files (total size 2GB), on Ryzen 5950X CPU (Windows 10, built with Visual Studio 2022 v17.13.1),
in "Release" build configuration. See details on files below.

What the test application does, is parse the input files, and does a _very basic_ "summary": how many meshes, lights, cameras,
bones are present, and the total sum of mesh vertices and faces. Nothing related to animations is tested.

| Parser                   | Time sequential, s | Time parallel, s | Executable size, KB |
|--------------------------|------:|-------:|-----:|
| ufbx                     |   9.8 |    2.7 |  457 |
| ufbx w/ internal threads |   4.4 |    2.6 |  462 |
| FBX SDK                  | 869.9 | crash! | 4508 |
| AssImp                   |  33.9 |   26.7 | 1060 |
| OpenFBX                  |  26.7 |   15.8 |  312 |

Files being tested (not in the repo, but I've [uploaded them here](https://aras-p.info/files/blender/fbx_data/test_fbx_parsers/)): 
- Caldera:  388MB file exported out of [Activision Caldera](https://github.com/Activision/caldera) USD data set.
- Moore Lane: 502MB file exported out of [Intel Moore Lane](https://dpel.aswf.io/4004-moore-lane/) USD data set.
- Rain Restaurant: 267MB file exported out of [Hi, my name is Amy](https://studio.blender.org/characters/rain/showcase/1/) Blender Studio character showcase. Complex character rig, lots of animation curves.
- 3.0 Splash: 288MB file exported out of [Blender 3.0 Splash](https://cloud.blender.org/p/gallery/617933e9b7b35ce1e1c01066) (Sprite Fright) demo file. Lots of instancing, no animations.
- 3.0 Splash Ellie: 232MB file exported out of [Blender 3.0 Splash](https://cloud.blender.org/p/gallery/617933e9b7b35ce1e1c01066) (Sprite Fright) demo file. Only the animated Ellie character.
- Measure One: 290MB file from [Beeple Zero Day](https://developer.nvidia.com/orca/beeple-zero-day) data set.
- Bistro: three files (117MB, 41MB, 48MB) from [Amazon Lumberyard Bistro](https://developer.nvidia.com/orca/amazon-lumberyard-bistro) data set.

### Summary

`ufbx` seems to be overall winner in all aspects. It is written very cleanly, is trivial to build (just one source file),
has good comments, is _very extensively_ tested, and is very fast.

FBX SDK is... _not fast_, shall we say. Out of the files I tested, it seems to spend most of the time in _cleaning up_ after the import is done
(in `sdk->Destroy()`), and that is pathologically bad performance in files that have lots of instancing. Feels like it does cleanup
by removing objects and their relations one-by-one, de-registering the removed relations from all affected objects. Probably resulting in
quadratic complexity in terms of relation/object count.

It also can not be safetly multi-threaded, like if you want to parse multiple FBX files on their own threads. The SDK documentation is not very
clear on this; it says that access to the same `FbxManager` from multiple threads is definitely not safe (fine!), and suggests that you
_can_ create multiple `FbxManager` objects (fine!), but then if you try to use one of them per each imported file, in parallel, you get
random crashes deep from the innards of the SDK. Probably has some actually shared / global data.

### Notes

I have only built and tested this code on Windows, with Visual Studio 2022. Just do "Open with Visual Studio" on the folder; it goes into regular CMake
project mode.

#### Parallel file import

An easy way to import several FBX files in parallel would be C++17 `<execution>` and doing `std::for_each` with `std::execution::par`. However,
that is not available / implemented in Apple's Xcode/clang at this time. So instead I'm using a simple `ic_pfor.h` utility from Ignacio Castaño's
[ICBC](https://github.com/castano/icbc) repository.

#### ufbx build options

`ufbx` is built with various "not interesting for me" parts of it disabled. With all default
settings, they all would increase the executable size by about 40 KB. The build flags are:
`UFBX_NO_SUBDIVISION`, `UFBX_NO_TESSELLATION`, `UFBX_NO_GEOMETRY_CACHE`, `UFBX_NO_SCENE_EVALUATION`,
`UFBX_NO_SKINNING_EVALUATION`, `UFBX_NO_ANIMATION_BAKING`, `UFBX_NO_TRIANGULATION`,
`UFBX_NO_INDEX_GENERATION`, `UFBX_NO_FORMAT_OBJ`.

ufbx can also do internal threading _while parsing one file_, which is off by default but can be set up
by pointing to job run/wait functions in `ufbx_load_opts`. In the table above, this configuration is
`ufbx w/ internal threads`.

#### FBX SDK location

The test application expects FBX SDK headers and libraries to be located locally, under `external/fbxsdk/include` and
`external/fbxsdk/lib/x64` (`debug` and `release` under that). I'm only testing on Windows, x64 build, and only the `-mt`
library variants.

#### Statistics computed by the test applications

"Correct" outcome of my data files, as produced by both FBX SDK and ufbx, is `16812 meshes (28499.1 Kverts, 40006.0 Kfaces), 69 lights, 20 cameras, 10399 bones`.
- OpenFBX seems to produce "corner" vertices of the faces, without ability to query
  original "logical" vertex count, so in the end it miscounts as `132519.7 Kverts`.
- AssImp similarly miscounts the vertices (but it produces `132504.1 Kverts`), and
  possibly something else related to instancing, as it produces `18750 meshes` and `70 lights` numbers.
