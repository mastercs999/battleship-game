#version 130

// MVP matrice
uniform mat4 mvp;

// Waves
const float pi = 3.14159;
uniform float time;
uniform int numWaves;
uniform float amplitude[8];
uniform float wavelength[8];
uniform float speed[8];
uniform vec2 direction[8];

// Koule
in vec3 position; 
in vec3 normal; 
uniform vec3 lightVec;
uniform float scaleS;
uniform vec3 translateS;
out float color;
out float waveY;
out float vertexY;

// Mlha
out vec3 VertexPosition;

float wave(int i, float x, float y) {
    float frequency = 2*pi/wavelength[i];
    float phase = speed[i] * frequency;
    float theta = dot(direction[i], vec2(x, y));
    return amplitude[i] * sin(theta * frequency + time * phase);
}

float waveHeight(float x, float y) {
    float height = 0.0;
    for (int i = 0; i < numWaves; ++i)
        height += wave(i, x, y);
    return height;
}

void main() {

        // Vertex position
        vec3 newPosition = (position * scaleS) + translateS;
        gl_Position = mvp*vec4(newPosition,1);     

        // Light
        color = 0.7*max(0, dot(normal, lightVec));

        // Count wave height
        waveY = waveHeight(newPosition.x, newPosition.z) - 0.05;
        
        // Save vertex y
        vertexY = newPosition.y;
        
        // Save vertex position
        VertexPosition = newPosition;
}