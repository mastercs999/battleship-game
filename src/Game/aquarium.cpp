//
// Project for course Computer Graphics (PGR)
// Faculty of Information Technology, Brno University of Technology
//
// Authors: Petr Dobes, xdobes14@stud.fit.vutbr.cz
//          Jaromir Deak, xdeakj00@stud.fit.vutbr.cz
//          Petr Huf, xhufpe00@stud.fit.vutbr.cz
// Date: December 2014
//

#include "aquarium.h"
#include "aquarium_data.h"
#include "water.h"

extern bool cullBackface;

static GLuint VSaq, FSaq, ProgAquarium;
static GLuint AquariumSidesVBO, AquariumBottomVBO, AquariumSidesEBO, AquariumBottomEBO;
static GLuint aqPositionAttrib, aqTcAttrib, aqMvpUniform, aqTextureUniform, aqTextureNormalUniform, aqMvUniform, aqMnUniform, aqMvUniformFS, aqMvUniformInvTranspFS;
static GLuint aquariumTexture, aquariumBottomTexture, normal_texture, mvMatrixInvUniform, normal_side_texture, normalTexCoordsLoc;
static GLuint drawingBottom;
static GLuint lightmapTexture, aqTextureLightUniform, aqTcLightAttrib, timeLoc, deltaLoc, normalLoc, tangentLoc, lightmapSideTexture;

static int numWaves;
static float* amplitude;
static float* wavelength;
static float* speed;
static float* direction;

