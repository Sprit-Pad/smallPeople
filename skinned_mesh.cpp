#include "misc.h"
#include "skinned_mesh.h"

#include "shader.h"
#include "texture.h"
#include "Camera/Camera.h"

using namespace DirectX;
/// <summary>
/// FbxAMatrix から XMFLOAT4X4 に変換
/// </summary>
inline XMFLOAT4X4 to_xmfloat4x4(const FbxAMatrix& fbxamatrix)
{
    XMFLOAT4X4 xmfloat4x4;
    for (int row = 0; row < 4; row++)
    {
        for (int column = 0; column < 4; column++)
        {
            xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
        }
    }
    return xmfloat4x4;
}

/// <summary>
/// FbxDouble3 から XMFLOAT3 に変換
/// </summary>
inline XMFLOAT3 to_xmfloat3(const FbxDouble3& fbxdouble3)
{
    XMFLOAT3 xmfloat3;
    xmfloat3.x = static_cast<float>(fbxdouble3[0]);
    xmfloat3.y = static_cast<float>(fbxdouble3[1]);
    xmfloat3.z = static_cast<float>(fbxdouble3[2]);
    return xmfloat3;
}

/// <summary>
/// FbxDouble4 から XMFLOAT4 に変換
/// </summary>
inline XMFLOAT4 to_xmfloat4(const FbxDouble4& fbxdouble4)
{
    XMFLOAT4 xmfloat4;
    xmfloat4.x = static_cast<float>(fbxdouble4[0]);
    xmfloat4.y = static_cast<float>(fbxdouble4[1]);
    xmfloat4.z = static_cast<float>(fbxdouble4[2]);
    xmfloat4.w = static_cast<float>(fbxdouble4[3]);
    return xmfloat4;
}

 




void skinned_mesh::fetch_animations(FbxScene* fbx_scene, std::vector<animation>& animation_clips, float sampling_rate)
 {
   FbxArray<FbxString*> animation_stack_names;
   fbx_scene->FillAnimStackNameArray(animation_stack_names);
   const int animation_stack_count{ animation_stack_names.GetCount() };
   for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
     {
    animation & animation_clip{ animation_clips.emplace_back() };
     animation_clip.name = animation_stack_names[animation_stack_index]->Buffer();
    
     FbxAnimStack * animation_stack{ fbx_scene->FindMember<FbxAnimStack>(animation_clip.name.c_str()) };
     fbx_scene->SetCurrentAnimationStack(animation_stack);
    
     const FbxTime::EMode time_mode{ fbx_scene->GetGlobalSettings().GetTimeMode() };
     FbxTime one_second;
     one_second.SetTime(0, 0, 1, 0, 0, time_mode);
     animation_clip.sampling_rate = sampling_rate > 0 ?
     sampling_rate : static_cast<float>(one_second.GetFrameRate(time_mode));
     const FbxTime sampling_interval
    { static_cast<FbxLongLong>(one_second.Get() / animation_clip.sampling_rate) };
     const FbxTakeInfo * take_info{ fbx_scene->GetTakeInfo(animation_clip.name.c_str()) };
     const FbxTime start_time{ take_info->mLocalTimeSpan.GetStart() };
     const FbxTime stop_time{ take_info->mLocalTimeSpan.GetStop() };
     for (FbxTime time = start_time; time < stop_time; time += sampling_interval)
         {
        animation::keyframe & keyframe{ animation_clip.sequence.emplace_back() };
        
         const size_t node_count{ scene_view.nodes.size() };
        keyframe.nodes.resize(node_count);
        for (size_t node_index = 0; node_index < node_count; ++node_index)
            {
             FbxNode * fbx_node{ fbx_scene->FindNodeByName(scene_view.nodes.at(node_index).name.c_str()) };
        if (fbx_node)
             {
                animation::keyframe::node & node{ keyframe.nodes.at(node_index) };
            // 'global_transform' is a transformation matrix of a node with respect to
             // the scene's global coordinate system.
                node.global_transform = to_xmfloat4x4(fbx_node->EvaluateGlobalTransform(time));
                const FbxAMatrix& local_transform{ fbx_node->EvaluateLocalTransform(time) };
                node.scaling = to_xmfloat3(local_transform.GetS());
                node.rotation = to_xmfloat4(local_transform.GetQ());
                node.translation = to_xmfloat3(local_transform.GetT());


            }
         }
         }
     }
    for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
         {
        delete animation_stack_names[animation_stack_index];
        }
     }


void skinned_mesh::update_animation(animation::keyframe & keyframe)
 {
     size_t node_count{ keyframe.nodes.size() };
     for (size_t node_index = 0; node_index < node_count; ++node_index)
         {
        animation::keyframe::node & node{ keyframe.nodes.at(node_index) };
        XMMATRIX S{ XMMatrixScaling(node.scaling.x, node.scaling.y, node.scaling.z) };
        XMMATRIX R{ XMMatrixRotationQuaternion(XMLoadFloat4(&node.rotation)) };
        XMMATRIX T{ XMMatrixTranslation(node.translation.x, node.translation.y, node.translation.z) };
        
         int64_t parent_index{ scene_view.nodes.at(node_index).parent_index };
         XMMATRIX P{ parent_index < 0 ? XMMatrixIdentity() :
         XMLoadFloat4x4(&keyframe.nodes.at(parent_index).global_transform) };
        
         XMStoreFloat4x4(&node.global_transform, S * R * T * P);
         }
     }
void skinned_mesh::blend_animations(
    const animation::keyframe* keyframes[2],
    float factor,
    animation::keyframe& keyframe)
{
    size_t node_count{ keyframes[0]->nodes.size() };
    keyframe.nodes.resize(node_count);
    for (size_t node_index = 0; node_index < node_count; ++node_index)
    {
        XMVECTOR S[2]{
            XMLoadFloat3(&keyframes[0]->nodes.at(node_index).scaling),
            XMLoadFloat3(&keyframes[1]->nodes.at(node_index).scaling)
        };
        DirectX::XMStoreFloat3(&keyframe.nodes.at(node_index).scaling, XMVectorLerp(S[0], S[1], factor));

        XMVECTOR R[2]{
            XMLoadFloat4(&keyframes[0]->nodes.at(node_index).rotation),
            XMLoadFloat4(&keyframes[1]->nodes.at(node_index).rotation)
        };
        XMStoreFloat4(&keyframe.nodes.at(node_index).rotation, XMQuaternionSlerp(R[0], R[1], factor));

        XMVECTOR T[2]{
            XMLoadFloat3(&keyframes[0]->nodes.at(node_index).translation),
            XMLoadFloat3(&keyframes[1]->nodes.at(node_index).translation)
        };
        DirectX::XMStoreFloat3(&keyframe.nodes.at(node_index).translation, XMVectorLerp(T[0], T[1], factor));
    }
}


