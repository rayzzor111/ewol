/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#ifndef __EWOL_DISTANCE_FIELD_FONT_H__
#define __EWOL_DISTANCE_FIELD_FONT_H__

#include <ewol/resource/font/FontBase.h>
#include <ewol/resource/Texture.h>
#include <ewol/resource/Resource.h>
#include <ewol/resource/TexturedFont.h>

namespace ewol {
	namespace resource {
		class DistanceFieldFont : public ewol::resource::Texture {
			private:
				std::string m_fileName;
				float m_sizeRatio;
				// specific element to have the the know if the specify element is known...
				//  == > otherwise I can just generate italic ...
				//  == > Bold is a little more complicated (maybe with the bordersize)
				std::shared_ptr<ewol::resource::FontBase> m_font;
			public:
				std::vector<GlyphProperty> m_listElement;
			private:
				// for the texture generation :
				ivec2 m_lastGlyphPos;
				int32_t m_lastRawHeigh;
			protected:
				DistanceFieldFont();
				void init(const std::string& _fontName);
			public:
				DECLARE_RESOURCE_NAMED_FACTORY(DistanceFieldFont);
				virtual ~DistanceFieldFont();
			public:
				float getDisplayRatio(float _size);
				/**
				 * @brief get the display height of this font
				 * @param[in] _size Request font size
				 * @return Dimention of the font need between 2 lines
				 */
				float getHeight(float _size) {
					return ((float)m_font->getHeight(_size));
				};
				/**
				 * @brief get the font size with a specific display size
				 * @param[in] _fontHeight Request font height
				 * @return Dimention of the font for this compleate line size.
				 */
				float getSize(float _fontHeight) {
					return m_font->getSizeWithHeight(_fontHeight);
				}
				/**
				 * @brief get the ID of a unicode charcode
				 * @param[in] _charcode The unicodeValue
				 * @return The ID in the table (if it does not exist : return 0)
				 */
				int32_t getIndex(char32_t _charcode);
				/**
				 * @brief get the pointer on the coresponding glyph
				 * @param[in] _charcode The unicodeValue
				 * @return The pointer on the glyph  == > never nullptr
				 */
				ewol::GlyphProperty* getGlyphPointer(const char32_t& _charcode);
			public:
				/**
				 * @brief keep the resource pointer.
				 * @note Never free this pointer by your own...
				 * @param[in] _filename Name of the texture font.
				 * @return pointer on the resource or nullptr if an error occured.
				 */
				static std::shared_ptr<ewol::resource::DistanceFieldFont> keep(const std::string& _filename);
			private:
				/**
				 * @brief add a glyph in a texture font.
				 * @param[in] _val Char value to add.
				 * @return true if the image size have change, false otherwise
				 */
				bool addGlyph(const char32_t& _val);
				
				void generateDistanceField(const egami::ImageMono& _input, egami::Image& _output);
			private:
				float m_borderSize; //!< number of pixel added on the border of a glyph
				vec2 m_textureBorderSize; //!< Transformed the border size in the texture dimention
			public:
				float getPixelBorderSize() {
					return m_borderSize;
				}
				const vec2& getTextureBorderSize() {
					return m_textureBorderSize;
				}
			public:
				void exportOnFile();
				bool importFromFile();
		};
	};
};

#endif

