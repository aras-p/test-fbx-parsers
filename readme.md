# Testing various Autodesk .FBX parsing libraries

### Libraries

* ufbx: https://github.com/ufbx/ufbx (v0.18.2, 2025 Apr, 5b5494b). MIT or Public Domain license.
* FBX SDK: [Autodesk FBX SDK](https://aps.autodesk.com/developer/overview/fbx-sdk), 2020.3.7.
* OpenFBX: https://github.com/nem0/OpenFBX (2024 Dec, 82a43d9). MIT license.


### Performance

| Parser                   | Time sequential, sec | Time parallel, sec | Executable size, KB |
|--------------------------|------:|-------:|-----:|
| ufbx                     |   7.9 |    2.6 |  457 |
| ufbx w/ internal threads |   3.2 |    2.1 |  463 |
| FBX SDK                  | 565.4 | crash! | 4508 |
| OpenFBX                  |  19.7 |   12.6 |  316 |

Note that `ufbx` is built with various "not interesting for me" parts of it disabled. With all default
settings, they all would increase the executable size by about 40 KB. The build flags are:
`UFBX_NO_SUBDIVISION`, `UFBX_NO_TESSELLATION`, `UFBX_NO_GEOMETRY_CACHE`, `UFBX_NO_SCENE_EVALUATION`,
`UFBX_NO_SKINNING_EVALUATION`, `UFBX_NO_ANIMATION_BAKING`, `UFBX_NO_TRIANGULATION`,
`UFBX_NO_INDEX_GENERATION`, `UFBX_NO_FORMAT_OBJ`.

Expected: `- 12793 meshes (25120.9 Kverts, 34908.7 Kfaces), 64 lights, 17 cameras, 7729 bones`
