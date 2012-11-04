/**
 *******************************************************************************
 * @file ewol/widget/Button.cpp
 * @brief ewol Button widget system (Sources)
 * @author Edouard DUPIN
 * @date 07/11/2011
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

#include <ewol/widget/Button.h>

#include <ewol/oObject/OObject.h>
#include <ewol/widget/WidgetManager.h>


extern const char * const ewolEventButtonPressed    = "ewol-button-Pressed";
extern const char * const ewolEventButtonDown       = "ewol-button-down";
extern const char * const ewolEventButtonUp         = "ewol-button-up";
extern const char * const ewolEventButtonEnter      = "ewol-button-enter";
extern const char * const ewolEventButtonLeave      = "ewol-button-leave";


/**
 * @brief Initilise the basic widget property ==> due to the android system
 * @note all widget that have template might have this initializer ...
 * @param ---
 * @return ---
 */
void ewol::WIDGET_ButtonInit(void)
{
	
}

#undef __class__
#define __class__	"Button"


void ewol::Button::Init(void)
{
	m_oObjectImage=NULL;
	AddEventId(ewolEventButtonPressed);
	AddEventId(ewolEventButtonDown);
	AddEventId(ewolEventButtonUp);
	AddEventId(ewolEventButtonEnter);
	AddEventId(ewolEventButtonLeave);
	m_hasAnImage = false;
	m_alignement = ewol::TEXT_ALIGN_CENTER;
	
	#ifdef __TARGET_OS__Android
		m_padding.y = 12;
		m_padding.x = 12;
	#else
		m_padding.y = 4;
		m_padding.x = 4;
	#endif
	
	m_textColorFg = draw::color::black;
	
	m_textColorBg = draw::color::black;
	m_textColorBg.a = 0x3F;
	SetCanHaveFocus(true);
	#ifdef __VIDEO__OPENGL_ES_2
		etk::UString tmpString("THEME:rounded:widgetButton.prog");
		// get the shader resource :
		m_GLPosition = 0;
		if (true == ewol::resource::Keep(tmpString, m_GLprogram) ) {
			m_GLPosition     = m_GLprogram->GetAttribute("EW_coord2d");
			m_GLMatrix       = m_GLprogram->GetUniform("EW_MatrixTransformation");
			m_GLsizeBorder   = m_GLprogram->GetUniform("EW_sizeBorder");
			m_GLsizePadding  = m_GLprogram->GetUniform("EW_sizePadding");
			m_GLsize         = m_GLprogram->GetUniform("EW_size");
			m_GLposText      = m_GLprogram->GetUniform("EW_posText");
			m_GLstate        = m_GLprogram->GetUniform("EW_state");
		}
	#endif
}

ewol::Button::Button(void)
{
	m_label = "No Label";
	Init();
}

ewol::Button::Button(etk::UString newLabel)
{
	m_label = newLabel;
	Init();
}


ewol::Button::~Button(void)
{
	
}

void ewol::Button::SetImage(etk::UString imageName)
{
	if (imageName == "") {
		m_hasAnImage = false;
	} else {
		m_imageSelected = imageName;
		m_hasAnImage = true;
	}
	MarkToRedraw();
}


void ewol::Button::SetPadding(etk::Vector2D<float> newPadding)
{
	m_padding = newPadding;
}

bool ewol::Button::CalculateMinSize(void)
{
	etk::Vector2D<int32_t> minSize = m_oObjectText.GetSize(m_label);
	m_minSize.x = m_padding.x*2 + minSize.x;
	m_minSize.y = m_padding.y*2 + minSize.y;
	// Add the image element ...
	if (true == m_hasAnImage) {
		//m_minSize.x += -m_padding.x + m_padding.y*2 + minHeight;
		//m_minSize.y += m_padding.y*2;
		m_minSize.x += m_padding.x + minSize.y;
	}
	
	MarkToRedraw();
	return true;
}


