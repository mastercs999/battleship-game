//
// Project for course Computer Graphics (PGR)
// Faculty of Information Technology, Brno University of Technology
//
// Authors: Petr Dobes, xdobes14@stud.fit.vutbr.cz
//          Jaromir Deak, xdeakj00@stud.fit.vutbr.cz
//          Petr Huf, xhufpe00@stud.fit.vutbr.cz
// Date: December 2014
//

#ifndef MODEL_SHADER_H_
#define MODEL_SHADER_H_

#include "pgr.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#ifdef WIN
#include <glm/gtc/matrix_projection.hpp>
#endif
#include <glm/gtc/type_ptr.hpp>

// Class stores and set parameters of model shaders
class ModelShader
{
public:
  GLuint modelVS, modelFS, modelProg;
  GLuint modelVBO, modelEBO;
  GLuint modelPositionAttrib, modelColorAttrib;
  //GLuint aspectUniform, translateUniform, rotateUniform, scaleUniform;
  GLuint modelMvUniform, modelMUniform, modelVUniform, modelPUniform, modelVMatUniform, modelVMatTranspInvUniform;
  GLuint pickingUniform;
  GLuint destroyedUniform;

  ModelShader();
  ~ModelShader();

  // Set model matrix
  void setM(glm::mat4 m);
  // Set view matrix
  void setV(glm::mat4 v);
  // Set projection matrix
  void setP(glm::mat4 p);
  
  // Get model-view-projection matrix
  glm::mat4 getMVP();
  // Get model matrix
  glm::mat4 getM();
  // Get view matrix
  glm::mat4 getV();
  // Get projection matrix
  glm::mat4 getP();

private:
  glm::mat4 m, v, p;

  // Initialize shaders
  void init();
};

#endif /* MODEL_SHADER_H_ */
