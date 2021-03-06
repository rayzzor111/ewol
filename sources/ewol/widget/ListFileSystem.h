/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#ifndef __EWOL_LIST_FILE_H__
#define __EWOL_LIST_FILE_H__

#include <ewol/widget/List.h>
#include <etk/os/FSNode.h>
#include <ewol/resource/ColorFile.h>
#include <ewol/object/Signal.h>

namespace ewol {
	namespace widget {
		/**
		 * @brief Generic display folder class. This widget display the content of a single folder :
		 */
		class ListFileSystem : public ewol::widget::List {
			public:
				// Event list of properties
				ewol::object::Signal<std::string> signalFileSelect; //!< @event "file-select" Generated when a file is selected.
				ewol::object::Signal<std::string> signalFileValidate; //!< @event "file-validate" Generate when the user validate (return) or double click on the element
				ewol::object::Signal<std::string> signalFolderSelect;
				ewol::object::Signal<std::string> signalFolderValidate;
			protected:
				ListFileSystem();
				void init();
			public:
				DECLARE_WIDGET_FACTORY(ListFileSystem, "ListFileSystem");
				virtual ~ListFileSystem();
			protected:
				std::shared_ptr<ewol::resource::ColorFile> m_colorProperty; //!< theme color property.
				int32_t m_colorIdText; //!< Color of the text.
				int32_t m_colorIdBackground1; //!< Color of the Background.
				int32_t m_colorIdBackground2; //!< Color of the Background 2.
				int32_t m_colorIdBackgroundSelected; //!< Color of line selected.
			protected : // Derived function from the List...
				virtual etk::Color<> getBasicBG();
				virtual uint32_t getNuberOfColomn();
				virtual bool getTitle(int32_t _colomn, std::string& _myTitle, etk::Color<>& _fg, etk::Color<>& _bg);
				virtual uint32_t getNuberOfRaw();
				virtual bool getElement(int32_t _colomn, int32_t _raw, std::string& _myTextToWrite, etk::Color<>& _fg, etk::Color<>& _bg);
				virtual bool onItemEvent(int32_t _IdInput, enum ewol::key::status _typeEvent, int32_t _colomn, int32_t _raw, float _x, float _y);
			protected:
				std::vector<etk::FSNode *> m_list; //!< List of all element in the path. (they are filtered)
				/**
				 * @brief Clean the list of element.
				 */
				void clearList();
				/**
				 * @brief Regenerate the content of the view. this is actually not automation on the system update.
				 */
				virtual void regenerateView();
			protected:
				int32_t m_selectedLine; //!< Current Line ID that is selected
			public:
				/**
				 * @brief Select a specific file in the path
				 * @param[in] _data File to selested.
				 */
				virtual void setSelect(const std::string& _data);
				/**
				 * @brief Get the current selected file/folder/... in the list
				 * @return the String of the element selected.
				 */
				std::string getSelect() const ;
			protected:
				ewol::object::Param<std::string> m_folder; //!< Current folder that display point on.
				ewol::object::Param<std::string, true> m_selectFile; //!< current selected file
			public:
				/**
				 * @brief Set a folder to display (might be a valid folder !!!)
				 * @param[in] _newFolder Path on the folder to display content.
				 */
				void setFolder(const std::string& _newFolder) {
					m_folder.set(_newFolder);
				};
				/**
				 * @brief Get the element current displaying folder path.
				 * @return Path on the folder.
				 */
				const std::string& getFolder() const {
					return m_folder;
				};
			protected:
				ewol::object::Param<bool> m_showFile; //!< Show files elements
			public:
				/**
				 * @brief Set the status of the displaying files or Not.
				 * @param[in] _state New state to apply on display the 'file'.
				 */
				void setShowFiles(bool _state) {
					m_showFile.set(_state);
				};
				/**
				 * @brief Get the status of the displaying files or Not.
				 * @return The status on displaying the 'file'.
				 */
				bool getShowFiles() const {
					return m_showFile;
				};
			protected:
				ewol::object::Param<bool> m_showFolder; //!< Display the folders elements
			public:
				/**
				 * @brief Set the status of the displaying fodlers or Not.
				 * @param[in] _state New state to apply on display the 'folder'.
				 */
				void setShowFolder(bool _state) {
					m_showFolder.set(_state);
				};
				/**
				 * @brief Get the status of the displaying fodlers or Not.
				 * @return The status on displaying the 'folder'.
				 */
				bool getShowFolder() const {
					return m_showFile;
				};
			protected:
				ewol::object::Param<bool> m_showHidden; //!< Display hidden elements
			public:
				/**
				 * @brief Set the status of the displaying hidden files or folder or Not.
				 * @param[in] _state New state to apply on display the hidden element.
				 */
				void setShowHidden(bool _state) {
					m_showHidden.set(_state);
				};
				/**
				 * @brief Get the status of the displaying hidden files or folder or Not.
				 * @return The status on displaying the hidden element.
				 */
				bool getShowHidden() const {
					return m_showFile;
				};
			protected:
				ewol::object::Param<bool> m_showTemporaryFile; //!< show the temporary files elements (XXX~, XXX.bck, XXX.pyc ...)
			public:
				/**
				 * @brief Set the status of the displaying temporary file (xxx~, xxx.bck, xxx.pyc) or Not.
				 * @param[in] _state New state to apply on display temporary files.
				 */
				void setShowTemporaryFiles(bool _state) {
					m_showTemporaryFile.set(_state);
				};
				/**
				 * @brief Get the status of the displaying temporary file (xxx~, xxx.bck, xxx.pyc) or Not.
				 * @return The status on displaying temporary files.
				 */
				bool getShowTemporaryFiles() const {
					return m_showFile;
				};
			public: // glocal derived functions
				virtual void onParameterChangeValue(const ewol::object::ParameterRef& _paramPointer);
		};
	};
};

#endif