void skinned_mesh::fetch_bone_influences(const FbxMesh* fbx_mesh, std::vector<bone_influences_per_control_point>& bone_influences)
{
    const int control_points_count{ fbx_mesh->GetControlPointsCount() };

    bone_influences.resize(control_points_count);
   
     const int skin_count{ fbx_mesh->GetDeformerCount(FbxDeformer::eSkin) };
    for (int skin_index = 0; skin_index < skin_count; ++skin_index)
        {
        const FbxSkin * fbx_skin
        { static_cast<FbxSkin*>(fbx_mesh->GetDeformer(skin_index, FbxDeformer::eSkin)) };
       
        const int cluster_count{ fbx_skin->GetClusterCount() };
        for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
            {
            const FbxCluster * fbx_cluster{ fbx_skin->GetCluster(cluster_index) };
           
            const int control_point_indices_count{ fbx_cluster->GetControlPointIndicesCount() };
            for (int control_point_indices_index = 0; control_point_indices_index < control_point_indices_count;
                 ++control_point_indices_index)
                 {
                 int control_point_index{ fbx_cluster->GetControlPointIndices()[control_point_indices_index] };
                 double control_point_weight
                  { fbx_cluster->GetControlPointWeights()[control_point_indices_index] };
                 bone_influence & bone_influence{ bone_influences.at(control_point_index).emplace_back() };
                 bone_influence.bone_index = static_cast<uint32_t>(cluster_index);
                 bone_influence.bone_weight = static_cast<float>(control_point_weight);
                 }
             }
         }
   
}

void skinned_mesh::fetch_skeleton(FbxMesh* fbx_mesh, skeleton& bind_pose)
 {
     const int deformer_count = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
    for (int deformer_index = 0; deformer_index < deformer_count; ++deformer_index)
        {
        FbxSkin * skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(deformer_index, FbxDeformer::eSkin));
        const int cluster_count = skin->GetClusterCount();
        bind_pose.bones.resize(cluster_count);
        for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
             {
             FbxCluster * cluster = skin->GetCluster(cluster_index);
            
             skeleton::bone & bone{ bind_pose.bones.at(cluster_index) };
             bone.name = cluster->GetLink()->GetName();
             bone.unique_id = cluster->GetLink()->GetUniqueID();
             bone.parent_index = bind_pose.indexof(cluster->GetLink()->GetParent()->GetUniqueID());
             bone.node_index = scene_view.indexof(bone.unique_id);
            
            //'reference_global_init_position' is used to convert from local space of model(mesh) to
                 // global space of scene.
                FbxAMatrix reference_global_init_position;
            cluster->GetTransformMatrix(reference_global_init_position);
            
            // 'cluster_global_init_position' is used to convert from local space of bone to
                 // global space of scene.
                 FbxAMatrix cluster_global_init_position;
          cluster->GetTransformLinkMatrix(cluster_global_init_position);
          
           // Matrices are defined using the Column Major scheme. When a FbxAMatrix represents a transformation
                 // (translation, rotation and scale), the last row of the matrix represents the translation part of
                 // the transformation.
                 // Compose 'bone.offset_transform' matrix that trnasforms position from mesh space to bone space.
                 // This matrix is called the offset matrix.
                 bone.offset_transform
                 = to_xmfloat4x4(cluster_global_init_position.Inverse() * reference_global_init_position);
             }
        }
    int a = 0;
   }


void skinned_mesh::UpdateTransform(const DirectX::XMFLOAT4X4& transform)
{
    // 入力の変換行列をXMMATRIXに変換
    DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&transform);

    // 各メッシュごとの変換を更新
    for (mesh& mesh : meshes)
    {
        // メッシュのデフォルトのグローバル変換行列をXMMATRIXに変換
        DirectX::XMMATRIX MeshTransform = DirectX::XMLoadFloat4x4(&mesh.default_global_transform);

        // メッシュの新しいグローバル変換行列を計算
        DirectX::XMMATRIX GlobalMeshTransform = MeshTransform * ParentWorldTransform;

        // 計算結果をXMFLOAT4X4に格納
        DirectX::XMStoreFloat4x4(&mesh.default_global_transform, GlobalMeshTransform);
    }
   
}

void skinned_mesh::create_com_objects(ID3D11Device* device, const char* fbx_filename)
{
    for (mesh& mesh : meshes)
    {
        HRESULT hr{ S_OK };
        D3D11_BUFFER_DESC buffer_desc{};
        D3D11_SUBRESOURCE_DATA subresource_data{};
        buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * mesh.vertices.size());
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        buffer_desc.CPUAccessFlags = 0;
        buffer_desc.MiscFlags = 0;
        buffer_desc.StructureByteStride = 0;
        subresource_data.pSysMem = mesh.vertices.data();
        subresource_data.SysMemPitch = 0;
        subresource_data.SysMemSlicePitch = 0;
        hr = device->CreateBuffer(&buffer_desc, &subresource_data,
            mesh.vertex_buffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        subresource_data.pSysMem = mesh.indices.data();
        hr = device->CreateBuffer(&buffer_desc, &subresource_data,
            mesh.index_buffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#if 0
        mesh.vertices.clear();
        mesh.indices.clear();
#endif
    }


    // マテリアルの中にあるテクスチャファイル名の数だけテクスチャを生成する
    for (std::unordered_map<uint64_t, material>::iterator iterator = materials.begin();
         iterator != materials.end(); ++iterator)
    {
       // Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
        for (size_t texture_index = 0; texture_index < 2; ++texture_index) {
            if (iterator->second.texture_filenames[texture_index].size() > 0)
            {
                // テクスチャのパスを解決する
                std::filesystem::path path(fbx_filename);
                path.replace_filename(iterator->second.texture_filenames[texture_index]);
                D3D11_TEXTURE2D_DESC texture2d_desc;
                // テクスチャを生成
                load_texture_from_file(device, path.c_str(),
                    iterator->second.shader_resource_views[texture_index].GetAddressOf(), &texture2d_desc);

            }
            else
            {
                // ダミーテクスチャを作成
                make_dummy_texture(device, iterator->second.shader_resource_views[texture_index].GetAddressOf(),
                    texture_index == 1 ? 0xFFFF7F7F : 0xFFFFFFFF, 16);

            }
        }
        // ファイル名でシェーダーリソースビューを登録
       // shader_resource_views[0].insert(std::make_pair(material.texture_filenames[0], shader_resource_view));

    }
    HRESULT hr = S_OK;
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
        D3D11_APPEND_ALIGNED_ELEMENT },
        { "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
        D3D11_APPEND_ALIGNED_ELEMENT },
    };

    create_vs_from_cso(device, "skinned_mesh_vs.cso", vertex_shader.ReleaseAndGetAddressOf(),
        input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    create_ps_from_cso(device, "skinned_mesh_ps.cso", pixel_shader.ReleaseAndGetAddressOf());

    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(constants);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}
