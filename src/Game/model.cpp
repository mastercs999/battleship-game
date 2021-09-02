//
// Project for course Computer Graphics (PGR)
// Faculty of Information Technology, Brno University of Technology
//
// Authors: Petr Dobes, xdobes14@stud.fit.vutbr.cz
//          Jaromir Deak, xdeakj00@stud.fit.vutbr.cz
//          Petr Huf, xhufpe00@stud.fit.vutbr.cz
// Date: December 2014
//

#include "model.h"

// Constructor, expects a filepath to a 3D model.
Model::Model(const GLchar* path)
{
  // Set initial state of model
  this->setScale(0.0);
  this->setPosition(0.0, 0.0, 0.0);
  this->setRotation(0.0, 0.0, 0.0);
  this->direction = glm::vec3(0.0, 0.0, 0.0);

  this->loadModel(path);
  this->boundingBox.createBox();

  deviance = fRand(-0.1, 0.1); // swinging amplitude
  step = fRand(0.005, 0.01) ;  // increment of swinging amplitude
  swing = false;               // disable swinging
  destroyed = false;           // set visible
  visible = true;
}

Model::~Model()
{

}

// Draws the model, and thus all its meshes
void Model::Draw(ModelShader* shader)
{
  if(this->visible == false) return; // Don't draw if it is not visible.

  // Set the deviance of swinging
  if(deviance >= 0.1 || deviance <= -0.1)
    step *= -1;
  deviance += step;

  // Draw model (all meshes)
  for (GLuint i = 0; i < this->meshes.size(); i++)
  {
    if(swing == true)
      this->meshes[i].Draw(shader, glm::vec3(position.x, position.y + deviance, position.z), this->getScaleVector(), this->getRotationVector(), isDestroyed());
    else
      this->meshes[i].Draw(shader, position, this->getScaleVector(), this->getRotationVector(), isDestroyed());
  }
}


void Model::DrawBox(ModelShader* shader, float pickingColor)
{
  // Draw model bounding box.
  boundingBox.Draw(shader, this->getPositionVector(), this->getScaleVector(), this->getRotationVector(), pickingColor);
}


/*  Functions   */
// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::loadModel(string path)
{
  // Read file via ASSIMP
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);
  // Check for errors
  if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
  {
    cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
    return;
  }
  // Retrieve the directory path of the filepath
  this->directory = path.substr(0, path.find_last_of('/'));

  // Process ASSIMP's root node recursively
  this->processNode(scene->mRootNode, scene);
}

// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode* node, const aiScene* scene)
{
  // Process each mesh located at the current node
  for (GLuint i = 0; i < node->mNumMeshes; i++)
  {
    // The node object only contains indices to index the actual objects in the scene.
    // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    this->meshes.push_back(this->processMesh(mesh, scene));
  }
  // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
  for (GLuint i = 0; i < node->mNumChildren; i++)
  {
    this->processNode(node->mChildren[i], scene);
  }

}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
  // Data to fill
  vector<Vertex> vertices;
  vector<GLuint> indices;
  vector<Texture> textures;

  // Walk through each of the mesh's vertices
  for (GLuint i = 0; i < mesh->mNumVertices; i++)
  {
    Vertex vertex;
    glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.

    // Positions
    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    vertex.Position = vector;

    this->boundingBox.insertVertice(vector.x, vector.y, vector.z); // create bounding box

    // Normals
    vector.x = mesh->mNormals[i].x;
    vector.y = mesh->mNormals[i].y;
    vector.z = mesh->mNormals[i].z;
    vertex.Normal = vector;
    // Texture Coordinates
    if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
    {
      glm::vec2 vec;
      // A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
      // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.TexCoords = vec;
    }
    else
      vertex.TexCoords = glm::vec2(0.0f, 0.0f);
    vertices.push_back(vertex);
  }
  // Now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
  for (GLuint i = 0; i < mesh->mNumFaces; i++)
  {
    aiFace face = mesh->mFaces[i];
    // Retrieve all indices of the face and store them in the indices vector
    for (GLuint j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }

  // Process materials
  if (mesh->mMaterialIndex >= 0)
  {
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // We assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // Diffuse: texture_diffuseN
    // Specular: texture_specularN
    // Normal: texture_normalN

    // 1. Diffuse maps
    vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. Specular maps
    vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
  }

  // Return a mesh object created from the extracted mesh data
  return Mesh(vertices, indices, textures);
}

void Model::setScale(float scale)
{
  this->scale = glm::vec3(scale, scale, scale);
}

glm::vec3 Model::getScaleVector()
{
  return this->scale;
}

void Model::setPosition(float x, float y, float z)
{
  //translate model
  this->position =  glm::vec3(x, y, z);
}

glm::vec3 Model::getPositionVector()
{
  return this->position;
}

void Model::setRotation(float x, float y, float z)
{
  this->rotation =  glm::vec3(x, y, z);
}

glm::vec3 Model::getRotationVector()
{
  return this->rotation;
}


