/**
 *******************************************************************************
 * @file ewol/openGl.h
 * @brief ewol openGl abstarction (header)
 * @author Edouard DUPIN
 * @date 19/09/2012
 * @par Project
 * ewol
 *
 * @par Copyright
 * Copyright 2011 Edouard DUPIN, all right reserved
 *
 * This software is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY.
 *
 * Licence summary : 
 *    You can modify and redistribute the sources code and binaries.
 *    You can send me the bug-fix
 *
 * Term of the licence in in the file licence.txt.
 *
 *******************************************************************************
 */

#ifndef __OPEN_GL_H__
#define __OPEN_GL_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__TARGET_OS__Linux)
	#include <GL/gl.h>
	#include <GL/glu.h>
#elif defined(__TARGET_OS__Android)
	#include <GLES/gl.h>
#elif defined(__TARGET_OS__Windows)
	#include <GL/gl.h>
#elif defined(__TARGET_OS__MacOs)
	
#elif defined(__TARGET_OS__IOs)
	
#else
	#error you need to specify a __TAGET_OS__ ...
#endif

void glOrthoEwol(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat nearVal, GLfloat farVal);

#ifdef __cplusplus
}
#endif


#endif
