/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license BSD v3 (see license file)
 */


#include <ewol/Debug.h>
#include <ewol/compositing/Text.h>
#include <ewol/font/FontManager.h>


/*
			// curent Drawing position
			etk::Vector3D<float> m_position;         //!< the next position to draw the text
			// clipping section
			etk::Vector3D<float> m_clippingPosition;
			etk::Vector3D<float> m_clippingSize;
			bool                 m_clippingEnable;
			// Basic color
			etk::Color           m_color;
*/

ewol::Text::Text(void) :
	m_position(0.0, 0.0, 0.0),
	m_clippingPosition(0.0, 0.0, 0.0),
	m_clippingSize(0.0, 0.0, 0.0),
	m_clippingEnable(false),
	m_color(draw::color::black),
	m_colorBg(draw::color::none),
	m_mode(ewol::font::Regular),
	m_kerning(true),
	m_distanceField(false),
	m_previousCharcode(0),
	m_startTextpos(0),
	m_stopTextPos(0),
	m_alignement(ewol::Text::alignDisable),
	m_GLprogram(NULL),
	m_GLPosition(-1),
	m_GLMatrix(-1),
	m_GLColor(-1),
	m_GLtexture(-1),
	m_GLtexID(-1),
	m_font(NULL)
{
	SetFont(ewol::font::GetDefaultFont(), ewol::font::GetDefaultSize());
	LoadProgram();
}


ewol::Text::Text(etk::UString fontName, int32_t fontSize) :
	m_position(0.0, 0.0, 0.0),
	m_clippingPosition(0.0, 0.0, 0.0),
	m_clippingSize(0.0, 0.0, 0.0),
	m_clippingEnable(false),
	m_color(draw::color::black),
	m_colorBg(draw::color::none),
	m_mode(ewol::font::Regular),
	m_kerning(true),
	m_distanceField(false),
	m_previousCharcode(0),
	m_startTextpos(0),
	m_stopTextPos(0),
	m_alignement(ewol::Text::alignDisable),
	m_GLprogram(NULL),
	m_GLPosition(-1),
	m_GLMatrix(-1),
	m_GLColor(-1),
	m_GLtexture(-1),
	m_GLtexID(-1),
	m_font(NULL)
{
	SetFont(fontName, fontSize);
	LoadProgram();
}


ewol::Text::~Text(void)
{
	
	if (NULL != m_font) {
		ewol::resource::Release(m_font);
		m_font = NULL;
	}
	ewol::resource::Release(m_GLprogram);
}

void ewol::Text::LoadProgram(void)
{
	etk::UString tmpString("DATA:text.prog");
	// get the shader resource :
	m_GLPosition = 0;
	if (true == ewol::resource::Keep(tmpString, m_GLprogram) ) {
		m_GLPosition = m_GLprogram->GetAttribute("EW_coord2d");
		m_GLColor    = m_GLprogram->GetAttribute("EW_color");
		m_GLtexture  = m_GLprogram->GetAttribute("EW_texture2d");
		m_GLMatrix   = m_GLprogram->GetUniform("EW_MatrixTransformation");
		m_GLtexID    = m_GLprogram->GetUniform("EW_texID");
	}
}

void ewol::Text::Draw(void)
{
	if (m_coord.Size()<=0 || NULL == m_font) {
		// TODO : a remètre ...
		//EWOL_WARNING("Nothink to draw...");
		return;
	}
	if (m_font == NULL) {
		EWOL_WARNING("no font...");
		return;
	}
	if (m_GLprogram==NULL) {
		EWOL_ERROR("No shader ...");
		return;
	}
	m_GLprogram->Use();
	// set Matrix : translation/positionMatrix
	etk::Matrix4 tmpMatrix = ewol::openGL::GetMatrix()*m_matrixApply;
	m_GLprogram->UniformMatrix4fv(m_GLMatrix, 1, tmpMatrix.m_mat);
	// TextureID
	m_GLprogram->SetTexture0(m_GLtexID, m_font->GetId());
	// position :
	m_GLprogram->SendAttribute(m_GLPosition, 2/*x,y*/, &m_coord[0]);
	// Texture :
	m_GLprogram->SendAttribute(m_GLtexture, 2/*u,v*/, &m_coordTex[0]);
	// color :
	m_GLprogram->SendAttribute(m_GLColor, 4/*r,g,b,a*/, &m_coordColor[0]);
	// Request the draw od the elements : 
	glDrawArrays(GL_TRIANGLES, 0, m_coord.Size());
	m_GLprogram->UnUse();
}


