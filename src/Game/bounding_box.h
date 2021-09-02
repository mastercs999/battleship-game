//
// Project for course Computer Graphics (PGR)
// Faculty of Information Technology, Brno University of Technology
//
// Authors: Petr Dobes, xdobes14@stud.fit.vutbr.cz
//          Jaromir Deak, xdeakj00@stud.fit.vutbr.cz
//          Petr Huf, xhufpe00@stud.fit.vutbr.cz
// Date: December 2014
//

#ifndef SRC_BOUNDING_BOX_H_
#define SRC_BOUNDING_BOX_H_

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
#include "model_shader.h"

using namespace std;

// Enumeration of box vertices
enum{
    A, B, C, D, E, F, G, H
};

// Class provide model bounding box.
class BoundingBox
{
public:
  vector<glm::vec4> vertices;
  vector<GLuint> indices;

  // Constructor
  BoundingBox();
  // Destructor
  ~BoundingBox();

  // Draw model bounding box
  void Draw(ModelShader* shader, glm::vec3 positionVector, glm::vec3 scaleVector, glm::vec3 rotationVector, float pickingColor);

  // Insert vertice - if it is necessary, use for bounding box vertex possition
  void insertVertice(float x, float y, float z);

  // Create bounding box using set bor vertices
  void createBox();

  // Get position of bounding box vertice
  glm::vec4 a();
  glm::vec4 b();
  glm::vec4 c();
  glm::vec4 d();
  glm::vec4 e();
  glm::vec4 f();
  glm::vec4 g();
  glm::vec4 h();

private:
  GLuint VAO, VBO, EBO;

  // Intervals for bounding box construction
  float minX;
  float minY;
  float minZ;
  float maxX;
  float maxY;
  float maxZ;
};

#endif /* SRC_BOUNDING_BOX_H_ */
