# Testing various Autodesk .FBX parsing libraries

### Libraries

* `fbxsdk`: [Autodesk FBX SDK](https://aps.autodesk.com/developer/overview/fbx-sdk), 2020.3.7.
* `ufbx`: https://github.com/ufbx/ufbx v0.18.2 (2025 Apr, 5b5494b). MIT or Public Domain.


### Performance

| Parser                           | Import time, sec | Executable size, KB |
|----------------------------------|---:|---:|---:|
| FBX SDK                          | 565.4 | 4508 |
| ufbx sequential                  |   7.9 | 479 |
| ufbx parallel                    |   2.6 | 496 |
| ufbx parallel + internal threads |   2.1 | 501 |

Expected: `- 12793 meshes (25120.9 Kverts, 34908.7 Kfaces), 64 lights, 17 cameras, 7729 bones`
