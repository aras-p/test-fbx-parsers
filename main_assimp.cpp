#include <vector>
#include "common.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"

#define IMPORT_FILES_IN_PARALLEL

#if defined(IMPORT_FILES_IN_PARALLEL)
#define IC_PFOR_IMPLEMENTATION
#define IC_INIT_THREAD_CRT 1
#include "external/ic_pfor.h"
#endif

static void process_file(Stats& entry)
{
    auto t0 = time_now();

    // Load the file
    Assimp::Importer imp;
    const aiScene* scene = imp.ReadFile(entry.path, 0);
    if (scene == nullptr) {
        printf("ERROR: failed to load fbx %s\n", entry.path);
        exit(1);
    }

    // Gather some basic stats about the scene
    entry.meshes = scene->mNumMeshes;
    entry.cameras = scene->mNumCameras;
    entry.lights = scene->mNumLights;
    //@TODO: bones
    for (int idx = 0; idx < entry.meshes; idx++)
    {
        const aiMesh* m = scene->mMeshes[idx];
        entry.vertices += m->mNumVertices;
        entry.faces += m->mNumFaces;
    }

    entry.ms = time_duration_ms(t0);
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("Usage: pass input FBX files on the command line\n");
        return 1;
    }
    const int input_file_count = argc - 1;
    printf("Loading %i input files:\n", input_file_count);
    std::vector<Stats> stats(input_file_count);
    for (int i = 0; i < input_file_count; i++)
    {
        stats[i].path = argv[i + 1];
        printf("- '%s'\n", stats[i].path);
    }

    // Process the files
    auto t0 = time_now();

#if defined(IMPORT_FILES_IN_PARALLEL)
    ic::init_pfor(std::min(input_file_count, 32));
    ic::pfor(input_file_count, 1, [&](int index) { process_file(stats[index]); });
    ic::shut_pfor();
#else
    for (Stats& st : stats) { process_file(st); }
#endif
    float dt = time_duration_ms(t0);
    printf("Done in %.1f s\n", dt / 1000.0);
    Stats sum_stats = aggregate_stats(stats.size(), stats.data());
    print_sum_stats(sum_stats);
    return 0;
}
