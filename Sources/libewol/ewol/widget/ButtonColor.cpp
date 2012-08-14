/**
 *******************************************************************************
 * @file ewol/widget/ButtonColor.cpp
 * @brief ewol Button select color widget system (Sources)
 * @author Edouard DUPIN
 * @date 04/03/2012
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

#include <ewol/widget/ButtonColor.h>

#include <ewol/OObject.h>
#include <ewol/WidgetManager.h>
#include <ewol/widgetMeta/ColorChooser.h>
#include <ewol/ewol.h>


extern const char * const ewolEventButtonColorChange    = "ewol-Button-Color-Change";


/**
 * @brief Initilise the basic widget property ==> due to the android system
 * @note all widget that have template might have this initializer ...
 * @param ---
 * @return ---
 */
void ewol::WIDGET_ButtonColorInit(void)
{
	
}

#undef __class__
#define __class__	"ButtonColor"


void ewol::ButtonColor::Init(void)
{
	AddEventId(ewolEventButtonColorChange);
	
	m_alignement = ewol::TEXT_ALIGN_CENTER;
	
	#ifdef __TARGET_OS__Android
		m_padding.y = 12;
		m_padding.x = 12;
	#else
		m_padding.y = 4;
		m_padding.x = 4;
	#endif
	
	
	m_textColorBg = etk::color::color_Black;
	m_textColorBg.alpha = 0x3F;
	m_widgetContextMenu = NULL;
	SetCanHaveFocus(true);
}

ewol::ButtonColor::ButtonColor(void)
{
	m_label = "No Label";
	Init();
}

ewol::ButtonColor::ButtonColor(etk::UString newLabel)
{
	m_label = newLabel;
	Init();
}


ewol::ButtonColor::~ButtonColor(void)
{
	
}


void ewol::ButtonColor::SetPadding(Vector2D<float> newPadding)
{
	m_padding = newPadding;
}

bool ewol::ButtonColor::CalculateMinSize(void)
{
	int32_t fontId = GetDefaultFontId();
	int32_t minWidth = ewol::GetWidth(fontId, m_label);
	int32_t minHeight = ewol::GetHeight(fontId);
	m_minSize.x = m_padding.x*2 + minWidth;
	m_minSize.y = m_padding.y*2 + minHeight;
	MarkToRedraw();
	return true;
}


void ewol::ButtonColor::SetLabel(etk::UString newLabel)
{
	m_label = newLabel;
	MarkToRedraw();
}

void ewol::ButtonColor::SetValue(bool val)
{
	
}

void ewol::ButtonColor::SetAlignement(textAlignement_te typeAlign)
{
	m_alignement = typeAlign;
	MarkToRedraw();
}


bool ewol::ButtonColor::GetValue(void)
{
	return false;
}


