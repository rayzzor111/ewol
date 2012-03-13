/**
 *******************************************************************************
 * @file ewol/widgetMeta/FileChooser.h
 * @brief ewol File chooser meta widget system (header)
 * @author Edouard DUPIN
 * @date 29/12/2011
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

#ifndef __EWOL_FILE_CHOOSER_H__
#define __EWOL_FILE_CHOOSER_H__

#include <etk/Types.h>
#include <ewol/Debug.h>
#include <ewol/widget/PopUp.h>
#include <ewol/widget/Button.h>
#include <ewol/widget/Entry.h>
#include <ewol/widget/Label.h>
#include <ewol/widget/CheckBox.h>

extern const char * const ewolEventFileChooserCancel;
extern const char * const ewolEventFileChooserValidate;

namespace ewol {
	class FileChooser : public ewol::PopUp
	{
		public:
			FileChooser(void);
			~FileChooser(void);
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
			void SetTitle(etk::UString label);
			void SetValidateLabel(etk::UString label);
			void SetCancelLabel(etk::UString label);
			void SetFolder(etk::UString folder);
			void SetFileName(etk::UString filename);
			etk::UString GetCompleateFileName(void);
			void UpdateCurrentFolder(void);
		private:
			ewol::Label*     m_widgetTitle;
			ewol::Button*    m_widgetValidate;
			ewol::Button*    m_widgetCancel;
			ewol::Entry*     m_widgetCurrentFolder;
			ewol::Entry*     m_widgetCurrentFileName;
			ewol::Widget*    m_widgetListFolder;
			ewol::Widget*    m_widgetListFile;
			ewol::CheckBox*  m_widgetCheckBox;
			bool             m_hasSelectedFile;
			etk::UString     m_folder;
			etk::UString     m_file;
	};
	
	extern const char * const TYPE_EOBJECT_WIDGET_FILE_CHOOSER;
	
};

#define EWOL_CAST_WIDGET_FILE_CHOOSER(curentPointer) EWOL_CAST(ewol::TYPE_EOBJECT_WIDGET_FILE_CHOOSER,ewol::FileChooser,curentPointer)

#endif
