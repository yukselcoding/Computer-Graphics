#version 410

layout(location = 0) in vec3 position;

// Data from CPU 
uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV; // ModelView idMVPMatrix
uniform vec3 cameraPosition;
uniform float heightFactor;

// Texture-related data
uniform sampler2D rgbTexture;
uniform int widthTexture;
uniform int heightTexture;


// Output to Fragment Shader
out vec2 textureCoordinate; // For texture-color
out vec3 vertexNormal; // For Lighting computation
out vec3 ToLightVector; // Vector from Vertex to Light;
out vec3 ToCameraVector; // Vector from Vertex to Camera;


float computeHeight(vec3 rgb){
    return (0.2126 * rgb.r + 0.7152 * rgb.g  + 0.0722 * rgb.b) * heightFactor;
}

vec3 helper(vec3 vertice, vec3 position){

    vec2 neighborCoordinate = vec2(1 - vertice.x / widthTexture, 1 - vertice.z / heightTexture);
    vec3 textureColor = texture(rgbTexture, neighborCoordinate).xyz;
    vertice.y = computeHeight(textureColor);
    return vec4(MVP * vec4(vertice,1.0)).xyz - position;

}


void main()
{
    

    textureCoordinate = vec2(1-position.x / widthTexture, 1-position.z / heightTexture);
    vec3 textureColor = texture(rgbTexture, textureCoordinate).xyz;
    // get texture value, compute height


    vec3 myPos = vec3(position.x, computeHeight(textureColor), position.z);
    

    vec3 vertice1 = vec3(position.x-1, heightFactor * myPos.y, position.z);
    vec3 vertice2 = vec3(position.x, heightFactor * myPos.y, position.z-1);
    vec3 vertice3 = vec3(position.x+1, heightFactor * myPos.y, position.z-1);
    vec3 vertice4 = vec3(position.x+1, heightFactor * myPos.y, position.z);
    vec3 vertice5 = vec3(position.x, heightFactor * myPos.y, position.z+1);
    vec3 vertice6 = vec3(position.x-1, heightFactor * myPos.y, position.z+1);


    vertice1 = helper(vertice1, myPos);
    vertice2 = helper(vertice2, myPos);
    vertice3 = helper(vertice3, myPos);
    vertice4 = helper(vertice4, myPos);
    vertice5 = helper(vertice5, myPos);
    vertice6 = helper(vertice6, myPos);


   
    vec3 totalNormal = cross(vertice1, vertice2);
    totalNormal += cross(vertice2, vertice3);
    totalNormal += cross(vertice3, vertice4);
    totalNormal += cross(vertice4, vertice5);
    totalNormal += cross(vertice5, vertice6);
    totalNormal += cross(vertice6, vertice1);
  

    vertexNormal = normalize( totalNormal / 6.0);


    ToCameraVector = normalize(cameraPosition - myPos);

        
    // compute normal vector using also the heights of neighbor vertices

    // compute toLight vector vertex coordinate in VCS
    
    ToLightVector = normalize(vec3(widthTexture / 2 - myPos.x , widthTexture + heightTexture - myPos.y , heightTexture / 2 - myPos.z));
    
    // set gl_Position variable correctly to give the transformed vertex position
    
    gl_Position =  MVP * vec4(myPos,1);
    // gl_Position = vec4(0,0,0,0); // this is a placeholder. It does not correctly set the position
   
}