void ewol::Text::Clear(void)
{
	// call upper class
	ewol::Compositing::Clear();
	// Reset Buffer :
	m_coord.Clear();
	m_coordTex.Clear();
	m_coordColor.Clear();
	// Reset temporal variables :
	m_position = etk::Vector3D<float>(0.0, 0.0, 0.0);
	m_clippingPosition = etk::Vector3D<float>(0.0, 0.0, 0.0);
	m_clippingSize = etk::Vector3D<float>(0.0, 0.0, 0.0);
	m_clippingEnable = false;
	m_color = draw::color::black;
	m_colorBg = draw::color::none;
	m_mode = ewol::font::Regular;
	m_kerning = false;
	m_previousCharcode = 0;
	m_startTextpos = 0;
	m_stopTextPos = 0;
	m_alignement = ewol::Text::alignDisable;
}


void ewol::Text::SetPos(etk::Vector3D<float> pos)
{
	m_position = pos;
	m_previousCharcode = 0;
}


void ewol::Text::SetRelPos(etk::Vector3D<float> pos)
{
	m_position += pos;
	m_previousCharcode = 0;
}


void ewol::Text::SetColor(draw::Color color)
{
	m_color = color;
}


void ewol::Text::SetColorBG(draw::Color color)
{
	m_colorBg = color;
}


void ewol::Text::SetClipping(etk::Vector3D<float> pos, etk::Vector3D<float> width)
{
	m_clippingPosition = pos;
	m_clippingSize = width;
	m_clippingEnable = true;
}


void ewol::Text::SetClippingMode(bool newMode)
{
	m_clippingEnable = newMode;
}


void ewol::Text::SetFontSize(int32_t fontSize)
{
	// get old size
	etk::UString fontName = ewol::font::GetDefaultFont();
	if (NULL != m_font) {
		fontName = m_font->GetName();
	}
	SetFont(fontName, fontSize);
}


void ewol::Text::SetFontName(etk::UString fontName)
{
	// get old size
	int32_t fontSize = ewol::font::GetDefaultSize();
	if (NULL != m_font) {
		fontSize = m_font->GetFontSize();
	}
	SetFont(fontName, fontSize);
}


void ewol::Text::SetFont(etk::UString fontName, int32_t fontSize)
{
	// remove old one
	if (NULL != m_font) {
		ewol::resource::Release(m_font);
		m_font = NULL;
	}
	etk::UString tmpName = fontName;
	tmpName += ":";
	tmpName += fontSize;
	// link to new One
	if (false == ewol::resource::Keep(tmpName, m_font)) {
		EWOL_ERROR("Can not get font resource");
	}
}


void ewol::Text::SetFontMode(ewol::font::mode_te mode)
{
	m_mode = mode;
}


void ewol::Text::SetKerningMode(bool newMode)
{
	m_kerning = newMode;
}


void ewol::Text::SetDistanceFieldMode(bool newMode)
{
	m_distanceField = newMode;
	EWOL_TODO("The Distance field mode is not availlable for now ...");
}


