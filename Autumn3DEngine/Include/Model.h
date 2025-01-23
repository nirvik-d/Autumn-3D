#ifndef MODEL_H
#define MODEL_H

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "tiny_gltf.h"
#include "glm/glm.hpp"

#define MAX_BONE_INFLUENCE 4

using namespace std;

class Model
{
public:
    class Mesh
    {
    public:
        struct Vertex
        {
            glm::vec3 m_Position;    // position
            glm::vec3 m_Normal;      // normal
            glm::vec2 m_TexCoords;   // texture coordinates
            glm::vec3 m_Tangent;     // tangent
            glm::vec3 m_Bitangent;   // bitangent
            int m_BoneIDs[MAX_BONE_INFLUENCE]; // bone influences
            float m_Weights[MAX_BONE_INFLUENCE]; // bone weights
        };

        struct Texture
        {
            unsigned int m_TextureID;
            string m_TextureType;
        };

        // Mesh data
        vector<Vertex> m_Vertices;
        vector<unsigned int> m_Indices;
        vector<tinygltf::Image> m_TextureImages;
        vector<std::shared_ptr<Texture>> m_TexturesLoaded; // stores all the textures loaded so far
        unsigned int m_VAO, m_VBO, m_EBO;

        Mesh(const vector<Vertex>& vertices, const vector<unsigned int>& indices, const vector<tinygltf::Image>& textureImages);
        virtual ~Mesh() {}
    };

    // Model data
    vector<std::shared_ptr<Mesh>> m_Meshes;
    string m_Directory;
    bool m_GammaCorrection;

    Model(string const& modelPath, bool gamma = false);
    virtual ~Model() {}

private:
    // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void LoadModel(string const& modelPath);

    // Processes a node in a recursive fashion. Processes each mesh located at the node and repeats this process on its children nodes.
    void ProcessNode(const tinygltf::Model& gltfModel);

    // Processes a mesh and returns a Mesh object.
    std::shared_ptr<Mesh> ProcessMesh(const tinygltf::Mesh& gltfMesh, const tinygltf::Model& gltfModel);    
};

#endif
