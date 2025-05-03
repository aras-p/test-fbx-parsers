#include <fbxsdk.h>
#include <chrono>

/*
test_fbxsdk.exe 4505 KB

Loading MEASURE_ONE.fbx...
Loaded in 3606.8 ms
Meshes: 7293 (3053.2 Kverts, 5732.4 Kfaces)
Lights: 0
Cameras: 1
Skeletons: 0

Loading rain_restaurant_bl43.fbx...
Loaded in 11716.8 ms
Meshes: 93 (62.6 Kverts, 59.3 Kfaces)
Lights: 8
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

static void walk_attribute(FbxNodeAttribute* attr, Stats& r_stats)
{
    if (!attr)
        return;

    switch (attr->GetAttributeType()) {
    case FbxNodeAttribute::eSkeleton:
        r_stats.skeletons++;
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
        printf("Light %s\n", attr->GetName());
        break;
    }
}

static void walk_node(FbxNode* node, Stats &r_stats)
{
    if (!node)
        return;
    for (int i = 0, n = node->GetNodeAttributeCount(); i != n; i++)
        walk_attribute(node->GetNodeAttributeByIndex(i), r_stats);
    for (int i = 0, n = node->GetChildCount(); i != n; i++)
        walk_node(node->GetChild(i), r_stats);
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("Usage: pass input FBX file on the command line\n");
        return 1;
    }
    const char* input_filename = argv[1];
    printf("Loading %s...\n", input_filename);

    auto t0 = std::chrono::high_resolution_clock::now();

    // Create SDK manager, IO settings and a scene
    FbxManager* sdk = FbxManager::Create();
    FbxIOSettings* ios = FbxIOSettings::Create(sdk, IOSROOT);
    sdk->SetIOSettings(ios);
    FbxScene* scene = FbxScene::Create(sdk, "myScene");

    // Import the file
    FbxImporter* importer = FbxImporter::Create(sdk, "");
    if (!importer->Initialize(input_filename, -1, sdk->GetIOSettings())) {
        printf("Call to FbxImporter::Initialize() failed.\n");
        printf("Error returned: %s\n\n", importer->GetStatus().GetErrorString());
        return 1;
    }
    importer->Import(scene);
    importer->Destroy();

    auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> dt = t1 - t0;
    printf("Loaded in %.1f ms\n", dt.count());

    // Gather some basic stats about the scene
    Stats stats;
    FbxNode* root = scene->GetRootNode();
    walk_node(root, stats);
    printf("Meshes: %zi (%.1f Kverts, %.1f Kfaces)\n", stats.meshes, stats.vertices / 1024.0, stats.faces / 1024.0);
    printf("Lights: %zi\n", stats.lights);
    printf("Cameras: %zi\n", stats.cameras);
    printf("Skeletons: %zi\n", stats.skeletons);

    // Cleanup
    sdk->Destroy();
    return 0;
}