void initAquarium()
{
	// Aquarium shader
    VSaq = compileShader(GL_VERTEX_SHADER, loadFile("./shaders/aquariumVS.vs").c_str());
    FSaq = compileShader(GL_FRAGMENT_SHADER, loadFile("./shaders/aquariumFS.fs").c_str());
    ProgAquarium = linkShader(2, VSaq, FSaq);

    // Get position of attributes
    aqPositionAttrib = glGetAttribLocation(ProgAquarium, "position");
    aqTcAttrib = glGetAttribLocation(ProgAquarium, "tc");
    aqTcLightAttrib = glGetAttribLocation(ProgAquarium, "tcLight");
    normalTexCoordsLoc = glGetAttribLocation(ProgAquarium, "tcNormal");
    normalLoc = glGetAttribLocation(ProgAquarium, "normal");
    tangentLoc = glGetAttribLocation(ProgAquarium, "tangent");
    aqMvpUniform = glGetUniformLocation(ProgAquarium, "mvp");
    aqMvUniform = glGetUniformLocation(ProgAquarium, "mv");
    aqMnUniform = glGetUniformLocation(ProgAquarium, "mn");
    aqTextureUniform = glGetUniformLocation(ProgAquarium, "tex");
    aqTextureNormalUniform = glGetUniformLocation(ProgAquarium, "texNormal");
    aqTextureLightUniform = glGetUniformLocation(ProgAquarium, "texLight");
    aqMvUniformFS = glGetUniformLocation(ProgAquarium, "mvMatrix");
    aqMvUniformInvTranspFS = glGetUniformLocation(ProgAquarium, "mvMatrixInvTransp");
    mvMatrixInvUniform = glGetUniformLocation(ProgAquarium, "mvMatrixInv");
    drawingBottom = glGetUniformLocation(ProgAquarium, "bottom");
    timeLoc = glGetUniformLocation(ProgAquarium, "time");
    deltaLoc = glGetUniformLocation(ProgAquarium, "delta");
    
    // Copy Aquarium to graphics card
    glGenBuffers(1, &AquariumSidesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, AquariumSidesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(aquariumSidesVertices), aquariumSidesVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &AquariumSidesEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, AquariumSidesEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(aquariumSides), aquariumSides, GL_STATIC_DRAW);
    
    glGenBuffers(1, &AquariumBottomVBO);
    glBindBuffer(GL_ARRAY_BUFFER, AquariumBottomVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(aquariumBottomVertices), aquariumBottomVertices, GL_STATIC_DRAW);
    
    glGenBuffers(1, &AquariumBottomEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, AquariumBottomEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(aquariumBottom), aquariumBottom, GL_STATIC_DRAW);

	// Load texture for the aquarium sides	
	SDL_Surface * aqSideSurface = IMG_Load("./textures/rock.jpg");
    if(aqSideSurface == NULL) throw SDL_Exception();
    
    glGenTextures(1, &aquariumTexture);
    glBindTexture(GL_TEXTURE_2D, aquariumTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    SurfaceImage2D(GL_TEXTURE_2D, 0, GL_RGB, aqSideSurface);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Load texture for the aquarium bottom
	SDL_Surface * aqBottomSurface = IMG_Load("./textures/sand_02.jpg");
    if(aqBottomSurface == NULL) throw SDL_Exception();
    
    glGenTextures(1, &aquariumBottomTexture);
    glBindTexture(GL_TEXTURE_2D, aquariumBottomTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    SurfaceImage2D(GL_TEXTURE_2D, 0, GL_RGB, aqBottomSurface);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Load normal map for aquarium bottom
    SDL_Surface * aqNormalTx = IMG_Load("./textures/normal.jpg");
    if(aqNormalTx == NULL) throw SDL_Exception();
    
    glGenTextures(1, &normal_texture);
    glBindTexture(GL_TEXTURE_2D, normal_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    SurfaceImage2D(GL_TEXTURE_2D, 0, GL_RGB, aqNormalTx);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Load normal map for sides
    SDL_Surface * aqNormalSideTx = IMG_Load("./textures/normal-side.jpg");
    if(aqNormalSideTx == NULL) throw SDL_Exception();
    
    glGenTextures(1, &normal_side_texture);
    glBindTexture(GL_TEXTURE_2D, normal_side_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    SurfaceImage2D(GL_TEXTURE_2D, 0, GL_RGB, aqNormalSideTx);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Load lightmap for side
    SDL_Surface * lightTx2 = IMG_Load("./textures/lightmap-side.jpg");
    if(lightTx2 == NULL) throw SDL_Exception();
    
    glGenTextures(1, &lightmapSideTexture);
    glBindTexture(GL_TEXTURE_2D, lightmapSideTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    SurfaceImage2D(GL_TEXTURE_2D, 0, GL_RGB, lightTx2);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Load lightmap for bottom
    SDL_Surface * lightTx = IMG_Load("./textures/lightmap.jpg");
    if(lightTx == NULL) throw SDL_Exception();
    
    glGenTextures(1, &lightmapTexture);
    glBindTexture(GL_TEXTURE_2D, lightmapTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    SurfaceImage2D(GL_TEXTURE_2D, 0, GL_RGB, lightTx);
    glGenerateMipmap(GL_TEXTURE_2D);

    
    // Get water params
    numWaves = GetNumWaves();
    amplitude = GetAmplitudes();
    wavelength = GetWaveLengths();
    speed = GetSpeeds();
    direction = GetDirections();
}

void drawAquarium(glm::mat4 mv, glm::mat4 mvp, glm::mat3 mn)
{
    glUseProgram(ProgAquarium);
    
    glm::vec2 dir = glm::vec2(0);
    for (int i = 0; i < 4; ++i)
    {
         dir.y += direction[2 * i];
         dir.x -= direction[2 * i + 1];
    }

    glEnable(GL_DEPTH_TEST);
    glDepthMask (GL_TRUE);
    glDisable(GL_BLEND);

    // Backface culling
    if (cullBackface)
    {
    	glEnable(GL_CULL_FACE);
    }
    
    // Set uniform attributes
    glUniformMatrix4fv(aqMvpUniform, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(aqMvUniform, 1, GL_FALSE, glm::value_ptr(mv));
    glUniformMatrix4fv(aqMvUniformFS , 1, GL_FALSE, glm::value_ptr(mv));
    glUniformMatrix4fv(aqMnUniform, 1, GL_FALSE, glm::value_ptr(mn));
    glUniformMatrix4fv(aqMvUniformInvTranspFS, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(mv))));
    glUniformMatrix4fv(mvMatrixInvUniform, 1, GL_FALSE, glm::value_ptr(glm::inverse(mv)));
    glUniform1f(drawingBottom, 0.0f); 
    glUniform1f(timeLoc, GetWaveTime());
    glUniform2f(deltaLoc, dir.x, dir.y);
    
    // Send water parameters
    glUniform1f(glGetUniformLocation(ProgAquarium, "timeWave"), GetWaveTime()); 
    glUniform1i(glGetUniformLocation(ProgAquarium, "numWaves"), numWaves); 
    glUniform1fv(glGetUniformLocation(ProgAquarium, "amplitude"), 8, amplitude);
    glUniform1fv(glGetUniformLocation(ProgAquarium, "wavelength"), 8, wavelength);
    glUniform1fv(glGetUniformLocation(ProgAquarium, "speed"), 8, speed);
    glUniform2fv(glGetUniformLocation(ProgAquarium, "direction"), 8, direction);
    
    // Texture for sides of the aquarium
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, aquariumTexture);
    glUniform1i(aqTextureUniform, 0);
    
    // Normal texture
    glActiveTexture(GL_TEXTURE1);
    glUniform1i(aqTextureNormalUniform, 1);
    glBindTexture(GL_TEXTURE_2D, normal_side_texture);
    
    // Light texture
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, lightmapSideTexture);
    glUniform1i(aqTextureLightUniform, 2);
    
    // Enable shader attributes
    glEnableVertexAttribArray(aqPositionAttrib);
    glEnableVertexAttribArray(aqTcAttrib);
    glEnableVertexAttribArray(aqTcLightAttrib);
    glEnableVertexAttribArray(normalTexCoordsLoc);
    glEnableVertexAttribArray(normalLoc);
    glEnableVertexAttribArray(tangentLoc);

    // Draw Aquarium/     
    glBindBuffer(GL_ARRAY_BUFFER, AquariumSidesVBO);
    glVertexAttribPointer(aqPositionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(AquariumSideVertex), (void*)offsetof(AquariumSideVertex, position));
    glVertexAttribPointer(aqTcAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(AquariumSideVertex), (void*)offsetof(AquariumSideVertex, texcoord));
    glVertexAttribPointer(aqTcLightAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(AquariumSideVertex), (void*)offsetof(AquariumSideVertex, texLightCoords));
    glVertexAttribPointer(normalTexCoordsLoc, 2, GL_FLOAT, GL_FALSE, sizeof(AquariumSideVertex), (void*)offsetof(AquariumSideVertex, texNormalCoords));
    glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(AquariumSideVertex), (void*)offsetof(AquariumSideVertex, normal));
    glVertexAttribPointer(tangentLoc, 3, GL_FLOAT, GL_FALSE, sizeof(AquariumSideVertex), (void*)offsetof(AquariumSideVertex, tangent));
    
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, AquariumSidesEBO);
     glDrawElements(GL_TRIANGLES, sizeof(aquariumSides)/sizeof(*aquariumSides), GL_UNSIGNED_BYTE, NULL);
     
     // Set drawing bottom
     glUniform1f(drawingBottom, 1.0f); 
    
     // Texture for the bottom of the aquarium
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(aqTextureUniform, 0);
    glBindTexture(GL_TEXTURE_2D, aquariumBottomTexture);
    
    // Normal texture
    glActiveTexture(GL_TEXTURE1);
    glUniform1i(aqTextureNormalUniform, 1);
    glBindTexture(GL_TEXTURE_2D, normal_texture);
    
    // Light texture
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, lightmapTexture);
    glUniform1i(aqTextureLightUniform, 2);

    // Draw the bottom of the aquarium
    glBindBuffer(GL_ARRAY_BUFFER, AquariumBottomVBO);
    glVertexAttribPointer(aqPositionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(AquariumBottomVertex), (void*)offsetof(AquariumBottomVertex, position));
    glVertexAttribPointer(aqTcAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(AquariumBottomVertex), (void*)offsetof(AquariumBottomVertex, texcoord));
    glVertexAttribPointer(aqTcLightAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(AquariumBottomVertex), (void*)offsetof(AquariumBottomVertex, texLightCoords));
    glVertexAttribPointer(normalTexCoordsLoc, 2, GL_FLOAT, GL_FALSE, sizeof(AquariumBottomVertex), (void*)offsetof(AquariumBottomVertex, texNormalCoords));
    glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(AquariumBottomVertex), (void*)offsetof(AquariumBottomVertex, normal));
    glVertexAttribPointer(tangentLoc, 3, GL_FLOAT, GL_FALSE, sizeof(AquariumBottomVertex), (void*)offsetof(AquariumBottomVertex, tangent));
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, AquariumBottomEBO);
    glDrawElements(GL_TRIANGLES, sizeof(aquariumBottom)/sizeof(*aquariumBottom), GL_UNSIGNED_BYTE, NULL);
    
    glActiveTexture(GL_TEXTURE0);
    
    glDisableVertexAttribArray(aqPositionAttrib);
    glDisableVertexAttribArray(aqTcAttrib);
    glDisableVertexAttribArray(aqTcLightAttrib);
    glDisableVertexAttribArray(normalTexCoordsLoc);
    glDisableVertexAttribArray(normalLoc);
    glDisableVertexAttribArray(tangentLoc);
    
	if (cullBackface)
    {
		glDisable(GL_CULL_FACE);
    }
}

