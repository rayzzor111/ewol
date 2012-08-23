/**
 *******************************************************************************
 * @file ewol/oObject/2DTextColored.h
 * @brief ewol OpenGl Object system (header)
 * @author Edouard DUPIN
 * @date 16/01/2012
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

#ifndef __EWOL_O_OBJECT_2D_TEXT_COLORED_H__
#define __EWOL_O_OBJECT_2D_TEXT_COLORED_H__

#include <ewol/oObject/OObject.h>
#include <ewol/font/TexturedFont.h>

namespace ewol {
	class OObject2DTextColored :public ewol::OObject
	{
		public:
			OObject2DTextColored(etk::UString FontName, int32_t size);
			OObject2DTextColored(void);
			virtual ~OObject2DTextColored(void);
		public:
			virtual void Draw(void);
			void SetColor(float red, float green, float blue, float alpha = 1.0);
			void SetColor(draw::Color color);
			// set a specific text
			void    Clear(void);
			int32_t Text(Vector2D<float>  textPos, const etk::UString& unicodeString);
			int32_t Text(Vector2D<float>  textPos, const uniChar_t     unicodeChar);
		protected:
			ewol::TexturedFont*             m_font;          //!< ewol font system
			draw::Color                     m_color;         //!< tmp text color ...
			etk::Vector<Vector2D<float> >   m_coord;         //!< internal coord of the object
			etk::Vector<texCoord_ts>        m_coordTex;      //!< internal texture coordinate for every point
			etk::Vector<draw::Color>        m_coordColor;    //!< internal color of the different point
		public:
			void SetFont(etk::UString fontName);
			void SetSize(int32_t fontSize);
			void SetFontProperty(etk::UString fontName, int32_t fontSize);
			int32_t GetHeight(void) { return (NULL!=m_font)?m_font->GetHeight():10; };
			Vector2D<float> GetSize(const uniChar_t unicodeChar);
			Vector2D<float> GetSize(const etk::UString& unicodeString);
	};
};

#endif
