//
// Project for course Computer Graphics (PGR)
// Faculty of Information Technology, Brno University of Technology
//
// Authors: Petr Dobes, xdobes14@stud.fit.vutbr.cz
//          Jaromir Deak, xdeakj00@stud.fit.vutbr.cz
//          Petr Huf, xhufpe00@stud.fit.vutbr.cz
// Date: December 2014
//

#ifndef SRC_MODEL_H_
#define SRC_MODEL_H_

#pragma once

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

// GL Includes
//#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "pgr.h"

//#include <SOIL.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "model_shader.h"

#include "mesh.h"

#include "bounding_box.h"
#include "bubble.h"
#include "water.h"

GLint TextureFromFile(const char* path, string directory);


class Model
{
public:

    /*  Functions   */
    // Constructor, expects a filepath to a 3D model.
    Model(const GLchar* path);

    ~Model();

    // Draws the model, and thus all its meshes.
    void Draw(ModelShader* shader);

    // Draws bounding box.
    void DrawBox(ModelShader* shader, float pickingColor);

    // Set scale of model.
    void setScale(float scale);

    // Get scale of model.
    glm::vec3 getScaleVector();

    // Set model position in 3D area.
    void setPosition(float x, float y, float z);

    // Get posion of model.
    glm::vec3 getPositionVector();

    // Set model rotation.
    void setRotation(float x, float y, float z); //TODO implement rotation in mesh.cpp Draw()

    // Get rotation of model.
    glm::vec3 getRotationVector();

    // Translate model by distances in all axis
    void translate(float dx, float dy, float dz);

    // Rotate model by specified angle.
    void rotate(float pitch, float yaw, float roll);

    // Set swinging mode.
    void setSwing(bool swing);

    // Translate model by distances specified in direction vector.
    void moveAhead();

    // Print bounds of bounding box.
    void printBoundingBoxBounds();

    // Get point coordinates of model bounding box
    glm::vec4 BoxVertA();

    glm::vec4 BoxVertB();

    glm::vec4 BoxVertC();

    glm::vec4 BoxVertD();

    glm::vec4 BoxVertE();

    glm::vec4 BoxVertF();

    glm::vec4 BoxVertG();

    glm::vec4 BoxVertH();

    // Fire missile
    void fire(float x, float y, float z);

    // Missile hit action
    void hit();

    // Set model state to destroyed/non-destroyed
    void setDestroyed(bool destroyed);

    // True if model is destroyed
    bool isDestroyed();

    // Set model visibility
    void setVisibility(bool visible);

    // Set model to cursor position
    void setCursorPosition(float x, float y, float z);

private:
    /*  Model Data  */
    vector<Mesh> meshes;
    string directory;
    BoundingBox boundingBox;

    glm::vec3 scale;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 direction;

    // submarine swinging
    bool swing;
    float deviance;
    float step;

    // destroyed
    bool destroyed;

    // visible
    bool visible;

    /*  Functions   */
    // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string path);

    // Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    // Checks all material textures of a given type and loads the textures if they're not loaded yet.
    // The required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName); //TODO vratit material a texturovani

    // Generate bubbles behind the model
    void generateBubble();

    // Get max of four float numbers
    float max(float a, float b, float c, float d);

    // Get min of four float numbers
    float min(float a, float b, float c, float d);
};

#endif /* SRC_MODEL_H_ */