void ewol::ButtonColor::OnRegenerateDisplay(void)
{
	if (true == NeedRedraw()) {
		// clean the object list ...
		ClearOObjectList();
		
		int32_t tmpSizeX = m_minSize.x;
		int32_t tmpSizeY = m_minSize.y;
		int32_t tmpOriginX = (m_size.x - m_minSize.x) / 2;
		int32_t tmpOriginY = (m_size.y - m_minSize.y) / 2;
		// no change for the text orogin : 
		int32_t tmpTextOriginX = (m_size.x - m_minSize.x) / 2 + m_padding.x;
		int32_t tmpTextOriginY = (m_size.y - m_minSize.y) / 2 + m_padding.y;
		
		if (true==m_userFillX) {
			tmpSizeX = m_size.x;
			tmpOriginX = 0;
			if (m_alignement == ewol::TEXT_ALIGN_LEFT) {
				tmpTextOriginX = m_padding.x;
			}
		}
		if (true==m_userFillY) {
			tmpSizeY = m_size.y;
			tmpOriginY = 0;
		}
		tmpOriginX += m_padding.x;
		tmpOriginY += m_padding.y;
		tmpSizeX -= 2*m_padding.x;
		tmpSizeY -= 2*m_padding.y;
		
		if ((m_textColorBg.red>0.5) || (m_textColorBg.green>0.5) || (m_textColorBg.blue > 0.8) ) {
			m_textColorFg.red   = 0.0;
			m_textColorFg.green = 0.0;
			m_textColorFg.blue  = 0.0;
			m_textColorFg.alpha = 1.0;
		} else {
			m_textColorFg.red   = 1.0;
			m_textColorFg.green = 1.0;
			m_textColorFg.blue  = 1.0;
			m_textColorFg.alpha = 1.0;
		}
		ewol::OObject2DText * tmpText = new ewol::OObject2DText("", -1, m_textColorFg);
		/*
		int32_t fontId = GetDefaultFontId();
		int32_t fontHeight = ewol::GetHeight(fontId);
		int32_t fontWidth = ewol::GetWidth(fontId, m_label.c_str());
		*/
		Vector2D<float> textPos;
		textPos.x = tmpTextOriginX;
		textPos.y = tmpTextOriginY;
		clipping_ts drawClipping;
		drawClipping.x = m_padding.x;
		drawClipping.y = m_padding.y;
		drawClipping.w = m_size.x - 2*m_padding.x;
		drawClipping.h = m_size.y - 2*m_padding.y;
		tmpText->Text(textPos, drawClipping, m_label);
		
		ewol::OObject2DColored * tmpOObjects = new ewol::OObject2DColored;
		tmpOObjects->SetColor(m_textColorBg);
		tmpOriginX -= m_padding.x/2;
		tmpOriginY -= m_padding.y/2;
		tmpSizeX += m_padding.x/1;
		tmpSizeY += m_padding.y/1;
		tmpOObjects->Rectangle( tmpOriginX, tmpOriginY, tmpSizeX, tmpSizeY);
		AddOObject(tmpOObjects);
		
		AddOObject(tmpText);
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
bool ewol::ButtonColor::OnEventInput(ewol::inputType_te type, int32_t IdInput, eventInputType_te typeEvent, Vector2D<float> pos)
{
	//EWOL_DEBUG("Event on BT ...");
	if (1 == IdInput) {
		if(    ewol::EVENT_INPUT_TYPE_SINGLE == typeEvent
		    || ewol::EVENT_INPUT_TYPE_DOUBLE == typeEvent
		    || ewol::EVENT_INPUT_TYPE_TRIPLE == typeEvent) {
			// nothing to do ...
			//GenerateEventId(ewolEventButtonPressed);
			// Display the pop-up menu ... 
			
			// create a context menu : 
			m_widgetContextMenu = new ewol::ContextMenu();
			if (NULL == m_widgetContextMenu) {
				EWOL_ERROR("Allocation Error");
				return true;
			}
			// Get the button widget : 
			Vector2D<float> newPosition;
			newPosition.x = m_origin.x + m_size.x/2;
			newPosition.y = m_origin.y;
			
			m_widgetContextMenu->SetPositionMark(ewol::CONTEXT_MENU_MARK_BOTTOM, newPosition );
			
			ewol::ColorChooser * myColorChooser = new ewol::ColorChooser();
			myColorChooser->SetColor(m_textColorBg);
			// set it in the pop-up-system : 
			m_widgetContextMenu->SubWidgetSet(myColorChooser);
			myColorChooser->RegisterOnEvent(this, ewolEventColorChooserChange, ewolEventColorChooserChange);
			ewol::PopUpWidgetPush(m_widgetContextMenu);
			MarkToRedraw();
			
			return true;
		}
	}
	return false;
}


void ewol::ButtonColor::SetCurrentColor(color_ts color)
{
	m_selectedColor = color;
	m_textColorBg = m_selectedColor;
	char colorText[256];
	sprintf(colorText, "#%02X%02X%02X%02X",
	        (uint8_t)(color.red   * 0xFF),
	        (uint8_t)(color.green * 0xFF),
	        (uint8_t)(color.blue  * 0xFF),
	        (uint8_t)(color.alpha * 0xFF));
	//set the new label ...
	SetLabel(colorText);
}

/**
 * @brief Receive a message from an other EObject with a specific eventId and data
 * @param[in] CallerObject Pointer on the EObject that information came from
 * @param[in] eventId Message registered by this class
 * @param[in] data Data registered by this class
 * @return ---
 */
void ewol::ButtonColor::OnReceiveMessage(ewol::EObject * CallerObject, const char * eventId, etk::UString data)
{
	if (eventId == ewolEventColorChooserChange) {
		// TODO : Parse the input color ...
		//color_ts tmpColor(data);
		color_ts tmpColor;
		m_selectedColor = tmpColor;
		m_textColorBg = m_selectedColor;
		char colorText[256];
		sprintf(colorText, "#%02X%02X%02X%02X",
		        (uint8_t)(tmpColor.red   * 0xFF),
		        (uint8_t)(tmpColor.green * 0xFF),
		        (uint8_t)(tmpColor.blue  * 0xFF),
		        (uint8_t)(tmpColor.alpha * 0xFF));
		//set the new label ...
		SetLabel(colorText);
		GenerateEventId(ewolEventButtonColorChange);
	}
}