void ewol::Text::Print(const etk::UString& text)
{
	if (m_font == NULL) {
		EWOL_ERROR("Font Id is not corectly defined");
		return;
	}
	switch(m_alignement)
	{
		default:
		case ewol::Text::alignDisable:
			for(int32_t iii=0; iii<text.Size(); iii++) {
				Print(text[iii]);
			}
			break;
		case ewol::Text::alignJustify:
			{
				float basicSpaceWidth = CalculateSize(' ').x;
				int32_t currentId = 0;
				int32_t stop;
				int32_t space;
				int32_t freeSpace;
				while (currentId < text.Size()) {
					bool needNoJustify = ExtrapolateLastId(text, currentId, stop, space, freeSpace);
					
					float interpolation = basicSpaceWidth;
					if (needNoJustify == false) {
						interpolation += (float)freeSpace / (float)(space-1);
					}
					for(int32_t iii=currentId; iii<stop && iii<text.Size(); iii++) {
						if (text[iii] == (uniChar_t)' ') {
							// Must generate a dynamic space : 
							SetPos(etk::Vector3D<float>(m_position.x + interpolation,
							                            m_position.y,
							                            m_position.z) );
						} else {
							Print(text[iii]);
						}
					}
					if (currentId == stop) {
						currentId++;
					} else if(    text[stop] == (uniChar_t)' '
					           || text[stop] == (uniChar_t)'\n') {
						currentId = stop+1;
					} else {
						currentId = stop;
					}
					// Reset position : 
					SetPos(etk::Vector3D<float>(m_startTextpos,
					                            (float)(m_position.y - m_font->GetHeight(m_mode)),
					                            m_position.z) );
				}
			}
			break;
	}
}


void ewol::Text::PrintDecorated(const etk::UString& text)
{
	EWOL_TODO("The Decorated print is not supported now ...");
	Print(text);
}


void ewol::Text::Print(const etk::UString& text, const etk::Vector<TextDecoration>& decoration)
{
	EWOL_TODO("The Advenced print is not supported now ...");
	Print(text);
}


