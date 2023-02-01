#version 330 compatibility

uniform float	uTime;		// "Time", from Animate( )
uniform bool	uDisplayVert = true;
out vec2  	vST;		// texture coords
out vec3	vColor;		// vertex color

const float PI = 	3.14159265;
const float AMP = 	0.1;		// amplitude


void
main( )
{ 
	vST = gl_MultiTexCoord0.st;
	vec3 vert = gl_Vertex.xyz;
	
	if(uDisplayVert) {
		float displacement = sin(2 * PI * (vert.y - uTime)) * AMP;
		
		vColor = vec3(((displacement / AMP) + 1) / 2.);
		
		vec3 newPosition = vert + gl_Normal * displacement;
		
		gl_Position = gl_ModelViewProjectionMatrix * vec4(newPosition, 1.);
	}
	else {
		vColor = vec3(0.5, 0.5, 0.5);
		gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	}
}