bool skinned_mesh::append_animations(const char* animation_filename, float sampling_rate)
{

    FbxManager * fbx_manager{ FbxManager::Create() };
    FbxScene * fbx_scene{ FbxScene::Create(fbx_manager, "") };
    
    FbxImporter * fbx_importer{ FbxImporter::Create(fbx_manager, "") };
    bool import_status{ false };
    import_status = fbx_importer->Initialize(animation_filename);
    _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());
     import_status = fbx_importer->Import(fbx_scene);
     _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());
    
     fetch_animations(fbx_scene, animation_clips, sampling_rate);
    
     fbx_manager->Destroy();
    
     return true;

}
skinned_mesh::skinned_mesh(ID3D11Device * device, const char* fbx_filename, bool triangulate, float sampling_rate, float scale)
 {
    
     std::filesystem::path cereal_filename(fbx_filename);
     cereal_filename.replace_extension("cereal");
     if (std::filesystem::exists(cereal_filename.c_str()))
     {
         
         std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
         cereal::BinaryInputArchive deserialization(ifs);
         deserialization(scene_view, meshes, materials, animation_clips);
     }
     else 
     {
         FbxManager* fbx_manager{ FbxManager::Create() };
         FbxScene* fbx_scene{ FbxScene::Create(fbx_manager, "") };
         FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager, "") };
         bool import_status{ false };
         import_status = fbx_importer->Initialize(fbx_filename);
         _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

         import_status = fbx_importer->Import(fbx_scene);
         _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

         FbxGeometryConverter fbx_converter(fbx_manager);
         if (triangulate)
         {
             fbx_converter.Triangulate(fbx_scene, true/*replace*/, false/*legacy*/);
             fbx_converter.RemoveBadPolygonsFromMeshes(fbx_scene);
         }

         std::function<void(FbxNode*)> traverse{ [&](FbxNode* fbx_node) {
         scene::node& node{ scene_view.nodes.emplace_back() };
         node.attribute = fbx_node->GetNodeAttribute() ?
         fbx_node->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
         node.name = fbx_node->GetName();
         node.unique_id = fbx_node->GetUniqueID();


         node.parent_index = scene_view.indexof(fbx_node->GetParent() ?
         fbx_node->GetParent()->GetUniqueID() : 0);
        for (int child_index = 0; child_index < fbx_node->GetChildCount(); ++child_index)
         {
        traverse(fbx_node->GetChild(child_index));
        }
         } };
         traverse(fbx_scene->GetRootNode());

#if 1
         for (const scene::node& node : scene_view.nodes)
         {
             
             FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
             // Display node data in the output window as debug
             std::string node_name = fbx_node->GetName();
             uint64_t uid = fbx_node->GetUniqueID();
             uint64_t parent_uid = fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0;
             int32_t type = fbx_node->GetNodeAttribute() ? fbx_node->GetNodeAttribute()->GetAttributeType() : 0;

             std::stringstream debug_string;
             debug_string << node_name << ":" << uid << ":" << parent_uid << ":" << type << "¥n";
             OutputDebugStringA(debug_string.str().c_str());
         }
#endif
         fetch_meshes(fbx_scene, meshes);
         fetch_materials(fbx_scene, materials);
         sampling_rate = 0;
         fetch_animations(fbx_scene, animation_clips, sampling_rate);
         

         fbx_manager->Destroy();
        
         std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
         cereal::BinaryOutputArchive serialization(ofs);
         serialization(scene_view, meshes, materials, animation_clips);

     }
     DirectX::XMVECTOR VolumeMin = DirectX::XMVectorReplicate(FLT_MAX);
     DirectX::XMVECTOR VolumeMax = DirectX::XMVectorReplicate(-FLT_MAX);
     // 頂点データをワールド空間変換し、三角形データを作成
  
         for (const mesh& mesh : meshes)
         {
             
             //  UpdateTransform(mesh.default_global_transform);
              // Convert to model space.
            
             XMMATRIX concatenated_matrix{
                 XMLoadFloat4x4(&mesh.default_global_transform) *
                 XMLoadFloat4x4(&default_global_transform1) };
             // スケーリング行列の作成
             XMMATRIX scalingMatrix = XMMatrixScaling(scale, scale, scale);

             // スケーリング行列と既存の行列を掛け合わせる
             XMMATRIX finalMatrix = concatenated_matrix * scalingMatrix;

             // 結果をXMFLOAT4X4に保存（必要な場合）
             XMFLOAT4X4 finalMatrixFloat4x4;
             XMStoreFloat4x4(&finalMatrixFloat4x4, finalMatrix);
             for (size_t i = 0; i < mesh.indices.size(); i += 3)
             {
                 // 頂点データをワールド空間変換
                 uint32_t a = mesh.indices.at(i + 0);
                 uint32_t b = mesh.indices.at(i + 1);
                 uint32_t c = mesh.indices.at(i + 2);
                 DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&mesh.vertices.at(a).position);
                 DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&mesh.vertices.at(b).position);
                 DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&mesh.vertices.at(c).position);
                 A = DirectX::XMVector3Transform(A, finalMatrix);
                 B = DirectX::XMVector3Transform(B, finalMatrix);
                 C = DirectX::XMVector3Transform(C, finalMatrix);

                 // 法線ベクトルを算出
                 DirectX::XMVECTOR N = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(B, A), DirectX::XMVectorSubtract(C, A));
                 if (DirectX::XMVector3Equal(N, DirectX::XMVectorZero()))
                 {
                     // 面を構成できない場合は除外
                     continue;
                 }
                 N = DirectX::XMVector3Normalize(N);

                 // 三角形データを格納
                 CollisionMesh::Triangle triangle;
                 DirectX::XMStoreFloat3(&triangle.positions[0], A);
                 DirectX::XMStoreFloat3(&triangle.positions[1], B);
                 DirectX::XMStoreFloat3(&triangle.positions[2], C);
                 DirectX::XMStoreFloat3(&triangle.normal, N);
                 triangle.materialIndex = 1;
                 collisionMesh.triangles.emplace_back(triangle);
                 // モデル全体のAABBを計測
                 VolumeMin = DirectX::XMVectorMin(VolumeMin, A);
                 VolumeMin = DirectX::XMVectorMin(VolumeMin, B);
                 VolumeMin = DirectX::XMVectorMin(VolumeMin, C);
                 VolumeMax = DirectX::XMVectorMax(VolumeMax, A);
                 VolumeMax = DirectX::XMVectorMax(VolumeMax, B);
                 VolumeMax = DirectX::XMVectorMax(VolumeMax, C);

             }
         }

         // モデル全体のAABB
         DirectX::XMFLOAT3 volumeMin, volumeMax;
         DirectX::XMStoreFloat3(&volumeMin, VolumeMin);
         DirectX::XMStoreFloat3(&volumeMax, VolumeMax);
         SpatialPartitioning(volumeMin, volumeMax);
     
         create_com_objects(device, fbx_filename);

}
skinned_mesh::skinned_mesh(ID3D11Device* device, const char* fbx_filename, std::vector<std::string>& animation_filenames, bool triangulate, float sampling_rate, float scale)
{
    std::filesystem::path cereal_filename(fbx_filename);
    cereal_filename.replace_extension("cereal");
    if (std::filesystem::exists(cereal_filename.c_str()))
    {

        std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
        cereal::BinaryInputArchive deserialization(ifs);
        deserialization(scene_view, meshes, materials, animation_clips);
    }
    else
    {
        FbxManager* fbx_manager{ FbxManager::Create() };
        FbxScene* fbx_scene{ FbxScene::Create(fbx_manager, "") };
        FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager, "") };
        bool import_status{ false };
        import_status = fbx_importer->Initialize(fbx_filename);
        _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

        import_status = fbx_importer->Import(fbx_scene);
        _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

        FbxGeometryConverter fbx_converter(fbx_manager);
        if (triangulate)
        {
            fbx_converter.Triangulate(fbx_scene, true/*replace*/, false/*legacy*/);
            fbx_converter.RemoveBadPolygonsFromMeshes(fbx_scene);
        }

        std::function<void(FbxNode*)> traverse{ [&](FbxNode* fbx_node) {
        scene::node& node{ scene_view.nodes.emplace_back() };
        node.attribute = fbx_node->GetNodeAttribute() ?
        fbx_node->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
        node.name = fbx_node->GetName();
        node.unique_id = fbx_node->GetUniqueID();


        node.parent_index = scene_view.indexof(fbx_node->GetParent() ?
        fbx_node->GetParent()->GetUniqueID() : 0);
       for (int child_index = 0; child_index < fbx_node->GetChildCount(); ++child_index)
        {
       traverse(fbx_node->GetChild(child_index));
       }
        } };
        traverse(fbx_scene->GetRootNode());

