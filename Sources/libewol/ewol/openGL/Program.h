/**
 *******************************************************************************
 * @file ewol/openGL/Program.h
 * @brief ewol openGl Program shader system (header)
 * @author Edouard DUPIN
 * @date 24/08/2012
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

#ifndef __OPEN_GL__PROGRAM_H__
#define __OPEN_GL__PROGRAM_H__
	#ifdef __VIDEO__OPENGL_ES_2
		#include <etk/Types.h>
		#include <ewol/Debug.h>
		#include <ewol/Resource.h>
		#include <ewol/openGL/openGL.h>
		#include <ewol/openGL/Shader.h>
		
		namespace ewol
		{
			class progAttributeElement
			{
				public :
					etk::UString m_name;
					GLint        m_elementId;
					bool         m_isAttribute;
			};
			
			class Program : public ewol::Resource
			{
				private :
					GLuint                                  m_program;
					etk::Vector<ewol::Shader*>              m_shaderList;
					etk::Vector<ewol::progAttributeElement> m_elementList;
					bool                                    m_hasTexture;
				public:
					Program(etk::UString& filename);
					virtual ~Program(void);
					const char* GetType(void) { return "ewol::Program"; };
					bool CreateAndLink(void);
					int32_t GetAttribute(etk::UString tmpElement);
					void SendAttribute(int32_t idElem, int32_t nbElement, void* pointer, int32_t jumpBetweenSample=0);
					int32_t GetUniform(etk::UString tmpElement);
					void SendUniformMatrix4fv(int32_t idElem, int32_t nbElement, etk::Matrix pointer);
					void Use(void);
					void SetTexture0(int32_t idElem, GLint textureOpenGlID);
					void UnUse(void);
			};
		};
	#endif
#endif