void ewol::Text::Print(const uniChar_t charcode)
{
	if (m_font == NULL) {
		EWOL_ERROR("Font Id is not corectly defined");
		return;
	}
	// get a pointer on the glyph property : 
	ewol::GlyphProperty * myGlyph = m_font->GetGlyphPointer(charcode, m_mode);
	int32_t fontSize = m_font->GetFontSize();
	int32_t fontHeigh = m_font->GetHeight(m_mode);
	
	// Get the kerning ofset :
	float kerningOffset = 0.0;
	if (true==m_kerning) {
		kerningOffset = myGlyph->KerningGet(m_previousCharcode);
		if (kerningOffset != 0) {
			//EWOL_DEBUG("Kerning between : '" << (char)m_previousCharcode << "'&'" << (char)myGlyph->m_UVal << "' value : " << kerningOffset);
		}
	}
	// 0x01 == 0x20 == ' ';
	if (charcode != 0x01) {
		/* Bitmap position
		 *      xA     xB
		 *   yC *------*
		 *      |      |
		 *      |      |
		 *   yD *------*
		 */
		float dxA = m_position.x + myGlyph->m_bearing.x + kerningOffset;
		float dxB = dxA + myGlyph->m_sizeTexture.x;
		float dyC = m_position.y + myGlyph->m_bearing.y + fontHeigh - fontSize;
		float dyD = dyC - myGlyph->m_sizeTexture.y;
		
		float tuA = myGlyph->m_texturePosStart.u;
		float tuB = myGlyph->m_texturePosStop.u;
		float tvC = myGlyph->m_texturePosStart.v;
		float tvD = myGlyph->m_texturePosStop.v;
		
		
		// Clipping and drawing area
		/*
		if(     true == hasClipping
		    && (    dxB < clipping.x
		         || dxA > clipping.x + clipping.w) ) {
			// Nothing to diplay ...
		} else {
			if (true == hasClipping) {
				// generata positions...
				float TexSizeX = tuB - tuA;
				if (dxA < clipping.x) {
					// clip display
					float drawSize = clipping.x - dxA;
					// Update element start display
					dxA = clipping.x;
					float addElement = TexSizeX * drawSize / (float)myGlyph->m_sizeTexture.x;
					// update texture start X Pos
					tuA += addElement;
				}
				if (dxB > clipping.x + clipping.w) {
					// clip display
					float drawSize = dxB - (clipping.x + clipping.w);
					// Update element start display
					dxB = clipping.x + clipping.w;
					float addElement = TexSizeX * drawSize / (float)myGlyph->m_sizeTexture.x;
					// update texture start X Pos
					tuB -= addElement;
				}
				float TexSizeY = tvD - tvC;
				if (dyC < clipping.y) {
					// clip display
					float drawSize = clipping.y - dyC;
					// Update element start display
					dyC = clipping.y;
					float addElement = TexSizeY * drawSize / (float)myGlyph->m_sizeTexture.x;
					// update texture start X Pos
					tvC += addElement;
				}
				if (dyD > clipping.y + clipping.h) {
					// clip display
					float drawSize = dyD - (clipping.y + clipping.h);
					// Update element start display
					dyD = clipping.y + clipping.h;
					float addElement = TexSizeX * drawSize / (float)myGlyph->m_sizeTexture.x;
					// update texture start X Pos
					tvD -= addElement;
				}
			}
		*/
			if(    dxB <= dxA
			    || dyD >= dyC) {
				// nothing to do ...
			} else {
				/* Bitmap position
				 *   0------1
				 *   |      |
				 *   |      |
				 *   3------2
				 */
				etk::Vector2D<int32_t> bitmapDrawPos[4];
				bitmapDrawPos[0].x = (int32_t)dxA;
				bitmapDrawPos[1].x = (int32_t)dxB;
				bitmapDrawPos[2].x = (int32_t)dxB;
				bitmapDrawPos[3].x = (int32_t)dxA;
				
				bitmapDrawPos[0].y = (int32_t)dyC;
				bitmapDrawPos[1].y = (int32_t)dyC;
				bitmapDrawPos[2].y = (int32_t)dyD;
				bitmapDrawPos[3].y = (int32_t)dyD;
				/* texture Position : 
				 *   0------1
				 *   |      |
				 *   |      |
				 *   3------2
				 */
				texCoord_ts texturePos[4];
				texturePos[0].u = tuA+m_mode;
				texturePos[1].u = tuB+m_mode;
				texturePos[2].u = tuB+m_mode;
				texturePos[3].u = tuA+m_mode;
				
				texturePos[0].v = tvC;
				texturePos[1].v = tvC;
				texturePos[2].v = tvD;
				texturePos[3].v = tvD;
				
				// NOTE : Android does not support the Quads elements ...
				/* Step 1 : 
				 *   ********     
				 *     ******     
				 *       ****     
				 *         **     
				 *                
				 */
				// set texture coordonates :
				m_coordTex.PushBack(texturePos[0]);
				m_coordTex.PushBack(texturePos[1]);
				m_coordTex.PushBack(texturePos[2]);
				// set display positions :
				m_coord.PushBack(bitmapDrawPos[0]);
				m_coord.PushBack(bitmapDrawPos[1]);
				m_coord.PushBack(bitmapDrawPos[2]);
				// set the color
				m_coordColor.PushBack(m_color);
				m_coordColor.PushBack(m_color);
				m_coordColor.PushBack(m_color);
				
				/* Step 2 : 
				 *              
				 *   **         
				 *   ****       
				 *   ******     
				 *   ********   
				 */
				// set texture coordonates :
				m_coordTex.PushBack(texturePos[0]);
				m_coordTex.PushBack(texturePos[2]);
				m_coordTex.PushBack(texturePos[3]);
				// set display positions :
				m_coord.PushBack(bitmapDrawPos[0]);
				m_coord.PushBack(bitmapDrawPos[2]);
				m_coord.PushBack(bitmapDrawPos[3]);
				// set the color
				m_coordColor.PushBack(m_color);
				m_coordColor.PushBack(m_color);
				m_coordColor.PushBack(m_color);
			}
		//}
	}
	// move the position :
	m_position.x += myGlyph->m_advance.x + kerningOffset;
	// Register the previous character
	m_previousCharcode = charcode;
	return;
}


