#version 130

// Color
in float color;
in float waveY;
in float vertexY;
out vec4 fragColor;

// Mlha
in vec3 VertexPosition;
uniform mat4 mv;

vec3 rayPlaneIntersect(vec3 planeNormal, vec3 rayDirection, vec3 rayStart, vec3 planePoint) {
    float nDotD = dot(planeNormal, rayDirection);

    if (nDotD == 0)
        return vec3(1000, 1000, 1000);

    return rayStart + rayDirection * (-dot(planeNormal, rayStart - planePoint) / nDotD);
}


void main() {

        // Discard fragment
        if (vertexY > waveY)
            discard; 
            
        // Barva max 1
        float r = min(1, color * 1.8);
        float g = min(1, color * 1.8);
        float b = min(1, color * 2);
        float a = 0.03;

        // Color
	vec4 color = vec4(r, g, b, a);
	
	// Positions
        vec3 cameraPosition = vec3(0, 0, 0);
        vec3 vertexPosition = vec3(mv * vec4(VertexPosition, 1));
        
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
            vec3 point = rayPlaneIntersect(vec3(mv* vec4(planeVectors[i], 1)), vertexPosition - cameraPosition, cameraPosition, vec3(mv * vec4(planePoints[i], 1)));
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
        if (distanceInWater <= 0)
        {
            fragColor = color;
            return;
        }
        
        // Fog ratio
        float ratio = distanceInWater / longestDistance * fogDensity;
        if (ratio > 1)
            ratio = 1;

        fragColor = (1 - ratio) * color + ratio * vec4(0.38, 0.72, 0.75, a);
}