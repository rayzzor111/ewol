/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <ewol/debug.h>
#include <ewol/translate.h>
#include <map>
#include <etk/os/FSNode.h>
#include <ejson/ejson.h>

class LocalInstanceTranslation {
	private:
		std::map<std::string,std::string> m_listPath;
		std::string m_major;
		std::string m_languageDefault;
		std::string m_language;
		bool m_translateLoadad;
		std::map<std::string,std::string> m_translate;
	public:
		LocalInstanceTranslation() :
		  m_major("ewol"),
		  m_languageDefault("EN"),
		  m_language(""),
		  m_translateLoadad(false) {
			// nothing to do ...
		}
	public:
		void addPath(const std::string& _lib, const std::string& _path, bool _major) {
			auto it = m_listPath.find(_lib);
			if (it == m_listPath.end()) {
				m_listPath.insert(make_pair(_lib, _path));
			} else {
				it->second = _path;
			}
			if (_major == true) {
				m_major = _lib;
				EWOL_INFO("Change major translation : '" << m_major << "'");
			}
			m_translateLoadad = false;
			m_translate.clear();
		};
		
		const std::string& getPaths(const std::string& _lib) {
			auto it = m_listPath.find(_lib);
			if (it == m_listPath.end()) {
				static const std::string g_error("");
				return g_error;
			}
			return it->second;
		};
		
		void setLanguageDefault(const std::string& _lang) {
			if (m_languageDefault == _lang) {
				return;
			}
			EWOL_INFO("Change default language translation : '" << _lang << "'");
			m_languageDefault = _lang;
			m_translateLoadad = false;
			m_translate.clear();
		};
		
		const std::string& getLanguageDefault() {
			return m_languageDefault;
		};
		
		void setLanguage(const std::string& _lang) {
			if (m_language == _lang) {
				return;
			}
			m_language = _lang;
			m_translateLoadad = false;
			m_translate.clear();
			if (_lang == "EN") {
				EWOL_INFO("Change language translation: '" << _lang << "'=English");
			} else if (_lang == "FR") {
				EWOL_INFO("Change language translation: '" << _lang << "'=French");
			} else if (_lang == "DE") {
				EWOL_INFO("Change language translation: '" << _lang << "'=German");
			} else if (_lang == "SP") {
				EWOL_INFO("Change language translation: '" << _lang << "'=Spanish");
			} else if (_lang == "JA") {
				EWOL_INFO("Change language translation: '" << _lang << "'=Japanese");
			} else if (_lang == "IT") {
				EWOL_INFO("Change language translation: '" << _lang << "'=Italian");
			} else if (_lang == "KO") {
				EWOL_INFO("Change language translation: '" << _lang << "'=Korean");
			} else if (_lang == "RU") {
				EWOL_INFO("Change language translation: '" << _lang << "'=Russian");
			} else if (_lang == "PT") {
				EWOL_INFO("Change language translation: '" << _lang << "'=Portuguese, Brazilian");
			} else if (_lang == "ZH") {
				EWOL_INFO("Change language translation: '" << _lang << "'=Chinese");
			} else {
				EWOL_INFO("Change language translation: '" << _lang << "'=Unknow");
			}
		};
		
		const std::string& getLanguage() {
			return m_language;
		};
		
		const std::string& get(const std::string& _instance) {
			loadTranslation();
			auto it = m_translate.find(_instance);
			if (it == m_translate.end()) {
				EWOL_DEBUG("Can not find tranlation : '" << _instance << "'");
				return _instance;
			}
			return it->second;
		};
	private:
		void loadTranslation() {
			if (m_translateLoadad == true) {
				return;
			}
			// start parse language for Major:
			auto itMajor = m_listPath.find(m_major);
			if (itMajor != m_listPath.end()) {
				std::string filename(itMajor->second + "/" + m_language + ".json");
				ejson::Document doc;
				doc.load(filename);
				for (auto element : doc.getKeys()) {
					std::string val = doc.getStringValue(element);
					m_translate.insert(make_pair(element, val));
				}
				filename = itMajor->second + "/" + m_languageDefault + ".json";
				doc.load(filename);
				for (auto element : doc.getKeys()) {
					std::string val = doc.getStringValue(element);
					auto itTrans = m_translate.find(element);
					if (itTrans == m_translate.end()) {
						m_translate.insert(make_pair(element, val));
					}
				}
			}
			// start parse language:
			for (auto &it : m_listPath) {
				if (it.first == m_major) {
					continue;
				}
				std::string filename(it.second + "/" + m_languageDefault + ".json");
				if (etk::FSNodeExist(filename) == false) {
					continue;
				}
				ejson::Document doc;
				doc.load(filename);
				for (auto element : doc.getKeys()) {
					std::string val = doc.getStringValue(element);
					auto itTrans = m_translate.find(element);
					if (itTrans == m_translate.end()) {
						m_translate.insert(make_pair(element, val));
					}
				}
			}
			// start parse default language:
			for (auto &it : m_listPath) {
				if (it.first == m_major) {
					continue;
				}
				std::string filename(it.second + "/" + m_languageDefault + ".json");
				if (etk::FSNodeExist(filename) == false) {
					continue;
				}
				ejson::Document doc;
				doc.load(filename);
				for (auto element : doc.getKeys()) {
					std::string val = doc.getStringValue(element);
					auto itTrans = m_translate.find(element);
					if (itTrans == m_translate.end()) {
						m_translate.insert(make_pair(element, val));
					}
				}
			}
		}
		
};

static LocalInstanceTranslation& getInstanceTranslation() {
	static LocalInstanceTranslation g_val;
	return g_val;
}

void ewol::translate::addPath(const std::string& _lib, const std::string& _path, bool _major) {
	getInstanceTranslation().addPath(_lib, _path, _major);
}

const std::string& ewol::translate::getPaths(const std::string& _lib) {
	return getInstanceTranslation().getPaths(_lib);
}

void ewol::translate::setLanguageDefault(const std::string& _lang) {
	getInstanceTranslation().setLanguageDefault(_lang);
}

const std::string& ewol::translate::getLanguageDefault() {
	return getInstanceTranslation().getLanguageDefault();
}

void ewol::translate::setLanguage(const std::string& _lang) {
	getInstanceTranslation().setLanguage(_lang);
}

const std::string& ewol::translate::getLanguage() {
	return getInstanceTranslation().getLanguage();
}

void ewol::translate::autoDetectLanguage() {
	EWOL_INFO("Auto-detect language of system");
	#if defined(__TARGET_OS__Linux)
		char *s = getenv("LANG");
		if (s == nullptr || strlen(s) < 2) {
			EWOL_INFO("Try to determine system language FAIL ...");
		} else {
			std::string lang;
			lang += s[0];
			lang += s[1];
			lang = etk::toupper(lang);
			getInstanceTranslation().setLanguage(lang);
		}
	#else
		EWOL_INFO("Can not auto-detect language ...");
	#endif
}

const std::string& ewol::translate::get(const std::string& _instance) {
	return getInstanceTranslation().get(_instance);
}


