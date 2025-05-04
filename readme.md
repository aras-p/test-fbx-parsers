# Testing various Autodesk .FBX parsing libraries

### Libraries

* ufbx: https://github.com/ufbx/ufbx (v0.18.2, 2025 Apr, 5b5494b). MIT or Public Domain license.
* FBX SDK: [Autodesk FBX SDK](https://aps.autodesk.com/developer/overview/fbx-sdk), 2020.3.7.
* AssImp: https://github.com/assimp/assimp (5.4.3, 2024 Aug, c35200e). 3-clause BSD license.
* OpenFBX: https://github.com/nem0/OpenFBX (2024 Dec, 82a43d9). MIT license.


### Performance

| Parser                   | Time sequential, s | Time parallel, s | Executable size, KB |
|--------------------------|------:|-------:|-----:|
| ufbx                     |   9.8 |    2.7 |  457 |
| ufbx w/ internal threads |   4.4 |    2.6 |  463 |
| FBX SDK                  | 869.9 | crash! | 4508 |
| AssImp                   |  33.9 |   26.9 | 1058 |
| OpenFBX                  |  26.7 |   15.9 |  316 |

Note that `ufbx` is built with various "not interesting for me" parts of it disabled. With all default
settings, they all would increase the executable size by about 40 KB. The build flags are:
`UFBX_NO_SUBDIVISION`, `UFBX_NO_TESSELLATION`, `UFBX_NO_GEOMETRY_CACHE`, `UFBX_NO_SCENE_EVALUATION`,
`UFBX_NO_SKINNING_EVALUATION`, `UFBX_NO_ANIMATION_BAKING`, `UFBX_NO_TRIANGULATION`,
`UFBX_NO_INDEX_GENERATION`, `UFBX_NO_FORMAT_OBJ`.

- Correct: `16812 meshes (28499.1 Kverts, 40006.0 Kfaces), 69 lights, 20 cameras, 10399 bones`
- OpenFBX: `16812 meshes (132519.7 Kverts, 40006.0 Kfaces), 69 lights, 20 cameras, 10399 bones`
- AssImp: `18750 meshes (132504.1 Kverts, 40006.0 Kfaces), 70 lights, 20 cameras, 0 bones`