#if 1
        for (const scene::node& node : scene_view.nodes)
        {
            FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
            // Display node data in the output window as debug
            std::string node_name = fbx_node->GetName();
            uint64_t uid = fbx_node->GetUniqueID();
            uint64_t parent_uid = fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0;
            int32_t type = fbx_node->GetNodeAttribute() ? fbx_node->GetNodeAttribute()->GetAttributeType() : 0;

            std::stringstream debug_string;
            debug_string << node_name << ":" << uid << ":" << parent_uid << ":" << type << "¥n";
            OutputDebugStringA(debug_string.str().c_str());
        }
#endif
        fetch_meshes(fbx_scene, meshes);
        fetch_materials(fbx_scene, materials);
        sampling_rate = 0;
        fetch_animations(fbx_scene, animation_clips, sampling_rate);
      

        fbx_manager->Destroy();
        for (const std::string animation_filename : animation_filenames)
        {
            append_animations(animation_filename.c_str(), sampling_rate);

        }
        std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
        cereal::BinaryOutputArchive serialization(ofs);
        serialization(scene_view, meshes, materials, animation_clips);

    }
    DirectX::XMVECTOR VolumeMin = DirectX::XMVectorReplicate(FLT_MAX);
    DirectX::XMVECTOR VolumeMax = DirectX::XMVectorReplicate(-FLT_MAX);
    // 頂点データをワールド空間変換し、三角形データを作成
    
        for (const mesh& mesh : meshes)
        {
            //  UpdateTransform(mesh.default_global_transform);

          //  UpdateTransform(mesh.default_global_transform);
              // Convert to model space.

            XMMATRIX concatenated_matrix{
                XMLoadFloat4x4(&mesh.default_global_transform) *
                XMLoadFloat4x4(&default_global_transform1) };
            // スケーリング行列の作成
            XMMATRIX scalingMatrix = XMMatrixScaling(scale, scale, scale);

            // スケーリング行列と既存の行列を掛け合わせる
            XMMATRIX finalMatrix = concatenated_matrix * scalingMatrix;

            // 結果をXMFLOAT4X4に保存（必要な場合）
            XMFLOAT4X4 finalMatrixFloat4x4;
            XMStoreFloat4x4(&finalMatrixFloat4x4, finalMatrix);
            for (size_t i = 0; i < mesh.indices.size(); i += 3)
            {
                // 頂点データをワールド空間変換
                uint32_t a = mesh.indices.at(i + 0);
                uint32_t b = mesh.indices.at(i + 1);
                uint32_t c = mesh.indices.at(i + 2);
                DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&mesh.vertices.at(a).position);
                DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&mesh.vertices.at(b).position);
                DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&mesh.vertices.at(c).position);
                A = DirectX::XMVector3Transform(A, finalMatrix);
                B = DirectX::XMVector3Transform(B, finalMatrix);
                C = DirectX::XMVector3Transform(C, finalMatrix);

                // 法線ベクトルを算出
                DirectX::XMVECTOR N = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(B, A), DirectX::XMVectorSubtract(C, A));
                if (DirectX::XMVector3Equal(N, DirectX::XMVectorZero()))
                {
                    // 面を構成できない場合は除外
                    continue;
                }
                N = DirectX::XMVector3Normalize(N);

                // 三角形データを格納
                CollisionMesh::Triangle triangle;
                DirectX::XMStoreFloat3(&triangle.positions[0], A);
                DirectX::XMStoreFloat3(&triangle.positions[1], B);
                DirectX::XMStoreFloat3(&triangle.positions[2], C);
                DirectX::XMStoreFloat3(&triangle.normal, N);
                triangle.materialIndex = 1;
                collisionMesh.triangles.emplace_back(triangle);
                // モデル全体のAABBを計測
                VolumeMin = DirectX::XMVectorMin(VolumeMin, A);
                VolumeMin = DirectX::XMVectorMin(VolumeMin, B);
                VolumeMin = DirectX::XMVectorMin(VolumeMin, C);
                VolumeMax = DirectX::XMVectorMax(VolumeMax, A);
                VolumeMax = DirectX::XMVectorMax(VolumeMax, B);
                VolumeMax = DirectX::XMVectorMax(VolumeMax, C);

            }
        }

        // モデル全体のAABB
        DirectX::XMFLOAT3 volumeMin, volumeMax;
        DirectX::XMStoreFloat3(&volumeMin, VolumeMin);
        DirectX::XMStoreFloat3(&volumeMax, VolumeMax);
        SpatialPartitioning(volumeMin, volumeMax);
    
    create_com_objects(device, fbx_filename);
}

