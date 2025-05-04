#pragma once

#include <chrono>
#include <stdio.h>

inline std::chrono::high_resolution_clock::time_point time_now()
{
    return std::chrono::high_resolution_clock::now();
}

inline float time_duration_ms(std::chrono::high_resolution_clock::time_point t0)
{
    std::chrono::duration<float, std::milli> dt = std::chrono::high_resolution_clock::now() - t0;
    return dt.count();
}

struct Stats
{
    const char* path = nullptr;
    float ms = 0;
    size_t meshes = 0;
    size_t lights = 0;
    size_t cameras = 0;
    size_t bones = 0;
    size_t vertices = 0;
    size_t faces = 0;
};

inline Stats aggregate_stats(size_t count, const Stats* stats)
{
    Stats res;
    for (size_t i = 0; i < count; i++)
    {
        res.ms += stats[i].ms;
        res.meshes += stats[i].meshes;
        res.lights += stats[i].lights;
        res.cameras += stats[i].cameras;
        res.bones += stats[i].bones;
        res.vertices += stats[i].vertices;
        res.faces += stats[i].faces;
    }
    return res;
}

inline void print_sum_stats(const Stats& st)
{
    printf("- %zi meshes (%.1f Kverts, %.1f Kfaces), %zi lights, %zi cameras, %zi bones\n", st.meshes, st.vertices / 1024.0, st.faces / 1024.0, st.lights, st.cameras, st.bones);
}