void ewol::Text::SetTextAlignement(float startTextpos, float stopTextPos, aligneMode_te alignement)
{
	m_startTextpos = startTextpos;
	m_stopTextPos = stopTextPos;
	m_alignement = alignement;
	if (m_startTextpos >= m_stopTextPos) {
		EWOL_ERROR("Request allignement with Borne position error : " << startTextpos << " => " << stopTextPos);
	}
}


void ewol::Text::DisableAlignement(void)
{
	m_alignement = ewol::Text::alignDisable;
}

etk::Vector3D<float> ewol::Text::CalculateSize(const etk::UString& text)
{
	if (m_font == NULL) {
		EWOL_ERROR("Font Id is not corectly defined");
		return etk::Vector3D<float>(0,0,0);
	}
	etk::Vector3D<float> outputSize(0, 0, 0);
	for(int32_t iii=0; iii<text.Size(); iii++) {
		etk::Vector3D<float> tmpp = CalculateSize(text[iii]);
		if (outputSize.y == 0) {
			outputSize.y += tmpp.y;
		}
		outputSize.x += tmpp.x;
	}
	return outputSize;
}

etk::Vector3D<float> ewol::Text::CalculateSize(const uniChar_t charcode)
{
	if (m_font == NULL) {
		EWOL_ERROR("Font Id is not corectly defined");
		return etk::Vector3D<float>(0,0,0);
	}
	// get a pointer on the glyph property : 
	ewol::GlyphProperty * myGlyph = m_font->GetGlyphPointer(charcode, m_mode);
	int32_t fontHeigh = m_font->GetHeight(m_mode);
	
	// Get the kerning ofset :
	float kerningOffset = 0.0;
	if (true==m_kerning) {
		kerningOffset = myGlyph->KerningGet(m_previousCharcode);
	}
	
	etk::Vector3D<float> outputSize((float)(myGlyph->m_advance.x + kerningOffset),
	                                (float)(fontHeigh),
	                                (float)(0.0));
	// Register the previous character
	m_previousCharcode = charcode;
	return outputSize;
}


bool ewol::Text::ExtrapolateLastId(const etk::UString& text, const int32_t start, int32_t& stop, int32_t& space, int32_t& freeSpace)
{
	// store previous :
	uniChar_t storePrevious = m_previousCharcode;
	
	stop = text.Size();
	space = 0;
	
	int32_t lastSpacePosition = start;
	int32_t lastSpacefreeSize;
	
	float endPos = m_position.x;
	bool endOfLine = false;
	for (int32_t iii=start; iii<text.Size(); iii++) {
		etk::Vector3D<float> tmpSize = CalculateSize(text[iii]);
		// check oveflow :
		if (endPos + tmpSize.x > m_stopTextPos) {
			stop = iii;
			break;
		}
		// update local size :
		endPos += tmpSize.x;
		// save number of space :
		if (text[iii] == (uniChar_t)' ') {
			space++;
			lastSpacePosition = iii;
			lastSpacefreeSize = m_stopTextPos - endPos;
		} else if (text[iii] == (uniChar_t)'\n') {
			stop = iii;
			endOfLine = true;
			break;
		}
	}
	freeSpace = m_stopTextPos - endPos;
	// retore previous :
	m_previousCharcode = storePrevious;
	// need to align left or right ...
	if(stop == text.Size()) {
		return true;
	} else {
		if (endOfLine) {
			return true;
		} else {
			if (space == 0) {
				return true;
			}
			stop = lastSpacePosition;
			freeSpace = lastSpacefreeSize;
			return false;
		}
	}
}


