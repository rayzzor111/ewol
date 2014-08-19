/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <ewol/object/Manager.h>
#include <ewol/context/Context.h>
#include <ewol/ewol.h>
#include <etk/stdTools.h>
#include <unistd.h>

#undef __class__
#define __class__ "ewol::object::Manager"

ewol::object::Manager::Manager(ewol::Context& _context) :
  m_context(_context) {
	EWOL_DEBUG(" == > init Object-Manager");
}

ewol::object::Manager::~Manager() {
	bool hasError = false;
	if (m_eObjectList.size()!=0) {
		EWOL_ERROR("Must not have anymore eObject !!!");
		hasError = true;
	}
	if (true == hasError) {
		EWOL_ERROR("Check if the function UnInit has been called !!!");
	}
	displayListObject();
}

void ewol::object::Manager::displayListObject() {
	EWOL_INFO("List loaded object : ");
	for (auto &it : m_eObjectList) {
		std::shared_ptr<ewol::Object> element = it.lock();
		if (element != nullptr) {
			EWOL_INFO("  [" << element->getId() << "] ref=" << element.use_count()-1 << " name='" << element->getName() << "' type=" << element->getObjectType());
		}
	}
}

void ewol::object::Manager::unInit() {
	EWOL_DEBUG(" == > Un-Init Object-Manager");
	for (auto &it : m_eObjectList) {
		std::shared_ptr<ewol::Object> element = it.lock();
		if (element != nullptr) {
			//it->removeObject();
		}
	}
	if (m_eObjectList.size() != 0) {
		EWOL_ERROR("Have " << m_eObjectList.size() << " active Object");
	}
	m_multiCast.clear();
	m_eObjectList.clear();
}

void ewol::object::Manager::add(const std::shared_ptr<ewol::Object>& _object) {
	if (_object == nullptr) {
		EWOL_ERROR("try to add an inexistant Object in manager");
	}
	m_eObjectList.push_back(_object);
}

int32_t ewol::object::Manager::getNumberObject() {
	return m_eObjectList.size();
}

// clean all Object that request an autoRemove ...
void ewol::object::Manager::cleanInternalRemoved() {
	size_t nbObject = m_eObjectList.size();
	EWOL_VERBOSE("Clean Object List (if needed) : " << m_eObjectList.size() << " elements");
	auto it(m_eObjectList.begin());
	while (it != m_eObjectList.end()) {
		if (it->expired() == true) {
			m_eObjectList.erase(it);
			it = m_eObjectList.begin();
		} else {
			++it;
		}
	}
	if (m_eObjectList.size() != nbObject) {
		EWOL_VERBOSE(" remove " << nbObject - m_eObjectList.size() << " deprecated objects");
	}
}

std::shared_ptr<ewol::Object> ewol::object::Manager::get(const std::string& _name) {
	if (_name == "") {
		return nullptr;
	}
	for (auto &it : m_eObjectList) {
		std::shared_ptr<ewol::Object> element = it.lock();
		if (    element != nullptr
		     && element->getName() == _name) {
			return element;
		}
	}
	return nullptr;
}


void ewol::object::Manager::add(ewol::object::RemoveEvent* _class) {
	m_removeEventList.push_back(_class);
}

void ewol::object::Manager::rm(ewol::object::RemoveEvent* _class) {
	for (size_t iii=0; iii<m_removeEventList.size(); ++iii) {
		if (m_removeEventList[iii] == _class) {
			m_removeEventList.erase(m_removeEventList.begin() + iii);
			return;
		}
	}
}

std::shared_ptr<ewol::Object> ewol::object::Manager::getObjectNamed(const std::string& _name) {
	return ewol::object::Manager::get(_name);
}

