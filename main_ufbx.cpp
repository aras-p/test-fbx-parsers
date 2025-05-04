#include <vector>
#include "common.h"

#include "external/ufbx/ufbx.h"

#define IMPORT_FILES_IN_PARALLEL

#if defined(IMPORT_FILES_IN_PARALLEL)
#include <execution>
#endif

#define USE_UFBX_THREADS
#if defined(USE_UFBX_THREADS)
#define UFBX_OS_IMPLEMENTATION
#include "external/ufbx/ufbx_os.h"
static ufbx_os_thread_pool* g_thread_pool;
#endif

static void process_file(Stats& entry)
{
    auto t0 = time_now();

    // Load the file
    ufbx_load_opts opts = {};
#if defined(USE_UFBX_THREADS)
    ufbx_os_init_ufbx_thread_pool(&opts.thread_opts.pool, g_thread_pool);
#endif

    ufbx_error error = {};
    ufbx_scene* scene = ufbx_load_file(entry.path, &opts, &error);
    if (scene == nullptr) {
        printf("ERROR: failed to load fbx %s\n", entry.path);
        exit(1);
    }

    // Gather some basic stats about the scene
    entry.meshes = scene->meshes.count;
    entry.cameras = scene->cameras.count;
    entry.lights = scene->lights.count;
    entry.bones = scene->bones.count;
    for (const ufbx_mesh* mesh : scene->meshes) {
        entry.vertices += mesh->num_vertices;
        entry.faces += mesh->num_faces;
    }
    ufbx_free_scene(scene);

    entry.ms = time_duration_ms(t0);
}

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
    auto t0 = time_now();

#if defined(USE_UFBX_THREADS)
    {
        ufbx_os_thread_pool_opts pool_opts = { 0 };
        pool_opts.max_threads = 4;
        g_thread_pool = ufbx_os_create_thread_pool(&pool_opts);
    }
#endif

#if defined(IMPORT_FILES_IN_PARALLEL)
    std::for_each(std::execution::par, stats.begin(), stats.end(), [](Stats& st) { process_file(st); });
#else
    for (Stats& st : stats) { process_file(st); }
#endif
    float dt = time_duration_ms(t0);
    printf("Done in %.1f s\n", dt / 1000.0);
    Stats sum_stats = aggregate_stats(stats.size(), stats.data());
    print_sum_stats(sum_stats);
    return 0;
}
