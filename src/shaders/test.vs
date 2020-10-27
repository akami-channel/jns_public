#version 330 core
layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoord;

//out vec2 TexCoord;
out vec2 st_FragCoord;

// uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;

uniform float time;

uniform vec2 resolution;
uniform vec2 trans;

void main()
{
	// if( position.y < 0){
	// gl_Position = vec4(position.x * 2.0, position.y * 2.0, 0.0, 1.0);
	// }else {
	// 	gl_Position = vec4(position.x * 1.0, position.y * 2.0, 0.0, 1.0);
	// }

	gl_Position = vec4(position.x * 2.0 + trans.x, position.y * 2.0 + trans.y, position.z+0.20, 1.0);
	//gl_Position = vec4(position, 1.0);
	
// gl_Position = vec4(position.x * 2.0, position.y * 2.0, position.z, 1.0);

    //gl_Position = projection * view * model * vec4(position, 1.0f);
    // vec2 TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
    vec2 TexCoord = texCoord;
    // TexCoord = vec2(texCoord.x, texCoord.y);
    // st_FragCoord = vec2(texCoord.x * resolution.x, texCoord.y * resolution.y);
    st_FragCoord = vec2(TexCoord.x * resolution.x, TexCoord.y * resolution.y);
}