void Model::translate(float dx, float dy, float dz)
{
  this->position.x += dx;
  this->position.y += dy;
  this->position.z += dz;
}

void Model::rotate(float pitch, float yaw, float roll)
{
  this->rotation.x += pitch;
  this->rotation.y += yaw;
  this->rotation.z += roll;
}

glm::vec4 Model::BoxVertA()
{
  // Compute location matrix
    glm::mat4 m(1.0f);
    m = glm::translate(m, position);
    m = glm::scale( m, scale);
    m = glm::rotate( m, rotation.x , glm::vec3(1,0,0));
    m = glm::rotate( m, rotation.y , glm::vec3(0,1,0));
    m = glm::rotate( m, rotation.z , glm::vec3(0,0,1));
    return m * boundingBox.a();
}

glm::vec4 Model::BoxVertB()
{
  // Compute location matrix
    glm::mat4 m(1.0f);
    m = glm::translate(m, position);
    m = glm::scale( m, scale);
    m = glm::rotate( m, rotation.x , glm::vec3(1,0,0));
    m = glm::rotate( m, rotation.y , glm::vec3(0,1,0));
    m = glm::rotate( m, rotation.z , glm::vec3(0,0,1));
    return m * boundingBox.b();
}

glm::vec4 Model::BoxVertC()
{
  // Compute location matrix
  glm::mat4 m(1.0f);
    m = glm::translate(m, position);
    m = glm::scale( m, scale);
    m = glm::rotate( m, rotation.x , glm::vec3(1,0,0));
    m = glm::rotate( m, rotation.y , glm::vec3(0,1,0));
    m = glm::rotate( m, rotation.z , glm::vec3(0,0,1));
    return m * boundingBox.c();
}

glm::vec4 Model::BoxVertD()
{
  // Compute location matrix
  glm::mat4 m(1.0f);
    m = glm::translate(m, position);
    m = glm::scale( m, scale);
    m = glm::rotate( m, rotation.x , glm::vec3(1,0,0));
    m = glm::rotate( m, rotation.y , glm::vec3(0,1,0));
    m = glm::rotate( m, rotation.z , glm::vec3(0,0,1));
    return m * boundingBox.d();
}

glm::vec4 Model::BoxVertE()
{
  // Compute location matrix
  glm::mat4 m(1.0f);
    m = glm::translate(m, position);
    m = glm::scale( m, scale);
    m = glm::rotate( m, rotation.x , glm::vec3(1,0,0));
    m = glm::rotate( m, rotation.y , glm::vec3(0,1,0));
    m = glm::rotate( m, rotation.z , glm::vec3(0,0,1));
    return m * boundingBox.e();
}

glm::vec4 Model::BoxVertF()
{
  // Compute location matrix
  glm::mat4 m(1.0f);
    m = glm::translate(m, position);
    m = glm::scale( m, scale);
    m = glm::rotate( m, rotation.x , glm::vec3(1,0,0));
    m = glm::rotate( m, rotation.y , glm::vec3(0,1,0));
    m = glm::rotate( m, rotation.z , glm::vec3(0,0,1));
    return m * boundingBox.f();
}

glm::vec4 Model::BoxVertG()
{
  // Compute location matrix
  glm::mat4 m(1.0f);
    m = glm::translate(m, position);
    m = glm::scale( m, scale);
    m = glm::rotate( m, rotation.x , glm::vec3(1,0,0));
    m = glm::rotate( m, rotation.y , glm::vec3(0,1,0));
    m = glm::rotate( m, rotation.z , glm::vec3(0,0,1));
    return m * boundingBox.g();
}

glm::vec4 Model::BoxVertH()
{
  // Compute location matrix
  glm::mat4 m(1.0f);
    m = glm::translate(m, position);
    m = glm::scale( m, scale);
    m = glm::rotate( m, rotation.x , glm::vec3(1,0,0));
    m = glm::rotate( m, rotation.y , glm::vec3(0,1,0));
    m = glm::rotate( m, rotation.z , glm::vec3(0,0,1));
    return m * boundingBox.h();
}

// Checks all material textures of a given type and loads the textures if they're not loaded yet.
// The required info is returned as a Texture struct.
vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
  vector<Texture> textures;
  for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
  {
    aiString str;
    mat->GetTexture(type, i, &str);
    // Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
    Texture texture;
    texture.id = TextureFromFile(str.C_Str(), this->directory);
    texture.type = typeName;
    texture.path = str;
    textures.push_back(texture);
  }
  return textures;
}

GLint TextureFromFile(const char* path, string directory)
{
  //cout << "Texture loaded: " << path << endl;
  //Generate texture ID and load texture data
  string filename = string(path);
  filename = directory + '/' + filename;
  GLuint textureID;
  glGenTextures(1, &textureID);

  //int width, height;
  //unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
  //-------------
  // Load texture
  SDL_Surface * modelSurface = IMG_Load(filename.c_str());
  if (modelSurface == NULL)
  {
    throw SDL_Exception();
  }

  //SDL_DisplayFormat(modelSurface);

  // Assign texture to ID
  glBindTexture(GL_TEXTURE_2D, textureID);
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  //-------------
  SurfaceImage2D(GL_TEXTURE_2D, 0, GL_RGB, modelSurface);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Parameters
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  //SOIL_free_image_data(image);
  return textureID;
}

