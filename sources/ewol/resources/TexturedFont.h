/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license BSD v3 (see license file)
 */

#ifndef __EWOL_TEXTURED_FONT_H__
#define __EWOL_TEXTURED_FONT_H__

#include <ewol/resources/font/FontBase.h>
#include <ewol/resources/Texture.h>
#include <ewol/resources/Resource.h>

namespace ewol
{
	namespace font
	{
		typedef enum {
			Regular=0,
			Italic,
			Bold,
			BoldItalic,
		} mode_te;
	};
	etk::CCout& operator <<(etk::CCout& _os, const ewol::font::mode_te& _obj);
	
	class TexturedFont : public ewol::Texture {
		
		private:
			etk::UString m_fileName[4];
			int32_t m_size;
			int32_t m_height[4];
			// specific element to have the the know if the specify element is known...
			// ==> otherwise I can just generate italic ...
			// ==> Bold is a little more complicated (maybe with the bordersize)
			ewol::FontBase* m_font[4];
			ewol::font::mode_te m_modeWraping[4];     //!< This is a wrapping mode to prevent the fact that no font is define for a specific mode
		public:
			etk::Vector<GlyphProperty> m_listElement[4];
		private:
			// for the texture generation :
			ivec2 m_lastGlyphPos[4];
			int32_t m_lastRawHeigh[4];
		protected:
			TexturedFont(etk::UString fontName);
			~TexturedFont(void);
		public:
			virtual bool HasName(const etk::UString& fileName);
			const char* GetType(void) { return "ewol::TexturedFont"; };
			int32_t getFontSize(void) { return m_size; };
			/**
			 * @brief Get the display height of this font
			 * @param[in] displayMode Mode to display the currrent font
			 * @return Dimention of the font need between 2 lines
			 */
			int32_t GetHeight(const ewol::font::mode_te displayMode = ewol::font::Regular) { return m_height[displayMode]; };
			/**
			 * @brief Get the font height (user friendly)
			 * @return Dimention of the font the user requested
			 */
			int32_t GetFontSize(void) { return m_size; };
			/**
			 * @brief Get the ID of a unicode charcode
			 * @param[in] charcode The unicodeValue
			 * @param[in] displayMode Mode to display the currrent font
			 * @return The ID in the table (if it does not exist : return 0)
			 */
			int32_t GetIndex(const uniChar_t& charcode, const ewol::font::mode_te displayMode) const;
			/**
			 * @brief Get the pointer on the coresponding glyph
			 * @param[in] charcode The unicodeValue
			 * @param[in] displayMode Mode to display the currrent font
			 * @return The pointer on the glyph ==> never NULL
			 */
			ewol::GlyphProperty* GetGlyphPointer(const uniChar_t& charcode, const ewol::font::mode_te displayMode);
			/**
			 * @brief The wrapping mode is used to prevent the non existance of a specific mode.
			 *        For exemple when a blod mode does not exist, this resend a regular mode.
			 * @param[in] source The requested mode.
			 * @return the best mode we have in stock.
			 */
			ewol::font::mode_te GetWrappingMode(ewol::font::mode_te source) { return m_modeWraping[source]; };
		public:
			/**
			 * @brief Keep the resource pointer.
			 * @note Never free this pointer by your own...
			 * @param[in] _filename Name of the texture font.
			 * @return pointer on the resource or NULL if an error occured.
			 */
			static ewol::TexturedFont* Keep(const etk::UString& _filename);
			/**
			 * @brief Release the keeped resources
			 * @param[in,out] reference on the object pointer
			 */
			static void Release(ewol::TexturedFont*& _object);
	};
	
	
};

#endif
