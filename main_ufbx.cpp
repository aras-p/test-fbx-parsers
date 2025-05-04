#include "external/ufbx/ufbx.h"
#include <chrono>
#include <vector>
#include <execution>

/*
test_fbxsdk.exe 477 KB

Loading MEASURE_ONE.fbx...
Loaded in 1298.2 ms
Meshes: 7293 (3053.2 Kverts, 5732.4 Kfaces)
Lights: 0
Cameras: 1
Skeletons: 0

Loading rain_restaurant_bl43.fbx...
Loaded in 1166.6 ms
Meshes: 93 (62.6 Kverts, 59.3 Kfaces)
Lights: 7
Cameras: 1
Skeletons: 3495

Loading 5 input files in parallel:
- 'MEASURE_ONE.fbx'
- 'rain_restaurant_bl43.fbx'
- 'atvi_caldera_from_usd_bl45.fbx'
- 'intel_moore_lane_exported_from_usd_bl45.fbx'
- 'blender30splash_noanim_bl43.fbx'
Done in 2563.9 ms
- 'MEASURE_ONE.fbx' in 1426.9 ms
  - 7293 meshes (3053.2 Kverts, 5732.4 Kfaces), 0 lights, 1 cameras, 0 bones
- 'rain_restaurant_bl43.fbx' in 1193.1 ms
  - 93 meshes (62.6 Kverts, 59.3 Kfaces), 7 lights, 1 cameras, 3495 bones
- 'atvi_caldera_from_usd_bl45.fbx' in 2234.6 ms
  - 4371 meshes (9751.9 Kverts, 16740.1 Kfaces), 0 lights, 13 cameras, 113 bones
- 'intel_moore_lane_exported_from_usd_bl45.fbx' in 2532.9 ms
  - 605 meshes (10090.7 Kverts, 10286.1 Kfaces), 51 lights, 1 cameras, 0 bones
- 'blender30splash_noanim_bl43.fbx' in 1407.4 ms
  - 431 meshes (2162.6 Kverts, 2090.9 Kfaces), 6 lights, 1 cameras, 4121 bones
*/

struct Stats
{
    const char* path = nullptr;
    float ms = 0;
    size_t meshes = 0;
    size_t lights = 0;
    size_t cameras = 0;
    size_t skeletons = 0;
    size_t vertices = 0;
    size_t faces = 0;
};

int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("Usage: pass input FBX files on the command line\n");
        return 1;
    }
    const int input_file_count = argc - 1;
    printf("Loading %i input files in parallel:\n", input_file_count);
    std::vector<Stats> stats(input_file_count);
    for (int i = 0; i < input_file_count; i++)
    {
        stats[i].path = argv[i + 1];
        printf("- '%s'\n", stats[i].path);
    }

    // Process the files
    auto t0 = std::chrono::high_resolution_clock::now();
    std::for_each(std::execution::par, stats.begin(), stats.end(),
        [](Stats& stats) {
            auto ft0 = std::chrono::high_resolution_clock::now();

            // Load the file
            ufbx_load_opts opts = {};
            ufbx_error error = {};
            ufbx_scene* scene = ufbx_load_file(stats.path, &opts, &error);
            if (scene == nullptr) {
                printf("ERROR: failed to load fbx %s\n", stats.path);
                exit(1);
            }

            auto ft1 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float, std::milli> fdt = ft1 - ft0;
            stats.ms = fdt.count();

            // Gather some basic stats about the scene
            stats.meshes = scene->meshes.count;
            stats.cameras = scene->cameras.count;
            stats.lights = scene->lights.count;
            stats.skeletons = scene->bones.count;
            for (const ufbx_mesh* mesh : scene->meshes) {
                stats.vertices += mesh->num_vertices;
                stats.faces += mesh->num_faces;
            }
            ufbx_free_scene(scene);
        });
    auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> dt = t1 - t0;
    printf("Done in %.1f ms\n", dt.count());
    for (const Stats& st : stats)
    {
        printf("- '%s' in %.1f ms\n", st.path, st.ms);
        printf("  - %zi meshes (%.1f Kverts, %.1f Kfaces), %zi lights, %zi cameras, %zi bones\n", st.meshes, st.vertices / 1024.0, st.faces / 1024.0, st.lights, st.cameras, st.skeletons);
    }

    return 0;
}
