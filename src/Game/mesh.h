//
// Project for course Computer Graphics (PGR)
// Faculty of Information Technology, Brno University of Technology
//
// Authors: Petr Dobes, xdobes14@stud.fit.vutbr.cz
//          Jaromir Deak, xdeakj00@stud.fit.vutbr.cz
//          Petr Huf, xhufpe00@stud.fit.vutbr.cz
// Date: December 2014
//

#ifndef SRC_MESH_H_
#define SRC_MESH_H_

#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

// GL Includes
//#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#ifdef WIN
#include <glm/gtc/matrix_projection.hpp>
#endif
#include <glm/gtc/type_ptr.hpp>
#include "pgr.h"

// Assimp includes
#include <assimp/cimport.h> // Plain-C interface
#include <assimp/scene.h> // Output data structure
#include <assimp/postprocess.h> // Post processing flags

#include "model_shader.h"

using namespace std;

struct Vertex {
    // Position
    glm::vec3 Position;
    // Normal
    glm::vec3 Normal;
    // TexCoords
    glm::vec2 TexCoords;
};

struct Texture {
    GLuint id;
    string type;
    aiString path;
};

class Mesh
{
public:
    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;

    /*  Functions  */
    // Constructor
    Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures);

    virtual ~Mesh();

    // Render the mesh
    void Draw(ModelShader* shader, glm::vec3 positionVector, glm::vec3 scaleVector, glm::vec3 rotationVector, bool destroyed);  //TODO vratit texturu a vyresit shader

private:
    /*  Render data  */
    GLuint VAO, VBO, EBO;

    //Model model;

    /*  Functions    */
    // Initializes all the buffer objects/arrays
    void setupMesh();
};

#endif /* SRC_MESH_H_ */
