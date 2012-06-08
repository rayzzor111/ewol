/**
 *******************************************************************************
 * @file ewol/widget/ColorBar.h
 * @brief ewol ColorBar widget system (header)
 * @author Edouard DUPIN
 * @date 05/03/2012
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

#ifndef __EWOL_COLORBAR_H__
#define __EWOL_COLORBAR_H__

#include <etk/Types.h>
#include <ewol/Debug.h>
#include <ewol/widget/Drawable.h>

extern const char * const ewolEventColorBarChange;

namespace ewol {
	class ColorBar :public ewol::Drawable
	{
		public:
			ColorBar(void);
			virtual ~ColorBar(void);
			/**
			 * @brief Check if the object has the specific type.
			 * @note In Embended platforme, it is many time no -rtti flag, then it is not possible to use dynamic cast ==> this will replace it
			 * @param[in] objectType type of the object we want to check
			 * @return true if the object is compatible, otherwise false
			 */
			virtual bool CheckObjectType(const char * const objectType);
			
			/**
			 * @brief Get the current Object type of the EObject
			 * @note In Embended platforme, it is many time no -rtti flag, then it is not possible to use dynamic cast ==> this will replace it
			 * @param[in] objectType type description
			 * @return true if the object is compatible, otherwise false
			 */
			virtual const char * const GetObjectType(void);
			virtual bool   CalculateMinSize(void);
			color_ts GetCurrentColor(void);
			void     SetCurrentColor(color_ts newOne);
		private:
			color_ts   m_currentColor;
			Vector2D<float>  m_currentUserPos;
			Vector2D<float>  m_padding;
		public:
			virtual void   OnRegenerateDisplay(void);
		public:
			/**
			 * @brief Event on an input of this Widget
			 * @param[in] type Type of the input (ewol::INPUT_TYPE_MOUSE/ewol::INPUT_TYPE_FINGER ...)
			 * @param[in] IdInput Id of the current Input (PC : left=1, right=2, middle=3, none=0 / Tactil : first finger=1 , second=2 (only on this widget, no knowledge at ouside finger))
			 * @param[in] typeEvent ewol type of event like EVENT_INPUT_TYPE_DOWN/EVENT_INPUT_TYPE_MOVE/EVENT_INPUT_TYPE_UP/EVENT_INPUT_TYPE_SINGLE/EVENT_INPUT_TYPE_DOUBLE/...
			 * @param[in] pos Absolute position of the event
			 * @return true the event is used
			 * @return false the event is not used
			 */
			virtual bool OnEventInput(ewol::inputType_te type, int32_t IdInput, eventInputType_te typeEvent, Vector2D<float>  pos);
	};
	
	extern const char * const TYPE_EOBJECT_WIDGET_COLOR_BAR;
	
};

#define EWOL_CAST_WIDGET_COLOR_BAR(curentPointer) EWOL_CAST(ewol::TYPE_EOBJECT_WIDGET_COLOR_BAR,ewol::ColorBar,curentPointer)

#endif