void ewol::Button::SetLabel(etk::UString newLabel)
{
	m_label = newLabel;
	MarkToRedraw();
}

void ewol::Button::SetValue(bool val)
{
	
}

void ewol::Button::SetAlignement(textAlignement_te typeAlign)
{
	m_alignement = typeAlign;
	MarkToRedraw();
}


bool ewol::Button::GetValue(void)
{
	return false;
}

#ifdef __VIDEO__OPENGL_ES_2

	void ewol::Button::SetPoint(float x, float y)
	{
		etk::Vector2D<float> triangle(x, y);
		m_coord.PushBack(triangle);
	}
	
	void ewol::Button::Rectangle(float x, float y, float w, float h)
	{
		m_coord.Clear();
		/* Bitmap position
		 *      xA     xB
		 *   yC *------*
		 *      |      |
		 *      |      |
		 *   yD *------*
		 */
		float dxA = x;
		float dxB = x + w;
		float dyC = y;
		float dyD = y + h;
		SetPoint(dxA, dyD);
		SetPoint(dxA, dyC);
		SetPoint(dxB, dyC);
	
		SetPoint(dxB, dyC);
		SetPoint(dxB, dyD);
		SetPoint(dxA, dyD);
	}
#endif


void ewol::Button::OnDraw(DrawProperty& displayProp)
{
	#ifdef __VIDEO__OPENGL_ES_2
		if (m_GLprogram==NULL) {
			EWOL_ERROR("No shader ...");
			return;
		}
		//glScalef(m_scaling.x, m_scaling.y, 1.0);
		m_GLprogram->Use();
		// set Matrix : translation/positionMatrix
		etk::Matrix4 tmpMatrix = ewol::openGL::GetMatrix();
		m_GLprogram->UniformMatrix4fv(m_GLMatrix, 1, tmpMatrix.m_mat);
		// position :
		m_GLprogram->SendAttribute(m_GLPosition, 2/*x,y*/, &m_coord[0]);
		// all entry parameters :
		m_GLprogram->Uniform1f(m_GLsizeBorder, 3);
		m_GLprogram->Uniform1f(m_GLsizePadding, m_padding.x);
		m_GLprogram->Uniform2fv(m_GLsize, 1, &m_size.x);
		m_GLprogram->Uniform4fv(m_GLposText, 1, m_pos);
		m_GLprogram->Uniform1i(m_GLstate, 0);
		// Request the draw of the elements : 
		glDrawArrays(GL_TRIANGLES, 0, m_coord.Size());
		m_GLprogram->UnUse();
	#endif
	m_oObjectDecoration.Draw();
	if (NULL != m_oObjectImage) {
		m_oObjectImage->Draw();
	}
	m_oObjectText.Draw();
}

