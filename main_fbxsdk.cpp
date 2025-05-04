#include <fbxsdk.h>
#include <vector>
#include "common.h"
#include <unordered_set>

struct WalkState
{
    std::unordered_set<const FbxNodeAttribute*> seen_nodes;
};

static void walk_attribute(FbxNodeAttribute* attr, Stats& r_stats, WalkState& state)
{
    if (!attr)
        return;
    if (!state.seen_nodes.insert(attr).second)
        return; // this node already included in stats (happens with instancing)


    switch (attr->GetAttributeType())
    {
    case FbxNodeAttribute::eSkeleton:
        r_stats.bones++;
        break;
    case FbxNodeAttribute::eMesh:
        r_stats.meshes++;
        {
            FbxMesh* mesh = (FbxMesh*)attr;
            r_stats.vertices += mesh->GetControlPointsCount();
            r_stats.faces += mesh->GetPolygonCount();
        }
        break;
    case FbxNodeAttribute::eCamera:
        r_stats.cameras++;
        break;
    case FbxNodeAttribute::eLight:
        r_stats.lights++;
        break;
    }
}

static void walk_node(FbxNode* node, Stats &r_stats, WalkState &state)
{
    if (!node)
        return;
    for (int i = 0, n = node->GetNodeAttributeCount(); i != n; i++)
        walk_attribute(node->GetNodeAttributeByIndex(i), r_stats, state);
    for (int i = 0, n = node->GetChildCount(); i != n; i++)
        walk_node(node->GetChild(i), r_stats, state);
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
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

    // Process the files.
    // Note: you would think that you can do multi-threaded processing of FBX files,
    // by having each thread create its own FbxManager. But somewhere deep within
    // FBX SDK, it does use some global/shared state even with separate FbxManagers,
    // and you will get random crashes :(
    auto t0 = time_now();
    for (Stats& st : stats)
    {
        auto ft0 = time_now();
        // Create SDK manager, IO settings and a scene
        FbxManager* sdk = FbxManager::Create();
        FbxIOSettings* ios = FbxIOSettings::Create(sdk, IOSROOT);
        sdk->SetIOSettings(ios);
        FbxScene* scene = FbxScene::Create(sdk, "myScene");

        // Import the file
        FbxImporter* importer = FbxImporter::Create(sdk, "");
        if (!importer->Initialize(st.path, -1, sdk->GetIOSettings())) {
            printf("Call to FbxImporter::Initialize() failed.\n");
            printf("Error returned: %s\n\n", importer->GetStatus().GetErrorString());
            exit(1);
        }
        importer->Import(scene);
        importer->Destroy();

        // Gather some basic stats about the scene
        FbxNode* root = scene->GetRootNode();
        WalkState state;
        walk_node(root, st, state);
        sdk->Destroy();

        st.ms = time_duration_ms(ft0);
    };

    float dt = time_duration_ms(t0);
    printf("Done in %.1f s\n", dt / 1000.0);
    Stats sum_stats = aggregate_stats(stats.size(), stats.data());
    print_sum_stats(sum_stats);

    return 0;
}
