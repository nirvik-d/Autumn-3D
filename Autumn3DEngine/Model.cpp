#include "Model.h"

Model::Model(const std::string& modelPath, bool gamma) : m_GammaCorrection(gamma) 
{
    try 
    {
        LoadModel(modelPath);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Model initialization failed: " << e.what() << std::endl;
        throw;
    }
}

void Model::LoadModel(const std::string& modelPath)
{
    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    if (!loader.LoadBinaryFromFile(&gltfModel, &err, &warn, modelPath)) 
    {
        throw std::runtime_error("Failed to load GLB file: " + err);
    }

    m_Directory = modelPath.substr(0, modelPath.find_last_of('/'));

    try
    {
        ProcessNode(gltfModel);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error processing model nodes: " << e.what() << std::endl;
        throw;
    }
}

void Model::ProcessNode(const tinygltf::Model& gltfModel) {
    for (const auto& node : gltfModel.nodes)
    {
        if (node.mesh >= 0)
        {
            const tinygltf::Mesh& mesh = gltfModel.meshes[node.mesh];
            m_Meshes.emplace_back(ProcessMesh(mesh, gltfModel));
        }
    }
}

std::shared_ptr<Model::Mesh> Model::ProcessMesh(const tinygltf::Mesh& gltfMesh, const tinygltf::Model& gltfModel) 
{
    std::vector<Model::Mesh::Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<tinygltf::Image> textureImages;

    // Iterate over the primitives in the mesh
    for (const auto& primitive : gltfMesh.primitives)
    {
        // Process vertex positions
        const auto& positionsAccessor = gltfModel.accessors[primitive.attributes.at("POSITION")];
        const auto& bufferView = gltfModel.bufferViews[positionsAccessor.bufferView];
        const auto& buffer = gltfModel.buffers[bufferView.buffer];
        const float* positions = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + positionsAccessor.byteOffset]);

        // Iterate over each vertex
        for (size_t i = 0; i < positionsAccessor.count; ++i) 
        {
            Model::Mesh::Vertex vertex;
            vertex.m_Position = glm::vec3(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]);

            // Process normals
            if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
            {
                const auto& normalAccessor = gltfModel.accessors[primitive.attributes.at("NORMAL")];
                const auto& normalBufferView = gltfModel.bufferViews[normalAccessor.bufferView];
                const auto& normalBuffer = gltfModel.buffers[normalBufferView.buffer];
                const float* normals = reinterpret_cast<const float*>(&normalBuffer.data[normalBufferView.byteOffset + normalAccessor.byteOffset]);
                vertex.m_Normal = glm::vec3(normals[i * 3 + 0], normals[i * 3 + 1], normals[i * 3 + 2]);
            }

            // Process texture coordinates
            if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
            {
                const auto& texcoordAccessor = gltfModel.accessors[primitive.attributes.at("TEXCOORD_0")];
                const auto& texcoordBufferView = gltfModel.bufferViews[texcoordAccessor.bufferView];
                const auto& texcoordBuffer = gltfModel.buffers[texcoordBufferView.buffer];
                const float* texcoords = reinterpret_cast<const float*>(&texcoordBuffer.data[texcoordBufferView.byteOffset + texcoordAccessor.byteOffset]);
                vertex.m_TexCoords = glm::vec2(texcoords[i * 2 + 0], texcoords[i * 2 + 1]);
            }

            // Process tangents
            if (primitive.attributes.find("TANGENT") != primitive.attributes.end()) 
            {
                const auto& tangentAccessor = gltfModel.accessors[primitive.attributes.at("TANGENT")];
                const auto& tangentBufferView = gltfModel.bufferViews[tangentAccessor.bufferView];
                const auto& tangentBuffer = gltfModel.buffers[tangentBufferView.buffer];
                const float* tangents = reinterpret_cast<const float*>(&tangentBuffer.data[tangentBufferView.byteOffset + tangentAccessor.byteOffset]);
                vertex.m_Tangent = glm::vec3(tangents[i * 3 + 0], tangents[i * 3 + 1], tangents[i * 3 + 2]);
            }

            // Process bitangents (optional, not always available in glTF)
            if (primitive.attributes.find("BITANGENT") != primitive.attributes.end())
            {
                const auto& bitangentAccessor = gltfModel.accessors[primitive.attributes.at("BITANGENT")];
                const auto& bitangentBufferView = gltfModel.bufferViews[bitangentAccessor.bufferView];
                const auto& bitangentBuffer = gltfModel.buffers[bitangentBufferView.buffer];
                const float* bitangents = reinterpret_cast<const float*>(&bitangentBuffer.data[bitangentBufferView.byteOffset + bitangentAccessor.byteOffset]);
                vertex.m_Bitangent = glm::vec3(bitangents[i * 3 + 0], bitangents[i * 3 + 1], bitangents[i * 3 + 2]);
            }

            // Process bone weights and IDs (for skinned models)
            if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end() &&
                primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end())
            {
                const auto& jointsAccessor = gltfModel.accessors[primitive.attributes.at("JOINTS_0")];
                const auto& weightsAccessor = gltfModel.accessors[primitive.attributes.at("WEIGHTS_0")];
                const auto& jointsBufferView = gltfModel.bufferViews[jointsAccessor.bufferView];
                const auto& jointsBuffer = gltfModel.buffers[jointsBufferView.buffer];
                const unsigned short* joints = reinterpret_cast<const unsigned short*>(&jointsBuffer.data[jointsBufferView.byteOffset + jointsAccessor.byteOffset]);
                const float* weights = reinterpret_cast<const float*>(&gltfModel.buffers[gltfModel.bufferViews[weightsAccessor.bufferView].buffer].data[weightsAccessor.byteOffset]);

                // For each vertex, assign bone IDs and weights
                for (int j = 0; j < MAX_BONE_INFLUENCE; ++j) 
                {
                    vertex.m_BoneIDs[j] = (j < jointsAccessor.count) ? joints[i * MAX_BONE_INFLUENCE + j] : 0;
                    vertex.m_Weights[j] = (j < weightsAccessor.count) ? weights[i * MAX_BONE_INFLUENCE + j] : 0.0f;
                }
            }

            vertices.push_back(vertex);
        }

        // Process indices (handle if indices exist)
        if (primitive.indices >= 0)
        {
            const auto& indicesAccessor = gltfModel.accessors[primitive.indices];
            const auto& indicesBufferView = gltfModel.bufferViews[indicesAccessor.bufferView];
            const auto& indicesBuffer = gltfModel.buffers[indicesBufferView.buffer];
            const unsigned int* indicesData = reinterpret_cast<const unsigned int*>(&indicesBuffer.data[indicesBufferView.byteOffset + indicesAccessor.byteOffset]);
            indices.insert(indices.end(), indicesData, indicesData + indicesAccessor.count);
        }

        // Handle materials (if they exist)
        if (primitive.material >= 0) 
        {
            const auto& material = gltfModel.materials[primitive.material];
            if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
            {
                const auto& textureInfo = material.pbrMetallicRoughness.baseColorTexture;
                const auto& image = gltfModel.images[gltfModel.textures[textureInfo.index].source];
                textureImages.push_back(image);
            }
        }
    }

    return std::make_shared<Mesh>(vertices, indices, textureImages);
}

Model::Mesh::Mesh(const std::vector<Model::Mesh::Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<tinygltf::Image>& textureImages) :
    m_VAO(0), m_VBO(0), m_EBO(0)
{
    m_Vertices = vertices;
    m_Indices = indices;
    m_TextureImages = textureImages;
}
