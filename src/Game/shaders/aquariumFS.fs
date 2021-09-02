#version 130

in vec2 texcoord; 
in vec2 texLightCoord;
in vec2 texNormalCoord;
uniform sampler2D tex; 
uniform sampler2D texNormal; 
uniform sampler2D texLight;
uniform mat4 mvMatrix;
uniform mat4 mvMatrixInvTransp;
uniform mat4 mvMatrixInv;
uniform float bottom;
out vec4 fragColor;
in vec3 VertexPosition;
uniform mat3 mn;

in vec3 n;
in vec3 t;

// Water
const float pi = 3.14159;
uniform float timeWave;
uniform int numWaves;
uniform float amplitude[8];
uniform float wavelength[8];
uniform float speed[8];
uniform vec2 direction[8];

float wave(int i, float x, float y) {
    float frequency = 2*pi/wavelength[i];
    float phase = speed[i] * frequency;
    float theta = dot(direction[i], vec2(x, y));
    return amplitude[i] * sin(theta * frequency + timeWave * phase);
}

float waveHeight(float x, float y) {
    float height = 0.0;
    for (int i = 0; i < numWaves; ++i)
        height += wave(i, x, y);
    return height;
}
	
	
vec3 rayPlaneIntersect(vec3 planeNormal, vec3 rayDirection, vec3 rayStart, vec3 planePoint) {
    float nDotD = dot(planeNormal, rayDirection);

    if (nDotD == 0)
        return vec3(1000, 1000, 1000);

    return rayStart + rayDirection * (-dot(planeNormal, rayStart - planePoint) / nDotD);
}
	
void main() { 

        // Wave point
        vec3 wavePoint = vec3(VertexPosition.x, waveHeight(VertexPosition.x, VertexPosition.z), VertexPosition.z);

        // Texture color
        vec4 color = texture(tex, texcoord);
        
        // Add lightmap
        if (VertexPosition.y < wavePoint.y)
            color = color * texture(texLight, texLightCoord);
        
        // Bump mapping
        vec3 Normal = normalize(n);
        vec3 Tangent = normalize(t);
        Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
        vec3 Bitangent = cross(Tangent, Normal);
        vec3 BumpMapNormal = texture(texNormal, texNormalCoord).xyz;
        BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
        vec3 NewNormal;
        mat3 TBN = mat3(Tangent, Bitangent, Normal);
        NewNormal = TBN * BumpMapNormal;
        NewNormal = normalize(NewNormal);
            
        // Light
        vec3 lightPosition = vec3(5, 5, 5);

        vec3 N = vec3(mvMatrixInvTransp * vec4(NewNormal, 0));
        vec3 L = normalize(vec3(mvMatrix * vec4(lightPosition,1)) - vec3(mvMatrix * vec4(VertexPosition,1))); 
        vec3 E = normalize(-VertexPosition); // we are in Eye Coordinates, so EyePos is (0,0,0)
        vec3 R = normalize(-reflect(L,N)); 
        
        // Parametry
        vec4 amb, diff, spec;
        float shin;
        if (bottom > 0.5)
        {
            amb = vec4(0.1);
            diff = vec4(0.3);
            spec = vec4(0.0);
            shin = 20;
        }
        else
        {
            amb = vec4(0.1);
            diff = vec4(0.3);
            spec = vec4(0.0);
            shin = 20;
        }
        
        vec4 Iamb = amb;

        // Diffuse
        vec4 Idiff = diff * max(dot(N,L), 0.0);

        // Ambient
        vec4 Ispec = spec * pow(max(dot(R,E),0.0), shin);

        // write Total Color:
        color = color + Iamb + Idiff + Ispec; 
        
        // Positions
        vec3 cameraPosition = vec3(0, 0, 0);
        vec3 vertexPosition = vec3(mvMatrix * vec4(VertexPosition, 1));
        
        vec3 planePoints[6];
        vec3 planeVectors[6];
        vec3 pointInterSects[6];
        
        // Set plane points
        planePoints[0] = vec3(0, -13, 0);  // Bottom
        planePoints[1] = vec3(0, -1, 0);   // Top
        planePoints[2] = vec3(-20, 0, 0);  // Left
        planePoints[3] = vec3(20, 0, 0);   // Right
        planePoints[4] = vec3(0, 0, 20);   // Front
        planePoints[5] = vec3(0, 0, -20);  // Back
        
        // Set plane plane vectors
        planeVectors[0] = vec3(0, 1, 0);    // Bottom
        planeVectors[1] = vec3(0, 1, 0);    // Top
        planeVectors[2] = vec3(1, 0, 0);    // Left
        planeVectors[3] = vec3(1, 0, 0);    // Right
        planeVectors[4] = vec3(0, 0, 1);    // Front
        planeVectors[5] = vec3(0, 0, 1);    // Back
        
        // Find nearest intersect point
        vec3 intersectPoint = vec3(1000, 1000, 1000);
        float nearestDistance = 99999999;
        for (int i = 0; i < 6; ++i)
        {
            vec3 point = rayPlaneIntersect(vec3(mvMatrix * vec4(planeVectors[i], 1)), vertexPosition - cameraPosition, cameraPosition, vec3(mvMatrix * vec4(planePoints[i], 1)));
            float d1 = distance(point, cameraPosition);
            if (d1 < nearestDistance)
            {
                nearestDistance = d1;
                intersectPoint = point;
            }
        }
        
        // Set distance in distance in water
        float distanceInWater = 0;
        if (intersectPoint.x < 999)                  
        {  
            // Distance between camera and vertex
            float dC = distance(vertexPosition, cameraPosition); 
            
            // Distance between intersect point and vertex
            float dP = distance(vertexPosition, intersectPoint);
            
            distanceInWater = dC > dP ? dP : dC;
        }
        

        // Fog density
        float fogDensity = 1.8f;
        
        // Longest distance in distance in water
        float longestDistance = sqrt(56 * 56 + 40 * 40);
        
        // None fog
        if (distanceInWater <= 0 || ((mvMatrixInv * vec4(0, 0, 0, 1)).y > -1 && VertexPosition.y > wavePoint.y))
        {
            fragColor = color;
            return;
        }
        
        // Fog ratio
        float ratio = distanceInWater / longestDistance * fogDensity;
        if (ratio > 1)
            ratio = 1;

	fragColor = ((1 - ratio) * color + ratio * vec4(0.38, 0.72, 0.75, 1)) ; //* texture(texLight, texLightCoord);
}


