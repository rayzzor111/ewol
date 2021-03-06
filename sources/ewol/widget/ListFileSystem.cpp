/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <ewol/widget/ListFileSystem.h>
#include <etk/tool.h>

#include <etk/os/FSNode.h>


#undef __class__
#define __class__ "ListFileSystem"

ewol::widget::ListFileSystem::ListFileSystem() :
  signalFileSelect(*this, "file-select"),
  signalFileValidate(*this, "file-validate"),
  signalFolderSelect(*this, "folder-select"),
  signalFolderValidate(*this, "folder-validate"),
  m_selectedLine(-1),
  m_folder(*this, "path", "/", "Path to display"),
  m_selectFile(*this, "select", "", "selection af a specific file"),
  m_showFile(*this, "show-file", true, "display files"),
  m_showFolder(*this, "show-folder", true, "display folders"),
  m_showHidden(*this, "show-hidden", true, "Show the hidden element (file, folder, ...)"),
  m_showTemporaryFile(*this, "show-temporary", true, "display temporary files") {
	addObjectType("ewol::widget::ListFileSystem");
	#if defined(__TARGET_OS__Windows)
		m_folder = "c:/";
	#endif
	m_colorProperty = ewol::resource::ColorFile::create("THEME:COLOR:ListFileSystem.json");
	if (m_colorProperty != nullptr) {
		m_colorIdText = m_colorProperty->request("text");
		m_colorIdBackground1 = m_colorProperty->request("background1");
		m_colorIdBackground2 = m_colorProperty->request("background2");
		m_colorIdBackgroundSelected = m_colorProperty->request("selected");
	}
	setMouseLimit(1);
};

void ewol::widget::ListFileSystem::init() {
	ewol::widget::List::init();
};


ewol::widget::ListFileSystem::~ListFileSystem() {
	clearList();
};

void ewol::widget::ListFileSystem::clearList() {
	for (size_t iii=0; iii<m_list.size(); iii++) {
		if (nullptr != m_list[iii]) {
			delete(m_list[iii]);
			m_list[iii] = nullptr;
		}
	}
};

etk::Color<> ewol::widget::ListFileSystem::getBasicBG() {
	return m_colorProperty->get(m_colorIdBackground1);
}


void ewol::widget::ListFileSystem::regenerateView() {
	clearList();
	m_selectedLine = -1;
	m_list.clear();
	m_originScrooled.setValue(0,0);
	etk::FSNode tmpFolder(m_folder.get());
	// get the new list : 
	m_list = tmpFolder.folderGetSubList(m_showHidden, m_showFolder, m_showFile, m_showTemporaryFile);
	// request a redraw ...
	markToRedraw();
}

std::string ewol::widget::ListFileSystem::getSelect() const {
	std::string tmpVal = "";
	if (m_selectedLine >= 0) {
		if (m_list[m_selectedLine] != nullptr) {
			tmpVal = m_list[m_selectedLine]->getNameFile();
		}
	}
	return tmpVal;
}

// select the specific file
void ewol::widget::ListFileSystem::setSelect(const std::string& _data) {
	// remove selected line
	m_selectedLine = -1;
	// search the coresponding file :
	for (size_t iii=0; iii<m_list.size(); iii++) {
		if (nullptr!=m_list[iii]) {
			if (m_list[iii]->getNameFile() == _data) {
				// we find the line :
				m_selectedLine = iii;
				break;
			}
		}
	}
	markToRedraw();
}

uint32_t ewol::widget::ListFileSystem::getNuberOfColomn() {
	return 1;
}

bool ewol::widget::ListFileSystem::getTitle(int32_t _colomn, std::string &_myTitle, etk::Color<>& _fg, etk::Color<>& _bg) {
	_myTitle = "title";
	return true;
}

uint32_t ewol::widget::ListFileSystem::getNuberOfRaw() {
	int32_t offset = 0;
	if (m_showFolder == true) {
		if (m_folder.get() == "/") {
			offset = 1;
		} else {
			offset = 2;
		}
	}
	return m_list.size() + offset;
}

