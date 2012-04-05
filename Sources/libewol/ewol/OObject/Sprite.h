/**
 *******************************************************************************
 * @file ewol/OObject/Sprite.h
 * @brief ewol OpenGl Object system (header)
 * @author Edouard DUPIN
 * @date 04/04/2012
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

#ifndef __EWOL_O_OBJECT_SPRITE_H__
#define __EWOL_O_OBJECT_SPRITE_H__

#include <ewol/OObject.h>

namespace ewol {
	class Sprite :public ewol::OObject
	{
		private:
			etk::UString m_name;
		public:
			Sprite(etk::UString spriteName);
			Sprite(etk::UString spriteName, etkFloat_t sizeX, etkFloat_t sizeY);
			virtual ~Sprite(void);
		public:
			virtual void Draw(void);
			void Rectangle(etkFloat_t x, etkFloat_t y, etkFloat_t w, etkFloat_t h, etkFloat_t texX=0.0, etkFloat_t texY=0.0, etkFloat_t texSX=1.0, etkFloat_t texSY=1.0);
			void Element(coord2D_ts pos, etkFloat_t size, etkFloat_t angle);
		protected:
			int32_t                       m_textureId;   //!< texture internal ID
			etk::VectorType<coord2D_ts>   m_coord;       //!< internal coord of the object
			etk::VectorType<texCoord_ts>  m_coordTex;    //!< internal texture coordinate for every point
	};
};

#endif
