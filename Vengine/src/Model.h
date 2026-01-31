#pragma once
#include "Mesh.h"
#include "Shader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>
#include <unordered_map>

class Model {
public:
    Model(const std::string& path);
    void draw(Shader& shader);
    void cleanup();

    // Трансформации
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    glm::mat4 getModelMatrix() const;

private:
    std::vector<Mesh> meshes;
    std::string directory;
    std::unordered_map<std::string, Texture> loadedTextures;

    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(
        aiMaterial* mat,
        aiTextureType type,
        const std::string& typeName
    );
    GLuint loadTextureFromFile(const std::string& path);
};