bool ewol::widget::ListFileSystem::getElement(int32_t _colomn, int32_t _raw, std::string& _myTextToWrite, etk::Color<>& _fg, etk::Color<>& _bg) {
	int32_t offset = 0;
	if (m_showFolder == true) {
		if (m_folder.get() == "/") {
			offset = 1;
		} else {
			offset = 2;
		}
		if (_raw == 0) {
			_myTextToWrite = ".";
		} else if (    _raw == 1
		            && m_folder.get() != "/") {
			_myTextToWrite = "..";
		}
	}
	if(    _raw-offset >= 0
	    && _raw-offset < (int32_t)m_list.size()
	    && nullptr != m_list[_raw-offset]) {
		_myTextToWrite = m_list[_raw-offset]->getNameFile();
		EWOL_VERBOSE("get filename for : '" << *m_list[_raw-offset] << ":'" << _myTextToWrite << "'");
	}
	_fg = m_colorProperty->get(m_colorIdText);
	if (_raw % 2) {
		_bg = m_colorProperty->get(m_colorIdBackground1);
	} else {
		_bg = m_colorProperty->get(m_colorIdBackground2);
	}
	if (m_selectedLine == _raw) {
		_bg = m_colorProperty->get(m_colorIdBackgroundSelected);
	}
	return true;
};


bool ewol::widget::ListFileSystem::onItemEvent(int32_t _IdInput,
                                               enum ewol::key::status _typeEvent,
                                               int32_t _colomn,
                                               int32_t _raw,
                                               float _x,
                                               float _y) {
	int32_t offset = 0;
	if (m_showFolder == true) {
		if (m_folder.get() == "/") {
			offset = 1;
		} else {
			offset = 2;
		}
	}
	if (_typeEvent == ewol::key::statusSingle) {
		EWOL_VERBOSE("Event on List : IdInput=" << _IdInput << " colomn=" << _colomn << " raw=" << _raw );
		if (1 == _IdInput) {
			int32_t previousRaw = m_selectedLine;
			if (_raw > (int32_t)m_list.size()+offset ) {
				m_selectedLine = -1;
			} else {
				m_selectedLine = _raw;
			}
			if (previousRaw != m_selectedLine) {
				if(    m_showFolder == true
				    && m_selectedLine == 0) {
					// "." folder
					signalFolderSelect.emit(".");
				} else if (    m_showFolder == true
				            && m_selectedLine == 1) {
					// ".." folder
					signalFolderSelect.emit("..");
				} else if(    m_selectedLine-offset  >= 0
				           && m_selectedLine-offset < (int32_t)m_list.size()
				           && nullptr != m_list[m_selectedLine-offset] ) {
					// generate event extern : 
					switch(m_list[m_selectedLine-offset]->getNodeType()) {
						case etk::FSN_FILE :
							signalFileSelect.emit(m_list[m_selectedLine-offset]->getNameFile());
							break;
						case etk::FSN_FOLDER :
							signalFolderSelect.emit(m_list[m_selectedLine-offset]->getNameFile());
							break;
						default:
							EWOL_ERROR("Can not generate event on an unknow type");
							break;
					}
				}
			} else {
				if(    m_showFolder == true
				    && m_selectedLine == 0) {
					// "." folder
					signalFolderValidate.emit(".");
				} else if (    m_showFolder == true
				            && m_selectedLine == 1) {
					// ".." folder
					signalFolderValidate.emit("..");
				} else if(    m_selectedLine-offset >= 0
				           && m_selectedLine-offset < (int32_t)m_list.size()
				           && nullptr != m_list[m_selectedLine-offset] ) {
					switch(m_list[m_selectedLine-offset]->getNodeType()) {
						case etk::FSN_FILE :
							signalFileValidate.emit(m_list[m_selectedLine-offset]->getNameFile());
							break;
						case etk::FSN_FOLDER :
							signalFolderValidate.emit(m_list[m_selectedLine-offset]->getNameFile());
							break;
						default:
							EWOL_ERROR("Can not generate event on an unknow type");
							break;
					}
				}
			}
			// need to regenerate the display of the list : 
			markToRedraw();
			return true;
		}
	}
	return false;
}

void ewol::widget::ListFileSystem::onParameterChangeValue(const ewol::object::ParameterRef& _paramPointer) {
	ewol::widget::List::onParameterChangeValue(_paramPointer);
	if (_paramPointer == m_folder) {
		regenerateView();
	} else if (_paramPointer == m_selectFile) {
		setSelect(m_selectFile);
	} else if (_paramPointer == m_showFile) {
		regenerateView();
	} else if (_paramPointer == m_showFolder) {
		regenerateView();
	} else if (_paramPointer == m_showHidden) {
		regenerateView();
	} else if (_paramPointer == m_showTemporaryFile) {
		regenerateView();
	}
}

