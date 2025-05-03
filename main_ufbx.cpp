#include "external/ufbx/ufbx.h"
#include <chrono>

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
*/

struct Stats
{
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
        printf("Usage: pass input FBX file on the command line\n");
        return 1;
    }
    const char* input_filename = argv[1];
    printf("Loading %s...\n", input_filename);

    auto t0 = std::chrono::high_resolution_clock::now();

    // Load the file
    ufbx_load_opts opts = {};
    ufbx_error error = {};
    ufbx_scene* scene = ufbx_load_file(input_filename, &opts, &error);
    if (scene == nullptr) {
        printf("ERROR: failed to load fbx %s\n", input_filename);
        return 1;
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> dt = t1 - t0;
    printf("Loaded in %.1f ms\n", dt.count());

    // Gather some basic stats about the scene
    Stats stats;
    stats.meshes = scene->meshes.count;
    stats.cameras = scene->cameras.count;
    stats.lights = scene->lights.count;
    stats.skeletons = scene->bones.count;
    for (const ufbx_mesh* mesh : scene->meshes) {
        stats.vertices += mesh->num_vertices;
        stats.faces += mesh->num_faces;
    }
    printf("Meshes: %zi (%.1f Kverts, %.1f Kfaces)\n", stats.meshes, stats.vertices / 1024.0, stats.faces / 1024.0);
    printf("Lights: %zi\n", stats.lights);
    printf("Cameras: %zi\n", stats.cameras);
    printf("Skeletons: %zi\n", stats.skeletons);

    // Cleanup
    ufbx_free_scene(scene);
    return 0;
}