/// <summary>
/// 描画処理
/// </summary>
/// <param name="immediate_context">デバイスコンテキスト</param>
/// <param name="world">ワールド行列</param>
/// <param name="material_color">マテリアルカラー</param>
void skinned_mesh::render(ID3D11DeviceContext* immediate_context,
    const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color, const animation::keyframe* keyframe, ID3D11PixelShader* replaced_pixel_shader)
{
    for (const mesh& mesh : meshes)
    {
        if (!Camera::Instance().MeshFrustumCulling(mesh))
            continue;

        uint32_t stride{ sizeof(vertex) };
        uint32_t offset{ 0 };
        immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
        immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        immediate_context->IASetInputLayout(input_layout.Get());

        immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);

        // TODO HUSK.11
        if (replaced_pixel_shader)
        {
            immediate_context->PSSetShader(replaced_pixel_shader, nullptr, 0);
        }
        else
        {
            immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
        }
        // モデル変換行列を計算
        DirectX::XMFLOAT4X4 model_world_transform;
        if (keyframe && keyframe->nodes.size() > 0)
        {
            const animation::keyframe::node& mesh_node{ keyframe->nodes.at(mesh.node_index) };
            XMStoreFloat4x4(&model_world_transform, XMLoadFloat4x4(&mesh_node.global_transform) * XMLoadFloat4x4(&world));
        }
        else
        {
            XMStoreFloat4x4(&model_world_transform, XMLoadFloat4x4(&mesh.default_global_transform) * XMLoadFloat4x4(&world));
        }
        constants data;

        // UNIT.29
        if (keyframe && keyframe->nodes.size() > 0)
        {
            const animation::keyframe::node& mesh_node{ keyframe->nodes.at(mesh.node_index) };
            XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh_node.global_transform) * XMLoadFloat4x4(&world));

            const size_t bone_count{ mesh.bind_pose.bones.size() };
            _ASSERT_EXPR(bone_count < MAX_BONES, L"The value of the 'bone_count' has exceeded MAX_BONES.");

            for (size_t bone_index = 0; bone_index < bone_count; ++bone_index)
            {
                const skeleton::bone& bone{ mesh.bind_pose.bones.at(bone_index) };
                const animation::keyframe::node& bone_node{ keyframe->nodes.at(bone.node_index) };
                XMStoreFloat4x4(&data.bone_transforms[bone_index],
                    XMLoadFloat4x4(&bone.offset_transform) *
                    XMLoadFloat4x4(&bone_node.global_transform) *
                    XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.default_global_transform))
                );
            }
        }
        else
        {
            XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh.default_global_transform) * XMLoadFloat4x4(&world));

            // Bind dummy bone transforms
            for (size_t bone_index = 0; bone_index < MAX_BONES; ++bone_index)
            {
                data.bone_transforms[bone_index] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
            }
        }

        for (const mesh::subset& subset : mesh.subsets)
        {
            const material& material{ materials.at(subset.material_unique_id) };

            XMStoreFloat4(&data.material_color, XMLoadFloat4(&material_color) * XMLoadFloat4(&material.Kd));
            immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
            immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

            immediate_context->PSSetShaderResources(0, 1, material.shader_resource_views[0].GetAddressOf());
            // UNIT.29
            immediate_context->PSSetShaderResources(1, 1, material.shader_resource_views[1].GetAddressOf());

            immediate_context->DrawIndexed(subset.index_count, subset.start_index_location, 0);
        }
    }
}
/// <summary>
/// メッシュ情報の取り出し
/// </summary>
/// <param name="fbx_scene">メッシュ情報の元データがあるノードの情報を持っているシーン</param>
/// <param name="meshes">取り出し後にメッシュ情報を設定するための出力用メッシュ</param>
void skinned_mesh::fetch_meshes(FbxScene* fbx_scene, std::vector<mesh>& meshes)
{
    // シーンの中にあるノードの情報をすべて検索
    for (const scene::node& node : scene_view.nodes)
    {
        // ノードの中からメッシュの属性を持っているノードをチェック
        if (node.attribute != FbxNodeAttribute::EType::eMesh)
        {
            continue;
        }

        // シーンから FbxNode を取得
        FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
        // メッシュ情報を取得
        FbxMesh* fbx_mesh{ fbx_node->GetMesh() };

        // メッシュを新規で作成し取り付け
        mesh& mesh{ meshes.emplace_back() };
        // メッシュの識別 ID の設定
        mesh.unique_id = fbx_mesh->GetNode()->GetUniqueID();
        // メッシュ名の設定
        mesh.name = fbx_mesh->GetNode()->GetName();
        // メッシュに対するノード ID の割り振り
        mesh.node_index = scene_view.indexof(mesh.unique_id);

           // メッシュのグローバル行列を取得し XMFLOAT4X4 に変換して代入
            mesh.default_global_transform = to_xmfloat4x4(fbx_mesh->GetNode()->EvaluateGlobalTransform());

            std::vector<bone_influences_per_control_point> bone_influences;
            fetch_bone_influences(fbx_mesh, bone_influences);
            fetch_skeleton(fbx_mesh, mesh.bind_pose);

        std::vector<mesh::subset>& subsets{ mesh.subsets };
           // マテリアル数を取得
           const int material_count{ fbx_mesh->GetNode()->GetMaterialCount() };
          /* if(material_count==0)
           {
               materials.emplace();
           }*/
           // サブセットの数をリサイズ
           subsets.resize(material_count > 0 ? material_count : 1);
        for (int material_index = 0; material_index < material_count; ++material_index)
             {
                   // マテリアル情報を取得
               const FbxSurfaceMaterial* fbx_material{ fbx_mesh->GetNode()->GetMaterial(material_index) };
            subsets.at(material_index).material_name = fbx_material->GetName();
            subsets.at(material_index).material_unique_id = fbx_material->GetUniqueID();
            
        }

        if (material_count > 0)
             {
             const int polygon_count{ fbx_mesh->GetPolygonCount() };
            for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
               {
                         // メッシュのポリゴンの番号からインデックス番号を取得しマテリアルのインデックスの番号として割り当てる
                 const int material_index{ fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) };
                          // サブセットに対して割り当てられたマテリアルのインデックスの番号にインデックスの数を頂点数分(3)増やしていく
                  subsets.at(material_index).index_count += 3;
               
            }

                  // インデックスの数からそれぞれの開始の数を計算して設定していく
                uint32_t offset{ 0 };
            for (mesh::subset& subset : subsets)
                 {
                subset.start_index_location = offset;
                offset += subset.index_count;
                subset.index_count = 0;
                
            }
            
        }
        
      

        const int polygon_count{ fbx_mesh->GetPolygonCount() };	// ポリゴン数
        mesh.vertices.resize(polygon_count * 3LL);				// 頂点座標数
        mesh.indices.resize(polygon_count * 3LL);				// 頂点インデックス数

        // uv 名の取得。後々テクスチャ座標の取得に利用。
        FbxStringList uv_names;
        fbx_mesh->GetUVSetNames(uv_names);

        // コントロールポイントの取得
        const FbxVector4* control_points{ fbx_mesh->GetControlPoints() };
        // ポリゴンの数だけ頂点データを取得
        for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
        {
             const int material_index{ material_count > 0 ?
            fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) : 0 };
            mesh::subset& subset{ subsets.at(material_index) };
             const uint32_t offset{ subset.start_index_location + subset.index_count };

            // 三角形の数分の頂点の情報を取得する
            for (int position_in_polygon = 0; position_in_polygon < 3; ++position_in_polygon)
            {
                // 利用するインデックスの配列を計算
                const int vertex_index{ polygon_index * 3 + position_in_polygon };

                // 頂点座標の取得
                vertex vertex;
                const int polygon_vertex{ fbx_mesh->GetPolygonVertex(polygon_index, position_in_polygon) };
                vertex.position.x = static_cast<float>(control_points[polygon_vertex][0]);
                vertex.position.y = static_cast<float>(control_points[polygon_vertex][1]);
                vertex.position.z = static_cast<float>(control_points[polygon_vertex][2]);

                const bone_influences_per_control_point& influences_per_control_point
                     { bone_influences.at(polygon_vertex) };
                for (size_t influence_index = 0; influence_index < influences_per_control_point.size();
                    ++influence_index)
                {
                    if (influence_index < MAX_BONE_INFLUENCES)
                    {
                        vertex.bone_weights[influence_index] =
                            influences_per_control_point.at(influence_index).bone_weight;
                        vertex.bone_indices[influence_index] =
                            influences_per_control_point.at(influence_index).bone_index;
                    }
                }
                {
                }
                // 法線の取得
                if (fbx_mesh->GetElementNormalCount() > 0)
                {
                    FbxVector4 normal;
                    fbx_mesh->GetPolygonVertexNormal(polygon_index, position_in_polygon, normal);
                    vertex.normal.x = static_cast<float>(normal[0]);
                    vertex.normal.y = static_cast<float>(normal[1]);
                    vertex.normal.z = static_cast<float>(normal[2]);
                }
                // テクスチャ座標の取得
                if (fbx_mesh->GetElementUVCount() > 0)
                {
                    FbxVector2 uv;
                    bool unmapped_uv;
                    fbx_mesh->GetPolygonVertexUV(polygon_index, position_in_polygon, uv_names[0], uv, unmapped_uv);
                    vertex.texcoord.x = static_cast<float>(uv[0]);
                    vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
                }
                if (fbx_mesh->GenerateTangentsData(0, false))
                {
                    const FbxGeometryElementTangent* tangent = fbx_mesh->GetElementTangent(0);
                    vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[0]);
                    vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[1]);
                    vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[2]);
                    vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[3]);
                }

                // 現在のインデックス番号部分に頂点データを設定
                mesh.vertices.at(vertex_index) = std::move(vertex);
                // 現在のインデックス番号を設定
               // mesh.indices.at(vertex_index) = vertex_index;
                mesh.indices.at(static_cast<size_t>(offset) + position_in_polygon) = vertex_index;
                subset.index_count++;
            }
        }
        for (const vertex& v : mesh.vertices)
        {
            mesh.bounding_box[0].x = std::min<float>(mesh.bounding_box[0].x, v.position.x);
            mesh.bounding_box[0].y = std::min<float>(mesh.bounding_box[0].y, v.position.y);
            mesh.bounding_box[0].z = std::min<float>(mesh.bounding_box[0].z, v.position.z);
            mesh.bounding_box[1].x = std::max<float>(mesh.bounding_box[1].x, v.position.x);
            mesh.bounding_box[1].y = std::max<float>(mesh.bounding_box[1].y, v.position.y);
            mesh.bounding_box[1].z = std::max<float>(mesh.bounding_box[1].z, v.position.z);
        }
    }
}
/// <summary>
/// マテリアル情報の取り出し
/// </summary>
/// <param name="fbx_scene">マテリアル情報の元データがあるノードの情報を持っているシーン</param>
/// <param name="materials">取り出し後にマテリアル情報を設定する為の出力用マテリアル</param>
void skinned_mesh::fetch_materials(FbxScene* fbx_scene, std::unordered_map<uint64_t, material>& materials)
{
    // シーンの中にあるノードの情報をすべて検索
    const size_t node_count{ scene_view.nodes.size() };
    for (size_t node_index = 0; node_index < node_count; ++node_index)
    {
        // ノードの情報１つ１つをチェック
        const scene::node& node{ scene_view.nodes.at(node_index) };
        // シーンから FbxNode を取得
        const FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };

        // ノードの中にあるマテリアルの情報をすべて検索
        const int material_count{ fbx_node->GetMaterialCount() };
        if (material_count == 0) 
        {

            materials.emplace();
        }
        for (int material_index = 0; material_index < material_count; ++material_index)
        {
            // マテリアルの情報１つ１つをチェック
            const FbxSurfaceMaterial* fbx_material{ fbx_node->GetMaterial(material_index) };

            material material;
            // マテリアル名の設定
            material.name = fbx_material->GetName();
            // マテリアルの識別 ID の設定
            material.unique_id = fbx_material->GetUniqueID();

            FbxProperty fbx_property;
            // ディフューズ(拡散反射光)の情報を取得
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
            // ディフューズがあったらディフューズの情報を設定していく
            if (fbx_property.IsValid())
            {
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
                material.Kd.x = static_cast<float>(color[0]);
                material.Kd.y = static_cast<float>(color[1]);
                material.Kd.z = static_cast<float>(color[2]);
                material.Kd.w = 1.0f;

                // テクスチャのファイル名を取得
                const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
                // 相対パス込みでのファイル名を設定する
                material.texture_filenames[0] =
                    fbx_texture ? fbx_texture->GetRelativeFileName() : "";
            }
            // 取得したマテリアルの情報を設定する
            materials.emplace(material.unique_id, std::move(material));
            
            // アンビエント(環境光)の情報を取得
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sNormalMap);
            //アンビエント設定
            if (fbx_property.IsValid())
            {
            
                const FbxFileTexture* file_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
                material.texture_filenames[1] = file_texture ? file_texture->GetRelativeFileName() : "";
            }
            // スペキュラ(鏡面反射光)の情報を取得


        }
    }
 #if 1
              
    
  
 #endif
}

