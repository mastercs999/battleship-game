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
#include "bubble.h"
#include "sphere.h"
#include "water.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

static GLuint SphereVBO, SphereEBO;
static GLuint SProg;
static GLuint SpositionAttrib, SnormalAttrib, SmvpUniform, SmvUniform, SlightVecUniform;
static GLuint timeLoc;

static int numWaves;
static float* amplitude;
static float* wavelength;
static float* speed;
static float* direction;

static struct Bubble {
    glm::vec3 translate;
    float scale;
    float dY;
} bub;
static std::vector<Bubble> Bubbles;

// Bubble params
static float NewBubbleIntervalFrom = 1.0f;
static float NewBubbleIntervalTo = 2.0f;

static float NewBubbleDyPerFrameFrom = 0.025f;
static float NewBubbleDyPerFrameTo = 0.2f;
static float NewBubbleScaleFrom = 0.2f;
static float NewBubbleScaleTo = 0.7f;

static float SubmarineBubbleDyPerFrameFrom = 0.04f;
static float SubmarineBubbleDyPerFrameTo = 0.06f;
static float SubmarineBubbleScaleFrom = 0.1f;
static float SubmarineBubbleScaleTo = 0.6f;

static float NextTimeForNewBubble;

void BubbleInit()
{
    // Program
    GLuint SVS = compileShader(GL_VERTEX_SHADER, loadFile("shaders/bubble.vs").c_str());
    GLuint SFS = compileShader(GL_FRAGMENT_SHADER, loadFile("shaders/bubble.fs").c_str());
    SProg = linkShader(2, SVS, SFS);
    
    // Get location of uniform variables
    SpositionAttrib = glGetAttribLocation(SProg, "position");
    SnormalAttrib = glGetAttribLocation(SProg, "normal");
    SmvpUniform = glGetUniformLocation(SProg, "mvp");
    SmvUniform = glGetUniformLocation(SProg, "mv");
    SlightVecUniform = glGetUniformLocation(SProg, "lightVec");
    timeLoc = glGetUniformLocation(SProg, "time");
    
    // Copy sphere to graphics card
    glGenBuffers(1, &SphereVBO);
    glBindBuffer(GL_ARRAY_BUFFER, SphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphereVertices), sphereVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &SphereEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere), sphere, GL_STATIC_DRAW);
    
    // Get water parameters
    numWaves = GetNumWaves();
    amplitude = GetAmplitudes();
    wavelength = GetWaveLengths();
    speed = GetSpeeds();
    direction = GetDirections();
    
    // Get next time for new bubble
    NextTimeForNewBubble = GetWaveTime() + fRand(NewBubbleIntervalFrom, NewBubbleIntervalTo);
    
    // Create some bubbles
    for (int i = 0; i < NewBubbleIntervalFrom * 7; ++i)
    {
        Bubble b;
        b.translate = glm::vec3(fRand(-18, 18), fRand(-13, 0), fRand(-18, 18));
        b.scale = fRand(NewBubbleScaleFrom, NewBubbleScaleTo);
        b.dY = fRand(NewBubbleDyPerFrameFrom, NewBubbleDyPerFrameTo);
        Bubbles.push_back(b);
    }
}

void BubbleDraw(glm::mat4 mv, glm::mat4 mvp, glm::mat3 mn)
{
    // Use program
    glUseProgram(SProg);
    
    // Enable blending
    glEnable(GL_DEPTH_TEST);
    glDepthMask(false);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Fill parameters
    glEnableVertexAttribArray(SnormalAttrib);
    glEnableVertexAttribArray(SpositionAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, SphereVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SphereEBO);
    glVertexAttribPointer(SpositionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(SphereVertex), (void*)offsetof(SphereVertex, position));
    glVertexAttribPointer(SnormalAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(SphereVertex), (void*)offsetof(SphereVertex, normal));

    // Mvp
    glUniformMatrix4fv(SmvpUniform, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(SmvUniform, 1, GL_FALSE, glm::value_ptr(mv));
     
    // Send water parameters
    glUniform1f(timeLoc, GetWaveTime()); 
    glUniform1i(glGetUniformLocation(SProg, "numWaves"), numWaves); 
    glUniform1fv(glGetUniformLocation(SProg, "amplitude"), 8, amplitude);
    glUniform1fv(glGetUniformLocation(SProg, "wavelength"), 8, wavelength);
    glUniform1fv(glGetUniformLocation(SProg, "speed"), 8, speed);
    glUniform2fv(glGetUniformLocation(SProg, "direction"), 8, direction);
    
    // Get actual time
    float actualTime = GetWaveTime();
    
    // Create new bubble if needed
    if (actualTime > NextTimeForNewBubble)
    {
        Bubble b;
        b.translate = glm::vec3(fRand(-18, 18), -13, fRand(-18, 18));
        b.scale = fRand(NewBubbleScaleFrom, NewBubbleScaleTo);
        b.dY = fRand(NewBubbleDyPerFrameFrom, NewBubbleDyPerFrameTo);
        Bubbles.push_back(b);
        
        // Plan new bubble
        NextTimeForNewBubble = actualTime + fRand(NewBubbleIntervalFrom, NewBubbleIntervalTo);
    }
    
    // Draw bubbles
    for(std::vector<Bubble>::size_type i = 0; i != Bubbles.size(); i++) {
        Bubble b = Bubbles[i];
        
        glUniform3fv(SlightVecUniform, 1, glm::value_ptr(glm::normalize(glm::vec3(5.0f, 5.0f, 5.0f) - b.translate)));
        glUniform1f(glGetUniformLocation(SProg, "scaleS"), b.scale); 
        glUniform3fv(glGetUniformLocation(SProg, "translateS"), 1, glm::value_ptr(b.translate)); 
        glDrawElements(GL_TRIANGLES, sizeof(sphere)/sizeof(**sphere), sphereIndexType, NULL);
    }

    glDisableVertexAttribArray(SnormalAttrib);
    glDisableVertexAttribArray(SpositionAttrib);
    
    // Disable blending
    glDepthMask(true);
    glDisable(GL_BLEND);
    
    // Update bubbles
    for(std::vector<Bubble>::size_type i = 0; i != Bubbles.size(); i++) 
    {
        // Update Y coord
        Bubbles[i].translate.y += Bubbles[i].dY;
        
        // Remove too high bubble
        if (Bubbles[i].translate.y > 5)
        {
            Bubbles.erase(Bubbles.begin() + i);
            --i;
            continue;
        }
    }
}

void DrawSubmarineBubble(float x, float y, float z)
{
    Bubble b;
    b.translate = glm::vec3(x, y, z);
    b.scale = fRand(SubmarineBubbleScaleFrom, SubmarineBubbleScaleTo);
    b.dY = fRand(SubmarineBubbleDyPerFrameFrom, SubmarineBubbleDyPerFrameTo);
    Bubbles.push_back(b);
}
