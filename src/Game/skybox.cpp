//
// Project for course Computer Graphics (PGR)
// Faculty of Information Technology, Brno University of Technology
//
// Authors: Petr Dobes, xdobes14@stud.fit.vutbr.cz
//          Jaromir Deak, xdeakj00@stud.fit.vutbr.cz
//          Petr Huf, xhufpe00@stud.fit.vutbr.cz
// Date: December 2014
//

#include "pgr.h"
#include "skybox.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define SKYBOX_WIDTH 100

static float points[] = {
  -SKYBOX_WIDTH,  SKYBOX_WIDTH, -SKYBOX_WIDTH,
  -SKYBOX_WIDTH, -SKYBOX_WIDTH, -SKYBOX_WIDTH,
   SKYBOX_WIDTH, -SKYBOX_WIDTH, -SKYBOX_WIDTH,
   SKYBOX_WIDTH, -SKYBOX_WIDTH, -SKYBOX_WIDTH,
   SKYBOX_WIDTH,  SKYBOX_WIDTH, -SKYBOX_WIDTH,
  -SKYBOX_WIDTH,  SKYBOX_WIDTH, -SKYBOX_WIDTH,
  
  -SKYBOX_WIDTH, -SKYBOX_WIDTH,  SKYBOX_WIDTH,
  -SKYBOX_WIDTH, -SKYBOX_WIDTH, -SKYBOX_WIDTH,
  -SKYBOX_WIDTH,  SKYBOX_WIDTH, -SKYBOX_WIDTH,
  -SKYBOX_WIDTH,  SKYBOX_WIDTH, -SKYBOX_WIDTH,
  -SKYBOX_WIDTH,  SKYBOX_WIDTH,  SKYBOX_WIDTH,
  -SKYBOX_WIDTH, -SKYBOX_WIDTH,  SKYBOX_WIDTH,
  
   SKYBOX_WIDTH, -SKYBOX_WIDTH, -SKYBOX_WIDTH,
   SKYBOX_WIDTH, -SKYBOX_WIDTH,  SKYBOX_WIDTH,
   SKYBOX_WIDTH,  SKYBOX_WIDTH,  SKYBOX_WIDTH,
   SKYBOX_WIDTH,  SKYBOX_WIDTH,  SKYBOX_WIDTH,
   SKYBOX_WIDTH,  SKYBOX_WIDTH, -SKYBOX_WIDTH,
   SKYBOX_WIDTH, -SKYBOX_WIDTH, -SKYBOX_WIDTH,
   
  -SKYBOX_WIDTH, -SKYBOX_WIDTH,  SKYBOX_WIDTH,
  -SKYBOX_WIDTH,  SKYBOX_WIDTH,  SKYBOX_WIDTH,
   SKYBOX_WIDTH,  SKYBOX_WIDTH,  SKYBOX_WIDTH,
   SKYBOX_WIDTH,  SKYBOX_WIDTH,  SKYBOX_WIDTH,
   SKYBOX_WIDTH, -SKYBOX_WIDTH,  SKYBOX_WIDTH,
  -SKYBOX_WIDTH, -SKYBOX_WIDTH,  SKYBOX_WIDTH,
  
  -SKYBOX_WIDTH,  SKYBOX_WIDTH, -SKYBOX_WIDTH,
   SKYBOX_WIDTH,  SKYBOX_WIDTH, -SKYBOX_WIDTH,
   SKYBOX_WIDTH,  SKYBOX_WIDTH,  SKYBOX_WIDTH,
   SKYBOX_WIDTH,  SKYBOX_WIDTH,  SKYBOX_WIDTH,
  -SKYBOX_WIDTH,  SKYBOX_WIDTH,  SKYBOX_WIDTH,
  -SKYBOX_WIDTH,  SKYBOX_WIDTH, -SKYBOX_WIDTH,
  
  -SKYBOX_WIDTH, -SKYBOX_WIDTH, -SKYBOX_WIDTH,
  -SKYBOX_WIDTH, -SKYBOX_WIDTH,  SKYBOX_WIDTH,
   SKYBOX_WIDTH, -SKYBOX_WIDTH, -SKYBOX_WIDTH,
   SKYBOX_WIDTH, -SKYBOX_WIDTH, -SKYBOX_WIDTH,
  -SKYBOX_WIDTH, -SKYBOX_WIDTH,  SKYBOX_WIDTH,
   SKYBOX_WIDTH, -SKYBOX_WIDTH,  SKYBOX_WIDTH
};

static GLuint Prog;
static GLuint vbo, vao, texture;
static GLuint VUniform, PUniform;

void LoadSide(GLuint texture, GLenum side_target, const char* file_name) 
{
    glBindTexture (GL_TEXTURE_CUBE_MAP, texture);
    SDL_Surface *cubeSurface = IMG_Load(file_name);
    SurfaceImage2D(side_target, 0, GL_RGBA, cubeSurface);
}

void CreateSkybox(const char* front, const char* back, const char* top, const char* bottom, const char* left, const char* right, GLuint* tex_cube) 
{
  // Texture
  glActiveTexture (GL_TEXTURE0);
  glGenTextures (1, tex_cube);
  
  // Obrazky
  LoadSide(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, front);
  LoadSide(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, back);
  LoadSide(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, top);
  LoadSide(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, bottom);
  LoadSide(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, left);
  LoadSide(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_X, right);
  
  // Parametry texture
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void SkyboxInit()
{
    // Program
    GLuint SVS = compileShader(GL_VERTEX_SHADER, loadFile("shaders/skybox.vs").c_str());
    GLuint SFS = compileShader(GL_FRAGMENT_SHADER, loadFile("shaders/skybox.fs").c_str());
    Prog = linkShader(2, SVS, SFS);
    
    // Get uniform locations
    VUniform = glGetUniformLocation(Prog, "V");
    PUniform = glGetUniformLocation(Prog, "P");
    
    // Zkopriovani dat na grafiku
    glGenBuffers (1, &vbo);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, 3 * 36 * sizeof (float), &points, GL_STATIC_DRAW);
    
    glGenVertexArrays (1, &vao);
    glBindVertexArray (vao);
    glEnableVertexAttribArray (0);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    // Vytvoreni textury na skybox
    CreateSkybox("./textures/skybox/negz.jpg", "./textures/skybox/posz.jpg", "./textures/skybox/posy.jpg", "./textures/skybox/negy.jpg", "./textures/skybox/negx.jpg", "./textures/skybox/posx.jpg", &texture); 
}

void SkyboxDraw(glm::mat4 v, glm::mat4 p)
{
    glUseProgram (Prog);
    
    glDisable(GL_DEPTH_TEST);
    glDepthMask (GL_FALSE);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    
    glUniformMatrix4fv(VUniform, 1, GL_FALSE, glm::value_ptr(v));
    glUniformMatrix4fv(PUniform, 1, GL_FALSE, glm::value_ptr(p));
    
    glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_CUBE_MAP, texture);
    glBindVertexArray (vao);
    glDrawArrays (GL_TRIANGLES, 0, 36);
    
    // Unbind - important!!!
    glBindVertexArray (0);
}
