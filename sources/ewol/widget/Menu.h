/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license BSD v3 (see license file)
 */

#ifndef __EWOL_MENU_H__
#define __EWOL_MENU_H__

#include <etk/types.h>
#include <etk/UString.h>
#include <ewol/debug.h>
#include <ewol/widget/Widget.h>
#include <ewol/widget/SizerHori.h>
#include <ewol/widget/ContextMenu.h>

namespace ewol {
	
	class MenuElement {
		public : 
			MenuElement(void) : m_widgetPointer(NULL) { };
			int32_t        m_localId;
			int32_t        m_parentId;
			ewol::EObject* m_widgetPointer;
			etk::UString   m_label;
			etk::UString   m_image;
			const char *   m_generateEvent;
			etk::UString   m_message;
	};
	class Menu :public ewol::SizerHori
	{
		public:
			Menu(void);
			virtual ~Menu(void);
			
			/**
			 * @brief Get the current Object type of the EObject
			 * @note In Embended platforme, it is many time no -rtti flag, then it is not possible to use dynamic cast ==> this will replace it
			 * @param[in] objectType type description
			 * @return true if the object is compatible, otherwise false
			 */
			virtual const char * const GetObjectType(void) { return "EwolMenu"; };
		private:
			virtual void   SubWidgetRemoveAll(void);
			virtual void   SubWidgetAdd(ewol::Widget* newWidget);
			virtual void   SubWidgetRemove(ewol::Widget* newWidget);
			virtual void   SubWidgetUnLink(ewol::Widget* newWidget);
		private:
			etk::Vector<MenuElement*>  m_listElement;
			int32_t                        m_staticId;      // unique ID for every element of the menu ...
			ewol::ContextMenu*             m_widgetContextMenu;
		public:
			void           Clear(void);
			int32_t        AddTitle(etk::UString label, etk::UString image="", const char * generateEvent = NULL, const etk::UString message = "");
			int32_t        Add(int32_t parent, etk::UString label, etk::UString image="", const char * generateEvent = NULL, const etk::UString message = "");
			void           AddSpacer(void);
			
			/**
			 * @brief Receive a message from an other EObject with a specific eventId and data
			 * @param[in] CallerObject Pointer on the EObject that information came from
			 * @param[in] eventId Message registered by this class
			 * @param[in] data Data registered by this class
			 * @return ---
			 */
			virtual void OnReceiveMessage(ewol::EObject * CallerObject, const char * eventId, etk::UString data);
			/**
			 * @brief Inform object that an other object is removed ...
			 * @param[in] removeObject Pointer on the EObject remeved ==> the user must remove all reference on this EObject
			 * @note : Sub classes must call this class
			 * @return ---
			 */
			virtual void OnObjectRemove(ewol::EObject * removeObject);
	};
	
};

#endif