// RAYCAST
#include "raycast.h"

// The coordinate system of all function arguments is world space.
bool skinned_mesh::raycast(const XMFLOAT4& position, const XMFLOAT4& direction, const XMFLOAT4X4& world_transform, XMFLOAT4& closest_point, DirectX::XMFLOAT3& intersected_normal,
    std::string& intersected_mesh, std::string& intersected_material)
{
    float closest_distance{ FLT_MAX };

    for (const mesh& mesh : meshes)
    {
        XMFLOAT4 ray_position = position;
        XMFLOAT4 ray_direction = direction;
        ray_direction.w = 0;
       
        // Convert to model space.
        XMMATRIX concatenated_matrix{
            XMLoadFloat4x4(&mesh.default_global_transform) *
            XMLoadFloat4x4(&world_transform) };
        XMMATRIX inverse_concatenated_matrix{ XMMatrixInverse(nullptr, concatenated_matrix) };
        XMStoreFloat4(&ray_position, XMVector3TransformCoord(XMLoadFloat4(&ray_position), inverse_concatenated_matrix));
        XMStoreFloat4(&ray_direction, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat4(&ray_direction), inverse_concatenated_matrix)));
      

     

#if 0
        const float* min{ reinterpret_cast<const float*>(&mesh.bounding_box[0]) };
        const float* max{ reinterpret_cast<const float*>(&mesh.bounding_box[1]) };
        if (!intersect_ray_aabb(reinterpret_cast<const float*>(&ray_position), reinterpret_cast<const float*>(&ray_direction), min, max))
        {
            continue;
        }
