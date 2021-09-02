//
// Project for course Computer Graphics (PGR)
// Faculty of Information Technology, Brno University of Technology
//
// Authors: Petr Dobes, xdobes14@stud.fit.vutbr.cz
//          Jaromir Deak, xdeakj00@stud.fit.vutbr.cz
//          Petr Huf, xhufpe00@stud.fit.vutbr.cz
// Date: December 2014
//

#include "bounding_box.h"

using namespace std;

BoundingBox::BoundingBox()
{
  this->minX = 0.0;
  this->minY = 0.0;
  this->minZ = 0.0;

  this->maxX = 0.0;
  this->maxY = 0.0;
  this->maxZ = 0.0;
}

BoundingBox::~BoundingBox()
{
  // TODO Auto-generated destructor stub
}


void BoundingBox::Draw(ModelShader* shader, glm::vec3 positionVector, glm::vec3 scaleVector, glm::vec3 rotationVector, float pickingColor)
{
  // Compute location matrix
  glm::mat4 m;
  m = glm::translate(shader->getM(), positionVector);
  m = glm::scale( m, scaleVector);
  m = glm::rotate( m, rotationVector.x , glm::vec3(1,0,0));
  m = glm::rotate( m, rotationVector.y , glm::vec3(0,1,0));
  m = glm::rotate( m, rotationVector.z , glm::vec3(0,0,1));

  // Use modelProg
  glUseProgram(shader->modelProg);

  // Set uniform attributes
  glUniformMatrix4fv(shader->modelMUniform, 1, GL_FALSE, glm::value_ptr(m));
  glUniformMatrix4fv(shader->modelVUniform, 1, GL_FALSE, glm::value_ptr(shader->getV()));
  glUniformMatrix4fv(shader->modelPUniform, 1, GL_FALSE, glm::value_ptr(shader->getP()));
  glUniformMatrix4fv(shader->modelMvUniform, 1, GL_FALSE, glm::value_ptr(shader->getV() * shader->getM()));
  glUniform1f(shader->pickingUniform, 1.0);

  // Enable shader attributes
  glEnableVertexAttribArray(shader->modelPositionAttrib);

  glDisableVertexAttribArray(shader->modelColorAttrib);
  glVertexAttrib4f(shader->modelColorAttrib, 0.0, 0.0, pickingColor, 1.0);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glVertexAttribPointer(shader->modelPositionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (GLvoid*)0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  // Draw mesh
  glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}


void BoundingBox::createBox()
{
  // Create vector of box vertices
  // BOTTOM
  vertices.push_back(glm::vec4(minX, minY, minZ, 1.0f)); // A 0
  vertices.push_back(glm::vec4(maxX, minY, minZ, 1.0f)); // B 1
  vertices.push_back(glm::vec4(maxX, minY, maxZ, 1.0f)); // C 2
  vertices.push_back(glm::vec4(minX, minY, maxZ, 1.0f)); // D 3

  // TOP
  vertices.push_back(glm::vec4(minX, maxY, minZ, 1.0f)); // E 4
  vertices.push_back(glm::vec4(maxX, maxY, minZ, 1.0f)); // F 5
  vertices.push_back(glm::vec4(maxX, maxY, maxZ, 1.0f)); // G 6
  vertices.push_back(glm::vec4(minX, maxY, maxZ, 1.0f)); // H 7

  // Create vector of box faces idices
  // ABC
  indices.push_back(A);
  indices.push_back(B);
  indices.push_back(C);

  indices.push_back(A);
  indices.push_back(D);
  indices.push_back(C);

  indices.push_back(A);
  indices.push_back(B);
  indices.push_back(F);

  indices.push_back(A);
  indices.push_back(E);
  indices.push_back(F);

  indices.push_back(B);
  indices.push_back(C);
  indices.push_back(G);

  indices.push_back(B);
  indices.push_back(F);
  indices.push_back(G);

  indices.push_back(C);
  indices.push_back(D);
  indices.push_back(H);

  indices.push_back(C);
  indices.push_back(G);
  indices.push_back(H);

  indices.push_back(A);
  indices.push_back(D);
  indices.push_back(H);

  indices.push_back(A);
  indices.push_back(E);
  indices.push_back(H);

  indices.push_back(E);
  indices.push_back(F);
  indices.push_back(G);

  indices.push_back(E);
  indices.push_back(H);
  indices.push_back(G);

  ////////////////////////////
  // Create buffers
  glGenBuffers(1, &this->VBO);
  glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
  glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec4), &this->vertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &this->EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);
}

void BoundingBox::insertVertice(float x, float y, float z)
{
  // check if vertise is necessary
  if(x < minX) minX = x;
  if(y < minY) minY = y;
  if(z < minZ) minZ = z;

  if(x > maxX) maxX = x;
  if(y > maxY) maxY = y;
  if(z > maxZ) maxZ = z;
}

glm::vec4 BoundingBox::a()
{
  return vertices[A];
}

glm::vec4 BoundingBox::b()
{
  return vertices[B];
}

glm::vec4 BoundingBox::c()
{
  return vertices[C];
}

glm::vec4 BoundingBox::d()
{
  return vertices[D];
}

glm::vec4 BoundingBox::e()
{
  return vertices[E];
}

glm::vec4 BoundingBox::f()
{
  return vertices[F];
}

glm::vec4 BoundingBox::g()
{
  return vertices[G];
}

glm::vec4 BoundingBox::h()
{
  return vertices[H];
}

