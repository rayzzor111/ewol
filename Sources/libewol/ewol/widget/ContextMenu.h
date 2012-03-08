/**
 *******************************************************************************
 * @file ewol/widget/ContextMenu.h
 * @brief ewol contextual Menu widget system (header)
 * @author Edouard DUPIN
 * @date 16/02/2012
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

#ifndef __EWOL_CONTEXT_MENU_H__
#define __EWOL_CONTEXT_MENU_H__

#include <etk/Types.h>
#include <ewol/Debug.h>
#include <ewol/Widget.h>
#include <ewol/widget/Drawable.h>

namespace ewol {
	typedef enum {
		CONTEXT_MENU_MARK_TOP,
		CONTEXT_MENU_MARK_RIGHT,
		CONTEXT_MENU_MARK_BOTTOM,
		CONTEXT_MENU_MARK_LEFT,
		CONTEXT_MENU_MARK_NONE
	}markPosition_te;
	class ContextMenu : public ewol::Drawable
	{
		public:
			ContextMenu(void);
			virtual ~ContextMenu(void);
		public:
			virtual bool   CalculateSize(etkFloat_t availlableX, etkFloat_t availlableY); // this generate the current size ...
			virtual bool   CalculateMinSize(void); //update the min Size ... and the expend parameters for the sizer
			virtual void   SetMinSise(etkFloat_t x=-1, etkFloat_t y=-1);
			virtual void   SetExpendX(bool newExpend=false);
			virtual void   SetExpendY(bool newExpend=false);
		private:
			color_ts        m_colorBackGroung;
			color_ts        m_colorBorder;
			coord2D_ts      m_padding;
			coord2D_ts      m_arrowPos;
			etkFloat_t      m_offset;
			markPosition_te m_arrawBorder;
			ewol::Widget*   m_subWidget[NB_BOUBLE_BUFFER];
		public:
			void           SubWidgetSet(ewol::Widget* newWidget);
			void           SubWidgetRemove(void);
			void           SetPositionMark(markPosition_te position, coord2D_ts arrowPos);
		protected:
			virtual bool   OnDraw(void);
		public:
			virtual void   OnRegenerateDisplay(void);
		public:
			/**
			 * @brief Get the widget at the specific windows absolute position
			 * @param[in] pos gAbsolute position of the requested widget knowledge
			 * @return NULL No widget found
			 * @return pointer on the widget found
			 */
			virtual ewol::Widget * GetWidgetAtPos(coord2D_ts pos);
			virtual bool   OnEventInput(int32_t IdInput, eventInputType_te typeEvent, eventPosition_ts pos);
			/**
			 * @brief Event generated to inform a flip-flop has occured on the current widget
			 * @param ---
			 * @return ---
			 */
			virtual void   OnFlipFlopEvent(void);
	};
};

#endif
