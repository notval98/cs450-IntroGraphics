#version 330 compatibility

uniform float	uTime;		// "Time", from Animate( )
uniform float	uBlend = 0.5; // ambient occlusion blend factor
uniform bool 	uDisplayFrag = true;
in vec2  	vST;		// texture coords
in vec3		vColor;		// vertex color

void
main( )
{
	if(uDisplayFrag) {
		vec3 myColor;
		
		if(uTime < (vST.t - .2) || vST.t < uTime) {
			myColor = vec3(1., 0., 0.);
		}
		else {
			myColor = vec3(0., 0., 1.);
		}
		
		vec3 finalColor = myColor * (1 - uBlend) + vColor * (uBlend);
		
		gl_FragColor = vec4(finalColor, 1.);
	}
	else {
		gl_FragColor = vec4(0.7, 0.7, 0.7, 1.);
	}
}