#include "Model.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Model::Model(const std::string& path) {
    loadModel(path);
}

void Model::loadModel(const std::string& path) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |           // Триангуляция
        aiProcess_GenSmoothNormals |      // Генерация нормалей
        aiProcess_FlipUVs |               // Переворот UV
        aiProcess_CalcTangentSpace |      // Тангенты для normal mapping
        aiProcess_JoinIdenticalVertices | // Оптимизация вершин
        aiProcess_OptimizeMeshes          // Оптимизация мешей
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
        || !scene->mRootNode) {
        std::cerr << "Assimp Error: " << importer.GetErrorString()
            << std::endl;
        return;
    }

    // Извлекаем директорию для текстур
    directory = path.substr(0, path.find_last_of('/'));
    if (directory == path) {
        directory = path.substr(0, path.find_last_of('\\'));
    }

    std::cout << "Loading model: " << path << std::endl;
    std::cout << "Meshes count: " << scene->mNumMeshes << std::endl;

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    // Обрабатываем все меши узла
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    // Рекурсивно обрабатываем дочерние узлы
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Обработка вершин
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        // Позиция
        vertex.position = glm::vec3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        );

        // Нормаль
        if (mesh->HasNormals()) {
            vertex.normal = glm::vec3(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            );
        }

        // Текстурные координаты
        if (mesh->mTextureCoords[0]) {
            vertex.texCoords = glm::vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            );

            // Тангенты
            if (mesh->mTangents) {
                vertex.tangent = glm::vec3(
                    mesh->mTangents[i].x,
                    mesh->mTangents[i].y,
                    mesh->mTangents[i].z
                );
            }
            if (mesh->mBitangents) {
                vertex.bitangent = glm::vec3(
                    mesh->mBitangents[i].x,
                    mesh->mBitangents[i].y,
                    mesh->mBitangents[i].z
                );
            }
        }
        else {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // Обработка индексов
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Обработка материалов
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // Diffuse maps
        auto diffuseMaps = loadMaterialTextures(material,
            aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(),
            diffuseMaps.begin(), diffuseMaps.end());

        // Specular maps
        auto specularMaps = loadMaterialTextures(material,
            aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(),
            specularMaps.begin(), specularMaps.end());

        // Normal maps
        auto normalMaps = loadMaterialTextures(material,
            aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(),
            normalMaps.begin(), normalMaps.end());
    }

    std::cout << "  Mesh loaded: " << vertices.size() << " vertices, "
        << indices.size() / 3 << " triangles" << std::endl;

    return Mesh(vertices, indices, textures);
}

//
std::vector<Texture> Model::loadMaterialTextures(
    aiMaterial* mat,
    aiTextureType type,
    const std::string& typeName)
{
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::string texPath = directory + "/" + str.C_Str();

        // Проверяем, загружена ли уже текстура
        if (loadedTextures.find(texPath) != loadedTextures.end()) {
            textures.push_back(loadedTextures[texPath]);
        }
        else {
            Texture texture;
            texture.id = loadTextureFromFile(texPath);
            texture.type = typeName;
            texture.path = texPath;
            textures.push_back(texture);
            loadedTextures[texPath] = texture;
        }
    }

    return textures;
}

GLuint Model::loadTextureFromFile(const std::string& path) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path.c_str(),
        &width, &height, &nrComponents, 0);

    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
            0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
            GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        std::cout << "  Texture loaded: " << path << std::endl;
    }
    else {
        std::cerr << "  Failed to load texture: " << path << std::endl;
    }

    stbi_image_free(data);
    return textureID;
}

void Model::draw(Shader& shader) {
    shader.setMat4("model", getModelMatrix());

    for (auto& mesh : meshes) {
        mesh.draw(shader);
    }
}

glm::mat4 Model::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x),
        glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(rotation.y),
        glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rotation.z),
        glm::vec3(0, 0, 1));
    model = glm::scale(model, scale);
    return model;
}

void Model::cleanup() {
    for (auto& mesh : meshes) {
        mesh.cleanup();
    }
}