#endif

        float distance{ 1.0e+7f };
        XMFLOAT4 intersection{};
        const float* vertex_positions{ reinterpret_cast<const float*>(mesh.vertices.data()) };
        const uint32_t* indices{ mesh.indices.data() };
        const size_t index_count{ mesh.indices.size() };

        const int intersected_triangle_index{ intersect_ray_triangles(vertex_positions, 0L, sizeof(vertex), indices, index_count, ray_position, ray_direction, intersection, distance) };
        if (intersected_triangle_index >= 0)
        {
            if (closest_distance > distance)
            {
                closest_distance = distance;
                // Convert model space to original space.
                XMStoreFloat4(&closest_point, XMVector3TransformCoord(XMLoadFloat4(&intersection), concatenated_matrix));
                intersected_mesh = mesh.name;
                intersected_material = mesh.find_subset(intersected_triangle_index * 3)->material_name;
                intersected_normal = mesh.vertices.at(indices[intersected_triangle_index + 0]).normal;
            }
        }
    }
    return closest_distance < FLT_MAX;
}
// The coordinate system of all function arguments is world space.
bool skinned_mesh::MoveFloorRaycast(const XMFLOAT4& position, const XMFLOAT4& direction, const XMFLOAT4X4& world_transform, XMFLOAT4& closest_point,XMFLOAT3& intersected_normal,
    std::string& intersected_mesh, std::string& intersected_material)
{
    float closest_distance{ FLT_MAX };

    for (const mesh& mesh : dummyMesh)
    {
        XMFLOAT4 ray_position = position;
        XMFLOAT4 ray_direction = direction;
        ray_direction.w = 0;

        // Convert to model space.
        XMMATRIX concatenated_matrix{
            XMLoadFloat4x4(&mesh.default_global_transform) 
            };
        XMMATRIX inverse_concatenated_matrix{ XMMatrixInverse(nullptr, concatenated_matrix) };
        XMStoreFloat4(&ray_position, XMVector3TransformCoord(XMLoadFloat4(&ray_position), inverse_concatenated_matrix));
        XMStoreFloat4(&ray_direction, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat4(&ray_direction), inverse_concatenated_matrix)));




#if 0
        const float* min{ reinterpret_cast<const float*>(&mesh.bounding_box[0]) };
        const float* max{ reinterpret_cast<const float*>(&mesh.bounding_box[1]) };
        if (!intersect_ray_aabb(reinterpret_cast<const float*>(&ray_position), reinterpret_cast<const float*>(&ray_direction), min, max))
        {
            continue;
        }
