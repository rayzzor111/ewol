/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license BSD v3 (see license file)
 */

#ifndef __EWOL_OBJECT_MANAGER_H__
#define __EWOL_OBJECT_MANAGER_H__

#include <etk/types.h>
#include <ewol/object/Object.h>
#include <ewol/object/MultiCast.h>

namespace ewol {
	namespace object {
		class Manager {
			private:
				std::vector<ewol::Object::Shared<ewol::Object>> m_eObjectList; // all widget allocated  == > all time increment ... never removed ...
				std::vector<ewol::Object::Shared<ewol::Object>> m_eObjectAutoRemoveList; // all widget allocated
			public:
				Manager();
				~Manager();
				/**
				 * @brief remove all resources (un-init) out of the destructor (due to the system implementation)
				 */
				void unInit();
				
				void add(ewol::Object* _object);
				void rm(ewol::Object* _object);
				int32_t getNumberObject();
				
				void autoRemove(ewol::Object* _object);
				void removeAllAutoRemove();
				
				ewol::Object* get(const std::string& _name);
			private:
				void informOneObjectIsRemoved(ewol::Object* _object);
			private:
				ewol::object::MultiCast m_multiCast; //!< muticast manager
			public:
				ewol::object::MultiCast& multiCast() {
					return m_multiCast;
				};
		};
	};
};

#endif

