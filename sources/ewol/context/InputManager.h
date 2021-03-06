/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#ifndef __EWOL_SYSTEM_INPUT_H__
#define __EWOL_SYSTEM_INPUT_H__

#include <ewol/widget/Widget.h>

#define MAX_MANAGE_INPUT (15)

namespace ewol {
	namespace context {
		/**
		 * @brief internal structure
		 * @not-in-doc
		 */
		class InputPoperty {
			public:
				bool isUsed;
				int32_t destinationInputId;
				int64_t lastTimeEvent;
				std::weak_ptr<ewol::Widget> curentWidgetEvent;
				vec2 origin;
				vec2 size;
				vec2 downStart;
				vec2 posEvent;
				bool isDown;
				bool isInside;
				int32_t nbClickEvent; // 0 .. 1 .. 2 .. 3
		};
		
		/**
		 * @brief internal structure
		 * @not-in-doc
		 */
		class InputLimit {
			public:
				int32_t sepatateTime;
				int32_t DpiOffset;
		};
		class Context;
		class InputManager{
			// special grab pointer mode : 
			private:
				std::weak_ptr<ewol::Widget> m_grabWidget; //!< widget that grab the curent pointer.
			private:
				int32_t m_dpi;
				InputLimit m_eventInputLimit;
				InputLimit m_eventMouseLimit;
				void calculateLimit();
				InputPoperty m_eventInputSaved[MAX_MANAGE_INPUT];
				InputPoperty m_eventMouseSaved[MAX_MANAGE_INPUT];
				void abortElement(InputPoperty* _eventTable, int32_t _idInput, enum ewol::key::type _type);
				void cleanElement(InputPoperty* _eventTable, int32_t _idInput);
				/**
				 * @brief generate the event on the destinated widget.
				 * @param[in] _type Type of the event that might be sended.
				 * @param[in] _destWidget Pointer on the requested widget that element might be sended
				 * @param[in] _IdInput Id of the event (PC : [0..9] and touch : [1..9])
				 * @param[in] _typeEvent type of the eventg generated
				 * @param[in] _pos position of the event
				 * @return true if event has been greped
				 */
				bool localEventInput(enum ewol::key::type _type,
				                     std::shared_ptr<ewol::Widget> _destWidget,
				                     int32_t _IdInput,
				                     enum ewol::key::status _typeEvent,
				                     vec2 _pos);
				/**
				 * @brief convert the system event id in the correct EWOL id depending of the system management mode
				 *        This function find the next input id unused on the specifiic widget
				 *             == > on PC, the ID does not change (GUI is not the same)
				 * @param[in] _type Type of the kay event.
				 * @param[in] _destWidget Pointer of the widget destination
				 * @param[in] _realInputId system Id
				 * @return the ewol input id
				 */
				int32_t localGetDestinationId(enum ewol::key::type _type,
				                              std::shared_ptr<ewol::Widget> _destWidget,
				                              int32_t _realInputId);
			private:
				ewol::Context& m_context;
			public:
				InputManager(ewol::Context& _context);
				~InputManager();
				void setDpi(int32_t _newDPI);
				
				// note if id<0  == > the it was finger event ...
				void motion(enum ewol::key::type _type, int _pointerID, vec2 _pos );
				void state(enum ewol::key::type _type, int _pointerID, bool _isDown, vec2 _pos);
			public:
				/**
				 * @brief a new layer on the windows is set  == > might remove all the property of the current element ...
				 */
				void newLayerSet();
				/**
				 * @brief This is to transfert the event from one widget to another one
				 * @param _source the widget where the event came from
				 * @param _destination the widget where the event mitgh be generated now
				 */
				void transfertEvent(std::shared_ptr<ewol::Widget> _source, std::shared_ptr<ewol::Widget> _destination);
				/**
				 * @brief This fonction lock the pointer properties to move in relative instead of absolute
				 * @param[in] _widget The widget that lock the pointer events
				 */
				void grabPointer(std::shared_ptr<ewol::Widget> _widget);
				/**
				 * @brief This fonction un-lock the pointer properties to move in relative instead of absolute
				 */
				void unGrabPointer();
			private:
				ewol::key::Special m_specialKey;
			public:
				void setLastKeyboardSpecial(const ewol::key::Special& _specialKey) {
					m_specialKey = _specialKey;
				}
		};
	};
};
#endif


