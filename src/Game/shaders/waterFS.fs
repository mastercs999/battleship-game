#version 130

// Texture
uniform sampler2D color_texture;
out vec4 fragColor;

// Light
in vec3 N;
in vec3 v;  

void main() {

        // Texture color
	vec4 newColor = texture(color_texture, gl_TexCoord[0].st);
	
	// Light
        vec3 lightPosition = vec3(5, 5, 5);
        vec4 ambient = vec4(vec3(0.7), 1);
        vec4 diffuse = vec4(vec3(0.5), 1);
        vec4 specular = vec4(vec3(1), 1);
        float shininess = 10.0f;
        
        vec3 L = normalize(lightPosition.xyz - v);   
        vec3 E = normalize(-v); 
        vec3 R = reflect(-L,N);  
        
        // Ambient
        vec4 Iamb = ambient;    

        // Diffuse
        vec4 Idiff = diffuse * max(dot(L,N), 0.0);    
        
        // Specular
        vec4 Ispec = specular * pow(max(dot(R,E),0.0), shininess);

        // Result 
        fragColor = (Iamb + Idiff) *newColor + Ispec; 
        fragColor.w = 0.7;
}