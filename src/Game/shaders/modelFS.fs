#version 130

in vec4 c;
in vec2 texcoord;
in vec3 VertexPosition;
in vec3 n;

uniform float picking; 
uniform float destroyed;
uniform mat4 mv;
uniform mat4 vMat;
uniform mat4 vMatTranspInv;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

out vec4 fragColor;

vec3 rayPlaneIntersect(vec3 planeNormal, vec3 rayDirection, vec3 rayStart, vec3 planePoint) {
    float nDotD = dot(planeNormal, rayDirection);

    if (nDotD == 0)
        return vec3(1000, 1000, 1000);

    return rayStart + rayDirection * (-dot(planeNormal, rayStart - planePoint) / nDotD);
}

void main()
{
	if(picking > 0.5)
	{
	// for object picking - DO NOT TOUCH!!!
    	fragColor = c;
    ///////////////////////////////////////
    }
    else
    {
    // Do everything you want :-P
      	vec4 color;
      	if(destroyed > 0.5)  // if model is destroyed - use black instead of texture
      	{
    	  //color = 0.9 * vec4(0.0, 0.0, 0.0, 1.0) + 0.1 * texture(texture_diffuse1, texcoord);
    	  color = vec4(0.0, 0.0, 0.0, 0.1);
		}
    	else
    	{
    	  color = texture(texture_diffuse1, texcoord);
    	}
	///////////////////////////////////////
	
	// Ligh
	vec3 lightPosition = vec3(5, 5, 5);

        vec3 N = vec3(vMatTranspInv * vec4(normalize(n), 0));
        vec3 L = normalize(vec3(vMat * vec4(lightPosition,1)) - vec3(vMat * vec4(VertexPosition,1))); 
        vec3 E = normalize(-VertexPosition); // we are in Eye Coordinates, so EyePos is (0,0,0)
        vec3 R = normalize(-reflect(L,N)); 
        
        // Ambient
        vec4 amb = vec4(-0.1);
        vec4 diff = vec4(0.2);
        vec4 spec = vec4(0.3);
        float shin = 1;
        
        vec4 Iamb = amb;

        // Diffuse
        vec4 Idiff = diff * max(dot(N,L), 0.0);

        // Specular
        vec4 Ispec = spec * pow(max(dot(R,E),0.0), shin);
 
        color = color + Iamb + Idiff + Ispec; 
        
        //////////////////////////////////////

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
            vec3 point = rayPlaneIntersect(vec3(mv * vec4(planeVectors[i], 1)), vertexPosition - cameraPosition, cameraPosition, vec3(mv * vec4(planePoints[i], 1)));
            //vec3 point = rayPlaneIntersect(vec3(transpose(inverse(mv)) * vec4(planeVectors[i], 0)), vertexPosition - cameraPosition, cameraPosition, vec3(mv * vec4(planePoints[i], 1)));
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
            fragColor = vec4(1, 0, 0, 1);
            return;
        }
        
        // Fog ratio
        float ratio = distanceInWater / longestDistance * fogDensity;
        if (ratio > 1)
            ratio = 1;

        fragColor = (1 - ratio) * color + ratio * vec4(0.38, 0.72, 0.75, 1);
	}
}
