//
// Project for course Computer Graphics (PGR)
// Faculty of Information Technology, Brno University of Technology
//
// Authors: Petr Dobes, xdobes14@stud.fit.vutbr.cz
//          Jaromir Deak, xdeakj00@stud.fit.vutbr.cz
//          Petr Huf, xhufpe00@stud.fit.vutbr.cz
// Date: December 2014
//

#ifndef AQUARIUM_DATA_H_
#define AQUARIUM_DATA_H_

// Model of an aquarium

// Size of the aquarium:

//  height:
#define Y_MIN -13.0
#define Y_MAX   1.0

// width in x-axis:
#define X_MAX  19.75
#define X_MIN -19.75

// width in z-axis:
#define Z_MAX  19.75
#define Z_MIN -19.75

struct AquariumBottomVertex {
    float color[4];
    float position[3];
    float texcoord[2];
    float texLightCoords[2];
    float texNormalCoords[2];
    float normal[3];
    float tangent[3];
} const aquariumBottomVertices[] = {
    
    { { 0.0, 0.0, 1.0, 1 }, {  X_MIN,  Y_MIN, Z_MIN }, { 0.0, 3.0 }, { 1.4, 0.0 }, { 0.0, 3.0 }, { 0.0, 1.0, 0.0 }, { 0.0, 0.0, -1.0 }},
    { { 0.0, 0.0, 1.0, 1 }, {  X_MAX,  Y_MIN, Z_MIN }, { 3.0, 3.0 }, { 1.4, 1.4 }, { 3.0, 3.0 }, { 0.0, 1.0, 0.0 }, { 0.0, 0.0, -1.0 }},
    { { 0.0, 0.0, 1.0, 1 }, {  X_MAX,  Y_MIN, Z_MAX }, { 3.0, 0.0 }, { 0.0, 1.4 }, { 3.0, 0.0 }, { 0.0, 1.0, 0.0 }, { 0.0, 0.0, -1.0 }},
    { { 0.0, 0.0, 1.0, 1 }, {  X_MIN,  Y_MIN, Z_MAX }, { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 1.0, 0.0 }, { 0.0, 0.0, -1.0 }},
        
};

const unsigned char aquariumBottom[] = {
     
        // Bottom
     0,  2, 1,
     0,  3, 2,

};