#endif

        float distance{ 1.0e+7f };
        XMFLOAT4 intersection{};
        const float* vertex_positions{ reinterpret_cast<const float*>(mesh.vertices.data()) };
        const uint32_t* indices{ mesh.indices.data() };
        const size_t index_count{ mesh.indices.size() };

        const int intersected_triangle_index{ intersect_ray_triangles(vertex_positions, 0L, sizeof(vertex), indices, index_count, ray_position, ray_direction, intersection, distance) };
        if (intersected_triangle_index >= 0)
        {
            if (closest_distance > distance)
            {
                closest_distance = distance;
                // Convert model space to original space.
                XMStoreFloat4(&closest_point, XMVector3TransformCoord(XMLoadFloat4(&intersection), concatenated_matrix));
                intersected_mesh = mesh.name;
                intersected_material = mesh.find_subset(intersected_triangle_index * 3)->material_name;
                intersected_normal = mesh.vertices.at(indices[intersected_triangle_index + 0]).normal;
            }
        }
    }
    return closest_distance < FLT_MAX;
}
void skinned_mesh::SpatialPartitioning(const DirectX::XMFLOAT3& volumeMin, const DirectX::XMFLOAT3& volumeMax)
{
    // ボリュームの幅、高さ、奥行きを計算
    const float minX = volumeMin.x;
    const float minY = volumeMin.y;
    const float minZ = volumeMin.z;
    const float volumeWidth = volumeMax.x - volumeMin.x;
    const float volumeHeight = volumeMax.y - volumeMin.y;
    const float volumeDepth = volumeMax.z - volumeMin.z;

    // セルのサイズを計算
    const float cellSizeX = volumeWidth / 12;
    const float cellSizeY = volumeHeight / 2; // 適切な高さを設定する
    const float cellSizeZ = volumeDepth / 12;

    // エリアを分割して追加
    for (float areaMinX = minX; areaMinX < volumeMax.x; areaMinX += cellSizeX)
    {
        for (float areaMinY = minY; areaMinY < volumeMax.y; areaMinY += cellSizeY)
        {
            for (float areaMinZ = minZ; areaMinZ < volumeMax.z; areaMinZ += cellSizeZ)
            {
                // エリアの最大座標を計算
                float areaMaxX = min(areaMinX + cellSizeX, volumeMax.x);
                float areaMaxY = min(areaMinY + cellSizeY, volumeMax.y);
                float areaMaxZ = min(areaMinZ + cellSizeZ, volumeMax.z);

                // エリアのバウンディングボックスを作成
                DirectX::BoundingBox boundingBox;
                boundingBox.Center = DirectX::XMFLOAT3(
                    areaMinX + (areaMaxX - areaMinX) * 0.5f,
                    areaMinY + (areaMaxY - areaMinY) * 0.5f,
                    areaMinZ + (areaMaxZ - areaMinZ) * 0.5f
                );
                boundingBox.Extents = DirectX::XMFLOAT3(
                    (areaMaxX - areaMinX) * 0.5f,
                    (areaMaxY - areaMinY) * 0.5f, // 高さの適切な値を設定する必要があります
                    (areaMaxZ - areaMinZ) * 0.5f
                );

                // エリアの三角形インデックスを取得
                std::vector<int> triangleIndices;
                for (size_t i = 0; i < collisionMesh.triangles.size(); ++i)
                {
                    const auto& triangle = collisionMesh.triangles[i];
                    for (const auto& vertex : triangle.positions)
                    {
                        if (vertex.x >= areaMinX && vertex.x <= areaMaxX &&
                            vertex.y >= areaMinY && vertex.y <= areaMaxY &&
                            vertex.z >= areaMinZ && vertex.z <= areaMaxZ)
                        {
                            // 三角形がエリア内にある場合、そのインデックスを追加
                            triangleIndices.push_back(static_cast<int>(i));
                            break;
                        }
                    }
                }

                // エリアを追加
                CollisionMesh::Area area;
                area.boundingBox = boundingBox;
                area.triangleIndices = std::move(triangleIndices);
                collisionMesh.areas.push_back(std::move(area));
            }
        }
    }
}
//bool skinned_mesh::SpatialPartitionedRaycast(
//    const DirectX::XMFLOAT3& start,
//    const DirectX::XMFLOAT3& end,
//    DirectX::XMFLOAT3& hitPosition,
//    DirectX::XMFLOAT3& hitNormal)
//{
//    bool hit = false;
//    bool hi1 = false;
//
//    // レイの始点と方向を計算
//    DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
//    DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
//    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);
//    DirectX::XMVECTOR Direction = DirectX::XMVector3Normalize(Vec);
//    float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(Vec));
//
//    // 空間分割せず、通常のレイキャストを実行
//    if (!spaceDivision)
//    {
//        for (const CollisionMesh::Triangle& triangle : collisionMesh.triangles)
//        {
//            DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&triangle.positions[0]);
//            DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&triangle.positions[1]);
//            DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&triangle.positions[2]);
//
//            float dist = distance;
//            if (DirectX::TriangleTests::Intersects(Start, Direction, A, B, C, dist))
//            {
//                if (distance < dist) continue;
//                distance = dist;
//                hitNormal = triangle.normal;
//                hit = true;
//            }
//        }
//    }
//    // 空間分割を利用してレイキャストを実行
//    else
//    {
//        for (const CollisionMesh::Area& area : collisionMesh.areas)
//        {
//            // レイとエリアの境界ボックスの交差判定を行う
//            float distanceToArea = distance;
//            if (area.boundingBox.Intersects(Start, Direction, distanceToArea))
//            {
//                // レイとエリアの境界ボックスが交差している場合、エリアに含まれる三角形に対してのみレイキャストを行う
//                for (int index : area.triangleIndices)
//                {
//                    const CollisionMesh::Triangle& triangle = collisionMesh.triangles[index];
//                    DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&triangle.positions[0]);
//                    DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&triangle.positions[1]);
//                    DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&triangle.positions[2]);
//
//                    float dist = distance;
//                    if (DirectX::TriangleTests::Intersects(Start, Direction, A, B, C, dist))
//                    {
//                        if (distance < dist) continue;
//                        distance = dist;
//                        hitNormal = triangle.normal;
//                        hit = true;
//                        hi1 = hit;
//                    }
//                }
//            }
//        }
//    }
//
//    if (hit)
//    {
//        // ヒット位置を計算
//        DirectX::XMVECTOR HitPosition = DirectX::XMVectorAdd(Start, DirectX::XMVectorScale(Direction, distance));
//        DirectX::XMStoreFloat3(&hitPosition, HitPosition);
//    }
//
//    return hit;
//}

bool skinned_mesh::SpatialPartitionedRaycast(const DirectX::XMFLOAT4& start, const DirectX::XMFLOAT4& direction, DirectX::XMFLOAT4& hitPosition, DirectX::XMFLOAT3& hitNormal)
{
    bool hit = false;
    bool hi1 = false;

    // レイの始点と方向を計算
    DirectX::XMVECTOR Start = DirectX::XMLoadFloat4(&start);
    DirectX::XMVECTOR End = DirectX::XMLoadFloat4(&direction);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);
    DirectX::XMVECTOR Direction = DirectX::XMVector3Normalize(Vec);
    float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(Vec));

    // 空間分割せず、通常のレイキャストを実行
    if (!spaceDivision)
    {
        for (const CollisionMesh::Triangle& triangle : collisionMesh.triangles)
        {
            DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&triangle.positions[0]);
            DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&triangle.positions[1]);
            DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&triangle.positions[2]);

            float dist = distance;
            if (DirectX::TriangleTests::Intersects(Start, Direction, A, B, C, dist))
            {
                if (distance < dist) continue;
                distance = dist;
                hitNormal = triangle.normal;
                hit = true;
            }
        }
    }
    // 空間分割を利用してレイキャストを実行
    else
    {
        for (const CollisionMesh::Area& area : collisionMesh.areas)
        {
            // レイとエリアの境界ボックスの交差判定を行う
            float distanceToArea = distance;
            if (area.boundingBox.Intersects(Start, Direction, distanceToArea))
            {
                // レイとエリアの境界ボックスが交差している場合、エリアに含まれる三角形に対してのみレイキャストを行う
                for (int index : area.triangleIndices)
                {
                    const CollisionMesh::Triangle& triangle = collisionMesh.triangles[index];
                    DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&triangle.positions[0]);
                    DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&triangle.positions[1]);
                    DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&triangle.positions[2]);

                    float dist = distance;
                    if (DirectX::TriangleTests::Intersects(Start, Direction, A, B, C, dist))
                    {
                        if (distance < dist) continue;
                        distance = dist;
                        hitNormal = triangle.normal;
                        
                        hit = true;
                        hi1 = hit;
                    }
                }
            }
        }
    }

    if (hit)
    {
        // ヒット位置を計算
        DirectX::XMVECTOR HitPosition = DirectX::XMVectorAdd(Start, DirectX::XMVectorScale(Direction, distance));
        DirectX::XMStoreFloat4(&hitPosition, HitPosition);
    }

    return hit;
}