//
// Project for course Computer Graphics (PGR)
// Faculty of Information Technology, Brno University of Technology
//
// Authors: Petr Dobes, xdobes14@stud.fit.vutbr.cz
//          Jaromir Deak, xdeakj00@stud.fit.vutbr.cz
//          Petr Huf, xhufpe00@stud.fit.vutbr.cz
// Date: December 2014
//

#include "model_shader.h"

ModelShader::ModelShader()
{
  this->init();

}

ModelShader::~ModelShader()
{
  // TODO Auto-generated destructor stub
}

void ModelShader::setM(glm::mat4 m)
{
  this->m = m;
}

void ModelShader::setV(glm::mat4 v)
{
  this->v = v;
}

void ModelShader::setP(glm::mat4 p)
{
  this->p = p;
}

glm::mat4 ModelShader::getMVP()
{
  return this->p * this->v * this->m;
}

glm::mat4 ModelShader::getM()
{
  return this->m;
}

glm::mat4 ModelShader::getV()
{
  return this->v;
}

glm::mat4 ModelShader::getP()
{
  return this->p;
}

void ModelShader::init(){
  modelVS = compileShader(GL_VERTEX_SHADER, loadFile("./shaders/modelVS.vs").c_str());
  modelFS = compileShader(GL_FRAGMENT_SHADER, loadFile("./shaders/modelFS.fs").c_str());
  modelProg = linkShader(2, modelVS, modelFS);

  // Get position of attributes
  modelPositionAttrib = glGetAttribLocation(modelProg, "position");
  modelColorAttrib = glGetAttribLocation(modelProg, "color");
  modelMvUniform = glGetUniformLocation(modelProg, "mv");
  modelMUniform = glGetUniformLocation(modelProg, "m");
  modelVUniform = glGetUniformLocation(modelProg, "v");
  modelPUniform = glGetUniformLocation(modelProg, "p");
  pickingUniform = glGetUniformLocation(modelProg, "picking");
  destroyedUniform = glGetUniformLocation(modelProg, "destroyed");
  modelVMatUniform = glGetUniformLocation(modelProg, "vMat");
  modelVMatTranspInvUniform = glGetUniformLocation(modelProg, "vMatTranspInv");
}

