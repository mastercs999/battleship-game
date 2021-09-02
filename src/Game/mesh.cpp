//
// Project for course Computer Graphics (PGR)
// Faculty of Information Technology, Brno University of Technology
//
// Authors: Petr Dobes, xdobes14@stud.fit.vutbr.cz
//          Jaromir Deak, xdeakj00@stud.fit.vutbr.cz
//          Petr Huf, xhufpe00@stud.fit.vutbr.cz
// Date: December 2014
//

#include "mesh.h"

// Constructor
Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
{
  this->vertices = vertices;
  this->indices = indices;
  this->textures = textures;

  // Now that we have all the required data, set the vertex buffers and its attribute pointers.
  this->setupMesh();
}

Mesh::~Mesh()
{
  // TODO Auto-generated destructor stub
}

// Render the mesh
void Mesh::Draw(ModelShader* shader, glm::vec3 positionVector, glm::vec3 scaleVector, glm::vec3 rotationVector, bool destroyed)  //TODO vratit texturu a vyresit shader
{
    glEnable(GL_DEPTH_TEST);
    glDepthMask (GL_TRUE);
    glDisable(GL_BLEND);
    
    if (destroyed)
    {
        glDepthMask (GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    }
      
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
  glUniformMatrix4fv(shader->modelVMatUniform, 1, GL_FALSE, glm::value_ptr(shader->getV()));
  glUniformMatrix4fv(shader->modelVMatTranspInvUniform, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(shader->getV()))));
  glUniform1f(shader->pickingUniform, 0.0);

  if(destroyed == true)
    glUniform1f(shader->destroyedUniform, 1.0f);
  else
    glUniform1f(shader->destroyedUniform, 0.0);

  // Enable shader attributes
  glEnableVertexAttribArray(shader->modelPositionAttrib);
  glEnableVertexAttribArray(shader->modelColorAttrib);


  // Bind appropriate textures
  GLuint diffuseNr = 1;
  GLuint specularNr = 1;
  for (GLuint i = 0; i < this->textures.size(); i++)
  {
    glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
    // Retrieve texture number (the N in diffuse_textureN)
    stringstream ss;
    string number;
    string name = this->textures[i].type;
    if (name == "texture_diffuse")
      ss << diffuseNr++; // Transfer GLuint to stream
    else if (name == "texture_specular")
      ss << specularNr++; // Transfer GLuint to stream
    number = ss.str();
    // Now set the sampler to the correct texture unit
    glUniform1f(glGetUniformLocation(shader->modelProg, (name + number).c_str()), i);
    // And finally bind the texture
    glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
  }
  glActiveTexture(GL_TEXTURE0); // Always good practice to set everything back to defaults once configured.

  // Draw mesh
  glBindVertexArray(this->VAO);
  glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

// Initializes all the buffer objects/arrays
void Mesh::setupMesh()
{
  // Create buffers/arrays
  glGenVertexArrays(1, &this->VAO);
  glGenBuffers(1, &this->VBO);
  glGenBuffers(1, &this->EBO);

  glBindVertexArray(this->VAO);
  // Load data into vertex buffers
  glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
  // A great thing about structs is that their memory layout is sequential for all its items.
  // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
  // again translates to 3/2 floats which translates to a byte array.
  glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

  // Set the vertex attribute pointers
  // Vertex Positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
  // Vertex Normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
  // Vertex Texture Coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

  glBindVertexArray(0);
}