struct AquariumSideVertex {
    float color[4];
    float position[3];
    float texcoord[2];
    float texLightCoords[2];
    float texNormalCoords[2];
    float normal[3];
    float tangent[3];
} const aquariumSidesVertices[] = {
    
    // Walls        
    { { 0.0, 1.0, 0.0, 1 }, { X_MIN, Y_MIN, Z_MIN }, { 0.0, 1.0 }, { 1.4, 1.0 }, { 1.4, 0.0 },{ 1.0, 0.0, 0.0 }, { 0.0, 0.0, -1.0 } },  // 0
    { { 0.0, 1.0, 1.0, 1 }, { X_MIN, Y_MIN, Z_MAX }, { 1.4, 1.0 }, { 0.0, 1.0 }, { 0.0, 0.0 },{ 1.0, 0.0, 0.0 }, { 0.0, 0.0, -1.0 } },  // 1
    { { 1.0, 0.0, 0.0, 1 }, { X_MIN, Y_MAX, Z_MAX }, { 1.4, 0.0 }, { 0.0, 0.0 }, { 0.0, 1.0 },{ 1.0, 0.0, 0.0 }, { 0.0, 0.0, -1.0 } },  // 2
    { { 1.0, 0.0, 1.0, 1 }, { X_MIN, Y_MAX, Z_MIN }, { 0.0, 0.0 }, { 1.4, 0.0 }, { 1.4, 1.0 },{ 1.0, 0.0, 0.0 }, { 0.0, 0.0, -1.0 } },  // 3
        
    { { 0.0, 1.0, 0.0, 1 }, {  X_MAX, Y_MIN, Z_MIN }, { 0.0, 1.0 }, { 1.4, 0.0 }, { 0.0, 1.0 },{ -1.0, 0.0, 0.0 }, { 0.0, 0.0, -1.0 } },  // 4
    { { 0.0, 1.0, 1.0, 1 }, {  X_MAX, Y_MIN, Z_MAX }, { 1.4, 1.0 }, { 0.0, 0.0 }, { 1.4, 1.0 },{ -1.0, 0.0, 0.0 }, { 0.0, 0.0, -1.0 } },  // 5
    { { 1.0, 0.0, 0.0, 1 }, {  X_MAX, Y_MAX, Z_MAX }, { 1.4, 0.0 }, { 0.0, 1.0 }, { 1.4, 0.0 },{ -1.0, 0.0, 0.0 }, { 0.0, 0.0, -1.0 } },  // 6
    { { 1.0, 0.0, 1.0, 1 }, {  X_MAX, Y_MAX, Z_MIN }, { 0.0, 0.0 }, { 1.4, 1.0 }, { 0.0, 0.0 },{ -1.0, 0.0, 0.0 }, { 0.0, 0.0, -1.0 } },  // 7
    
    
    { { 0.0, 1.0, 0.0, 1 }, {  X_MIN, Y_MIN, Z_MAX }, { 0.0, 1.0 }, { 1.0, 0.0 }, { 1.4, 0.0 },{ 0.0, 0.0, -1.0 }, { 0.0, -1.0, 0.0 } },  // 8
    { { 0.0, 1.0, 1.0, 1 }, {  X_MAX, Y_MIN, Z_MAX }, { 1.4, 1.0 }, { 1.0, 1.4 }, { 0.0, 0.0 },{ 0.0, 0.0, -1.0 }, { 0.0, -1.0, 0.0 } },  // 9
    { { 1.0, 0.0, 0.0, 1 }, {  X_MAX, Y_MAX, Z_MAX }, { 1.4, 0.0 }, { 0.0, 1.4 }, { 0.0, 1.0 },{ 0.0, 0.0, -1.0 }, { 0.0, -1.0, 0.0 } },  // 10
    { { 1.0, 0.0, 1.0, 1 }, {  X_MIN, Y_MAX, Z_MAX }, { 0.0, 0.0 }, { 0.0, 0.0 }, { 1.4, 1.0 },{ 0.0, 0.0, -1.0 }, { 0.0, -1.0, 0.0 } },  // 11
    
    { { 0.0, 1.0, 0.0, 1 }, {  X_MAX, Y_MIN, Z_MIN }, { 0.0, 1.0 }, { 0.0, 1.4 }, { 0.0, 1.0 },{ 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0 } },  // 12
    { { 0.0, 1.0, 1.0, 1 }, {  X_MIN, Y_MIN, Z_MIN }, { 1.4, 1.0 }, { 0.0, 0.0 }, { 1.4, 1.0 },{ 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0 } },  // 13
    { { 1.0, 0.0, 0.0, 1 }, {  X_MIN, Y_MAX, Z_MIN }, { 1.4, 0.0 }, { 1.0, 0.0 }, { 1.4, 0.0 },{ 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0 } },  // 14
    { { 1.0, 0.0, 1.0, 1 }, {  X_MAX, Y_MAX, Z_MIN }, { 0.0, 0.0 }, { 1.0, 1.4 }, { 0.0, 0.0 },{ 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0 } },  // 15
        
};



// Aquarium indices (sides)     
const unsigned char aquariumSides[] = {

        //Walls
     0, 2, 1,
     0, 3, 2,
     
     4, 5, 6, 
     4, 6,  7,
     
     8, 10, 9,
     8, 11, 10,
     
     12, 14, 13,
     12, 15, 14,
     
        // Bottom
     //16,  17, 18,
     //16,  19, 17,

};



#endif

