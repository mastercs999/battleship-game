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
#include "water.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static GLuint mvpUniformWater, mvUniformWater, mnUniformWater;
static GLuint VSWater, FSWater, ProgWater;
static GLuint texture[3]; 
static GLuint textureLoc;
static GLuint timeLoc;
static float waterGranulity = 0.25f;
static int waterWidth = 40;
static struct WavePoint {
    glm::vec3 position;
    glm::vec3 normal;
    float textCoords[2];
} waterVerticles[(int)(40 / 0.25)][(int)(40 / 0.25)];   // waterWidth / waterGranulity
static int waterVerticlesCount = (int)(40 / 0.25);

static const float pi = 3.14159;
static float waveTime = 0.5;
static float waveFreq = 0.05;
static int numWaves = 4;

static float amplitude[8];
static float wavelength[8];
static float speed[8];
static float direction[16];


double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

void WaterInit()
{
    // Water shader
    VSWater = compileShader(GL_VERTEX_SHADER, loadFile("shaders/waterVS.vs").c_str());
    FSWater = compileShader(GL_FRAGMENT_SHADER, loadFile("shaders/waterFS.fs").c_str());
    ProgWater = linkShader(2, VSWater, FSWater);
    
    // Get water parameter
    mvpUniformWater = glGetUniformLocation(ProgWater, "mvp");
    mvUniformWater = glGetUniformLocation(ProgWater, "mv");
    mnUniformWater = glGetUniformLocation(ProgWater, "mn");
    textureLoc = glGetUniformLocation(ProgWater, "color_texture");
    timeLoc = glGetUniformLocation(ProgWater, "time");

    // Load water texture
    SDL_Surface * surface = IMG_Load("./textures/sea_02.jpg");
    if(surface == NULL) throw SDL_Exception();

    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    SurfaceImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    
    // Fill water waterVerticles
    float TS = 1.0 / waterWidth;
    int p = 0, t = 0;
    glBegin(GL_QUADS);
    for (float i = -waterWidth / 2.0; i < waterWidth / 2.0 - waterGranulity / 2; i += waterGranulity)
    {
        t = 0;
            for (float j = -waterWidth / 2.0; j < waterWidth / 2.0 - waterGranulity / 2; j+= waterGranulity)
            {
                    float startX = TS*(i * 1 / waterGranulity + waterWidth / 2.0);
                    float startY = TS*(j * 1 / waterGranulity + waterWidth / 2.0);

                    waterVerticles[p][t].position[0] = i;
                    waterVerticles[p][t].position[2] = j;
                    waterVerticles[p][t].textCoords[0] = startX;
                    waterVerticles[p][t].textCoords[1] = startY;
                    
                    ++t;
            }
            ++p;
    }
    
    // Initialize wave parameters
    for (int i = 0; i < numWaves; ++i) {
        amplitude[i] = 0.7f / (i + 1);
        wavelength[i] = 8 * pi / (i + 1);
        speed[i] = i;

        float angle = fRand(-pi/3, pi/3);
        direction[i * 2] = cos(angle);
        direction[i * 2 + 1] = sin(angle);
    }
}

void WaterDraw(glm::mat4 mv, glm::mat4 mvp, glm::mat3 mn)
{
    glUseProgram(ProgWater);
    
    // Enable blending
    glEnable(GL_DEPTH_TEST);
    glDepthMask(false);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Set uniform attributes
    glUniformMatrix4fv(mvpUniformWater, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(mvUniformWater, 1, GL_FALSE, glm::value_ptr(mv)); 
    glUniformMatrix3fv(mnUniformWater, 1, GL_FALSE, glm::value_ptr(mn)); 

    // Bind water texture
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(textureLoc, 0);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    
    float x, y, z;
    glm::vec3 v1, v2, v3, v4, normal;

    // Send water parameters
    glUniform1f(timeLoc, waveTime); 
    glUniform1i(glGetUniformLocation(ProgWater, "numWaves"), numWaves); 
    glUniform1fv(glGetUniformLocation(ProgWater, "amplitude"), 8, amplitude);
    glUniform1fv(glGetUniformLocation(ProgWater, "wavelength"), 8, wavelength);
    glUniform1fv(glGetUniformLocation(ProgWater, "speed"), 8, speed);
    glUniform2fv(glGetUniformLocation(ProgWater, "direction"), 8, direction);
    
    // Draw plane
    glBegin(GL_QUADS);
    for (int i = 1; i < waterVerticlesCount - 1; ++i)
        for (int j = 1; j < waterVerticlesCount - 1; ++j)
        {
            x = waterVerticles[i][j].position[0];
            y = waterVerticles[i][j].position[1];
            z = waterVerticles[i][j].position[2];
            glTexCoord2f(waterVerticles[i][j].textCoords[0], waterVerticles[i][j].textCoords[1]);  
            glVertex3f(x, y, z);
            
            x = waterVerticles[i][j + 1].position[0];
            y = waterVerticles[i][j + 1].position[1];
            z = waterVerticles[i][j + 1].position[2];
            glTexCoord2f(waterVerticles[i][j + 1].textCoords[0], waterVerticles[i][j + 1].textCoords[1]);  
            glVertex3f(x, y, z);
            
            x = waterVerticles[i + 1][j + 1].position[0];
            y = waterVerticles[i + 1][j + 1].position[1];
            z = waterVerticles[i + 1][j + 1].position[2];
            glTexCoord2f(waterVerticles[i + 1][j + 1].textCoords[0], waterVerticles[i + 1][j + 1].textCoords[1]);  
            glVertex3f(x, y, z);
            
            x = waterVerticles[i + 1][j].position[0];
            y = waterVerticles[i + 1][j].position[1];
            z = waterVerticles[i + 1][j].position[2];
            glTexCoord2f(waterVerticles[i + 1][j].textCoords[0], waterVerticles[i + 1][j].textCoords[1]); 
            glVertex3f(x, y, z);
        }
    glEnd();

    // Disable blending
    glDepthMask(true);
    glDisable(GL_BLEND);

    // Update time for wave
    waveTime += waveFreq;
}

float GetWaveTime()
{
    return waveTime;
}

int GetNumWaves()
{
    return numWaves;
}

float* GetAmplitudes()
{
    return amplitude;
}

float* GetWaveLengths()
{
    return wavelength;
}

float* GetSpeeds()
{
    return speed;
}

float* GetDirections()
{
    return direction;
}