void ewol::Button::OnRegenerateDisplay(void)
{
	if (true == NeedRedraw()) {
		m_oObjectDecoration.Clear();
		m_oObjectText.Clear();
		if (NULL != m_oObjectImage) {
			m_oObjectImage->Clear();
		}
		int32_t tmpSizeX = m_minSize.x;
		int32_t tmpSizeY = m_minSize.y;
		int32_t tmpOriginX = (m_size.x - m_minSize.x) / 2;
		int32_t tmpOriginY = (m_size.y - m_minSize.y) / 2;
		// no change for the text orogin : 
		int32_t tmpTextOriginX = (m_size.x - m_minSize.x) / 2 + m_padding.x;
		int32_t tmpTextOriginY = (m_size.y - m_minSize.y) / 2 + m_padding.y;
		
		if (true==m_userFill.x) {
			tmpSizeX = m_size.x;
			tmpOriginX = 0;
			if (m_alignement == ewol::TEXT_ALIGN_LEFT) {
				tmpTextOriginX = m_padding.x;
			}
		}
		if (true==m_userFill.y) {
			tmpSizeY = m_size.y;
			tmpOriginY = 0;
		}
		tmpOriginX += m_padding.x;
		tmpOriginY += m_padding.y;
		tmpSizeX -= 2*m_padding.x;
		tmpSizeY -= 2*m_padding.y;
		
		etk::Vector2D<float> textPos(tmpTextOriginX, tmpTextOriginY);
		
		/*ewol::OObject2DTextured * tmpImage = NULL;
		if (true == m_hasAnImage) {
			int32_t fontId = GetDefaultFontId();
			int32_t fontHeight = ewol::GetHeight(fontId);
			tmpImage = new ewol::OObject2DTextured(m_imageSelected, fontHeight, fontHeight);
			tmpImage->Rectangle(textPos.x, textPos.y, fontHeight, fontHeight);
			// update the text position ...
			textPos.x += m_padding.x + fontHeight;
		}
		*/
		clipping_ts drawClipping;
		drawClipping.x = m_padding.x;
		drawClipping.y = m_padding.y;
		drawClipping.w = m_size.x - 2*m_padding.x;
		drawClipping.h = m_size.y - 2*m_padding.y;
		//EWOL_DEBUG("draw tex at pos : " <<textPos << "in element size:" << m_size);
		m_oObjectText.Text(textPos/*, drawClipping*/, m_label);
		
		#ifndef __VIDEO__OPENGL_ES_2
			m_oObjectDecoration.SetColor(m_textColorBg);
			tmpOriginX -= m_padding.x/2;
			tmpOriginY -= m_padding.y/2;
			tmpSizeX += m_padding.x/1;
			tmpSizeY += m_padding.y/1;
			m_oObjectDecoration.Rectangle( tmpOriginX, tmpOriginY, tmpSizeX, tmpSizeY);
		#else
			Rectangle(0, 0, m_size.x, m_size.y);
		#endif
	}
}

/**
 * @brief Event on an input of this Widget
 * @param[in] type Type of the input (ewol::INPUT_TYPE_MOUSE/ewol::INPUT_TYPE_FINGER ...)
 * @param[in] IdInput Id of the current Input (PC : left=1, right=2, middle=3, none=0 / Tactil : first finger=1 , second=2 (only on this widget, no knowledge at ouside finger))
 * @param[in] typeEvent ewol type of event like EVENT_INPUT_TYPE_DOWN/EVENT_INPUT_TYPE_MOVE/EVENT_INPUT_TYPE_UP/EVENT_INPUT_TYPE_SINGLE/EVENT_INPUT_TYPE_DOUBLE/...
 * @param[in] pos Absolute position of the event
 * @return true the event is used
 * @return false the event is not used
 */
bool ewol::Button::OnEventInput(ewol::inputType_te type, int32_t IdInput, eventInputType_te typeEvent, etk::Vector2D<float> pos)
{
	//EWOL_DEBUG("Event on BT ...");
	if (1 == IdInput) {
		if(ewol::EVENT_INPUT_TYPE_DOWN == typeEvent) {
			GenerateEventId(ewolEventButtonDown);
		}
		if(ewol::EVENT_INPUT_TYPE_UP == typeEvent) {
			GenerateEventId(ewolEventButtonUp);
		}
		if(    ewol::EVENT_INPUT_TYPE_SINGLE == typeEvent
		    || ewol::EVENT_INPUT_TYPE_DOUBLE == typeEvent
		    || ewol::EVENT_INPUT_TYPE_TRIPLE == typeEvent) {
			GenerateEventId(ewolEventButtonPressed);
			MarkToRedraw();
			return true;
		}
	}
	return false;
}


bool ewol::Button::OnEventKb(ewol::eventKbType_te typeEvent, uniChar_t unicodeData)
{
	//EWOL_DEBUG("BT PRESSED : \"" << UTF8_data << "\" size=" << strlen(UTF8_data));
	if(    typeEvent == ewol::EVENT_KB_TYPE_DOWN
	    && unicodeData == '\r') {
		GenerateEventId(ewolEventButtonEnter);
	}
	return false;
}

