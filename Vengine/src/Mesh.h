#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

struct Texture {
    GLuint id;
    std::string type;     // diffuse, specular, normal
    std::string path;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices,
        std::vector<unsigned int> indices,
        std::vector<Texture> textures);

    void draw(class Shader& shader) const;
    void cleanup();

private:
    GLuint VAO, VBO, EBO;

    void setupMesh();
};