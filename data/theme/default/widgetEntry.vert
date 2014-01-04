#ifdef GL_ES
precision mediump float;
precision mediump int;
#endif

struct widgetStateProperty {
	int   stateOld;
	int   stateNew;
	float transition;
};

// Input :
attribute vec2 EW_coord2d;
uniform mat4   EW_MatrixTransformation;
uniform widgetStateProperty EW_status;

// output :
varying vec2  v_position;       // This will be passed into the fragment shader.
varying vec4  v_colorTansition;

// internal :
vec4  S_colorFg[3];

void main(void) {
	S_colorFg[0] = vec4(1.0,1.0,1.0,0.8);
	S_colorFg[1] = vec4(1.0,1.0,1.0,0.4);
	S_colorFg[2] = vec4(0.0,0.0,1.0,0.1);
	
	gl_Position = EW_MatrixTransformation * vec4(EW_coord2d, 0.0, 1.0);
	// transmit position of the curent element (intermolated ...)
	v_position = EW_coord2d;
	
	
	vec4 colorOld = S_colorFg[0];
	if(EW_status.stateOld==1) {
		colorOld = S_colorFg[1];
	} else if(EW_status.stateOld==2) {
		colorOld = S_colorFg[2];
	}
	vec4 colorNew = S_colorFg[0];
	if(EW_status.stateNew==1) {
		colorNew = S_colorFg[1];
	} else if(EW_status.stateNew==2) {
		colorNew = S_colorFg[2];
	}
	
	// note : int() is needed for the OpenGL ES platform
	v_colorTansition =   colorOld*(1.0-EW_status.transition)
	                   + colorNew*EW_status.transition;
}