void Model::printBoundingBoxBounds()
{
  cout << "A: " << BoxVertA().x << " " << BoxVertA().y << " " << BoxVertA().z << endl;
}

void Model::moveAhead()
{
  if (direction.x != 0.0 || direction.z != 0.0) // Do nothing if direction model isn't moving.
  {
    // Stop model when position is out of aquarium
    if(position.x > 22 || position.x < -22 )
    {
      direction.x = 0.0;
    }
    // Stop model when position is out of aquarium
    if(position.z > 22 || position.z < -22 )
    {
      direction.z = 0.0;
    }

    setVisibility(true);

    // Rotate while moving.
    rotate(0, 0, -15);

    // Set new model position.
    translate(direction.x, direction.y, direction.z); // move ahead - Y axis is constant

    // Generate bubbles behind model.
    generateBubble();
  }
}

void Model::setSwing(bool swing)
{
  this->swing = swing;
}

void Model::setDestroyed(bool destroyed)
{
  this->destroyed = destroyed;

  // Destroyed object should be visible (another skin, but visible)
  if(this->destroyed == true)
    setVisibility(true);

  // Bubbles efect when model is destroyed or renewed
  int bubbles = fRand(30, 40);
  for(int i = 0; i < bubbles; ++i)
  {
    float x = fRand(min(BoxVertA().x, BoxVertB().x, BoxVertC().x, BoxVertD().x), max(BoxVertA().x, BoxVertB().x, BoxVertC().x, BoxVertD().x));
    float y = fRand(BoxVertA().y, BoxVertC().y);
    float z = fRand(min(BoxVertA().z, BoxVertB().z, BoxVertC().z, BoxVertD().z), max(BoxVertA().z, BoxVertB().z, BoxVertC().z, BoxVertD().z));

    DrawSubmarineBubble(x, y, z);
  }
}

bool Model::isDestroyed()
{
  return this->destroyed;
}

void Model::setVisibility(bool visible)
{
  this->visible = visible;
}

void Model::hit()
{
  // Stop model moving.
  this->direction.x = 0.0;
  this->direction.z = 0.0;

  // Hide model.
  setVisibility(false);
}

void Model::setCursorPosition(float x, float y, float z)
{
  // Position out off aquarium
  if (x < -22 || x > 22 || y < -12.7 || y > -1 || z < -22 || z > 22)
  {
    SDL_ShowCursor(true);
    setVisibility(false);

    if(direction.x != 0 || direction.z != 0)
      setVisibility(true);

    return;
  }

  if (this->direction.x != 0.0 || this->direction.z != 0.0)
    return; // Torpedo is running

  // Set correct model position and rotation. Depends on cursor position.
  setVisibility(true);
  SDL_ShowCursor(false);

  setPosition(x, y, z);

  if (x < -19.7 && x > -22)  // left side
  {
    setRotation(0, 270, 0);
  }
  else if (x > 19.7 && x < 22)    // right side
  {
    setRotation(0, 90, 0);
  }
  else if (z < -19.7 && z > -22)  // front side
  {
    setRotation(0, 180, 0);
  }
  else if (z > 19.7 && z < 22)    // back side
  {
    setRotation(0, 0, 0);
  }
}

void Model::generateBubble()
{
  // Compute random position behind model.
  float x = fRand(BoxVertC().x, BoxVertD().x);
  float y = fRand(BoxVertC().y, BoxVertG().y);
  float z = BoxVertC().z;

  DrawSubmarineBubble(x, y, z);
}

void Model::fire(float x, float y, float z)
{
  if (y < -12.7 || y > -1 ) return; // Position y out off aquarium
  if (this->direction.x != 0.0 || this->direction.z != 0.0) return; // Torpedo is running


  // Set correct model position and direction vector. Depends on cursor position.
  setPosition(x, y, z);

  if (x < -19.7 && x > -22)
  {
    this->direction = glm::vec3(0.2, 0.0, 0.0);
    setRotation(0, 270, 0);
  }
  if (x > 19.7 && x < 22)
  {
    this->direction = glm::vec3(-0.2, 0.0, 0.0);
    setRotation(0, 90, 0);
  }
  if (z < -19.7 && z > -22)
  {
    this->direction = glm::vec3(0.0, 0.0, 0.2);
    setRotation(0, 180, 0);
  }
  if (z > 19.7 && z < 22)
  {
    this->direction = glm::vec3(0.0, 0.0, -0.2);
    setRotation(0, 0, 0);
  }

}

float Model::min(float a, float b, float c, float d)
{
  if(b < a) a = b;
  if(c < a) a = c;
  if(d < a) a = d;
  return a;
}

float Model::max(float a, float b, float c, float d)
{
  if(b > a) a = b;
  if(c > a) a = c;
  if(d > a) a = d;
  return a;
}
