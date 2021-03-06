/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <etk/types.h>
#include <etk/types.h>

#include <ewol/debug.h>

#include <ewol/ewol.h>

#include <ewol/object/Object.h>
#include <ewol/object/Manager.h>
#include <ewol/context/Context.h>
#include <ewol/context/InputManager.h>
#include <ewol/resource/Texture.h>

#include <ewol/widget/Widget.h>
#include <ewol/widget/Windows.h>
#include <ewol/widget/Manager.h>



#define EVENT_DEBUG  EWOL_VERBOSE
//#define EVENT_DEBUG  EWOL_DEBUG

void ewol::context::InputManager::calculateLimit() {
	m_eventInputLimit.sepatateTime = 300000; // �s
	m_eventInputLimit.DpiOffset = m_dpi*100;
	m_eventMouseLimit.sepatateTime = 300000; // �s
	m_eventMouseLimit.DpiOffset = (float)m_dpi*(float)0.1;
}

void ewol::context::InputManager::setDpi(int32_t newDPI) {
	m_dpi = newDPI;
	// recalculate the DPI system ...
	calculateLimit();
}

bool ewol::context::InputManager::localEventInput(enum ewol::key::type _type,
                                                  std::shared_ptr<ewol::Widget> _destWidget,
                                                  int32_t _IdInput,
                                                  enum ewol::key::status _status,
                                                  vec2 _pos) {
	if (nullptr != _destWidget) {
		if (_type == ewol::key::typeMouse || _type == ewol::key::typeFinger) {
			// create the system Event :
			ewol::event::InputSystem tmpEventSystem(_type, _status, _IdInput, _pos, _destWidget, 0, m_specialKey); // TODO : set the real ID ...
			// generate the event :
			return _destWidget->systemEventInput(tmpEventSystem);
		} else {
			return false;
		}
	}
	return false;
}

void ewol::context::InputManager::abortElement(InputPoperty *_eventTable,
                                               int32_t _idInput,
                                               enum ewol::key::type _type) {
	if (nullptr == _eventTable) {
		return;
	}
	if (_eventTable[_idInput].isUsed == true) {
		localEventInput(_type, 
		                _eventTable[_idInput].curentWidgetEvent.lock(),
		                _eventTable[_idInput].destinationInputId,
		                ewol::key::statusAbort,
		                _eventTable[_idInput].posEvent);
	}
}

void ewol::context::InputManager::cleanElement(InputPoperty *_eventTable,
                                               int32_t _idInput) {
	if (nullptr == _eventTable) {
		return;
	}
	//EWOL_INFO("CleanElement[" << idInput << "] = @" << (int64_t)eventTable);
	_eventTable[_idInput].isUsed = false;
	_eventTable[_idInput].destinationInputId = 0;
	_eventTable[_idInput].lastTimeEvent = 0;
	_eventTable[_idInput].curentWidgetEvent.reset();
	_eventTable[_idInput].origin.setValue(0,0);
	_eventTable[_idInput].size.setValue(99999999,99999999);
	_eventTable[_idInput].downStart.setValue(0,0);
	_eventTable[_idInput].isDown = false;
	_eventTable[_idInput].isInside = false;
	_eventTable[_idInput].nbClickEvent = 0;
	_eventTable[_idInput].posEvent.setValue(0,0);
}

void ewol::context::InputManager::transfertEvent(std::shared_ptr<ewol::Widget> _source, std::shared_ptr<ewol::Widget> _destination) {
	if(    _source == nullptr
	    || _destination == nullptr) {
		// prevent errors ...
		return;
	}
	for(int32_t iii=0; iii<MAX_MANAGE_INPUT; iii++) {
		std::shared_ptr<ewol::Widget> tmpWidget = m_eventInputSaved[iii].curentWidgetEvent.lock();
		if (tmpWidget == _source) {
			// inform the widget that it does not receive the event now
			EVENT_DEBUG("GUI : Input ID=" << iii << " == >" << m_eventInputSaved[iii].destinationInputId << " [EVENT_INPUT_TYPE_ABORT] " << m_eventInputSaved[iii].posEvent);
			localEventInput(ewol::key::typeFinger, tmpWidget, m_eventInputSaved[iii].destinationInputId, ewol::key::statusAbort, m_eventInputSaved[iii].posEvent);
			// set the new widget ...
			m_eventInputSaved[iii].curentWidgetEvent = _destination;
			// inform the widget that he receive the event property now...
			EVENT_DEBUG("GUI : Input ID=" << iii << " == >" << m_eventInputSaved[iii].destinationInputId << " [EVENT_INPUT_TYPE_TRANSFERT] " << m_eventInputSaved[iii].posEvent);
			localEventInput(ewol::key::typeFinger, _destination, m_eventInputSaved[iii].destinationInputId, ewol::key::statusTransfert, m_eventInputSaved[iii].posEvent);
		}
		tmpWidget = m_eventMouseSaved[iii].curentWidgetEvent.lock();
		if (tmpWidget == _source) {
			// inform the widget that it does not receive the event now
			EVENT_DEBUG("GUI : Input ID=" << iii << " == >" << m_eventMouseSaved[iii].destinationInputId << " [EVENT_INPUT_TYPE_ABORT] " << m_eventMouseSaved[iii].posEvent);
			localEventInput(ewol::key::typeMouse, tmpWidget, m_eventMouseSaved[iii].destinationInputId, ewol::key::statusAbort, m_eventMouseSaved[iii].posEvent);
			// set the new widget ...
			m_eventMouseSaved[iii].curentWidgetEvent = _destination;
			// inform the widget that he receive the event property now...
			EVENT_DEBUG("GUI : Input ID=" << iii << " == >" << m_eventMouseSaved[iii].destinationInputId << " [EVENT_INPUT_TYPE_TRANSFERT] " << m_eventMouseSaved[iii].posEvent);
			localEventInput(ewol::key::typeMouse, _destination, m_eventMouseSaved[iii].destinationInputId, ewol::key::statusTransfert, m_eventMouseSaved[iii].posEvent);
		}
	}
}

void ewol::context::InputManager::grabPointer(std::shared_ptr<ewol::Widget> _widget) {
	if(_widget == nullptr) {
		return;
	}
	m_grabWidget = _widget;
	m_context.grabPointerEvents(true,   _widget->getOrigin()
	                                  + ivec2(_widget->getSize().x()/2.0f,
	                                          _widget->getSize().y()/2.0f) );
}

void ewol::context::InputManager::unGrabPointer() {
	m_grabWidget.reset();
	m_context.grabPointerEvents(false, vec2(0,0));
}

void ewol::context::InputManager::newLayerSet() {
	for(int32_t iii=0; iii<MAX_MANAGE_INPUT; iii++) {
		// remove the property of this input ...
		abortElement(m_eventInputSaved, iii, ewol::key::typeFinger);
		cleanElement(m_eventInputSaved, iii);
		abortElement(m_eventMouseSaved, iii, ewol::key::typeMouse);
		cleanElement(m_eventMouseSaved, iii);
	}
}

ewol::context::InputManager::InputManager(ewol::Context& _context) :
  m_grabWidget(),
  m_context(_context) {
	setDpi(200);
	EWOL_INFO("Init (start)");
	for(int32_t iii=0; iii<MAX_MANAGE_INPUT; iii++) {
		// remove the property of this input ...
		cleanElement(m_eventInputSaved, iii);
		cleanElement(m_eventMouseSaved, iii);
	}
	EWOL_INFO("Init (end)");
}

ewol::context::InputManager::~InputManager() {
	EWOL_INFO("Un-Init (start)");
	EWOL_INFO("Un-Init (end)");
}

int32_t ewol::context::InputManager::localGetDestinationId(enum ewol::key::type _type,
                                                           std::shared_ptr<ewol::Widget> _destWidget,
                                                           int32_t _realInputId) {
	if (_type == ewol::key::typeFinger) {
		int32_t lastMinimum = 0;
		for(int32_t iii=0; iii<MAX_MANAGE_INPUT; iii++) {
			if (true == m_eventInputSaved[iii].isUsed) {
				std::shared_ptr<ewol::Widget> tmpWidget = m_eventInputSaved[iii].curentWidgetEvent.lock();
				if (tmpWidget == _destWidget) {
					if (iii != _realInputId) {
						lastMinimum = std::max(lastMinimum, m_eventInputSaved[iii].destinationInputId);
					}
				}
			}
		}
		return lastMinimum+1;
	}
	return _realInputId;
}

// note if id<0  == > the it was finger event ...
void ewol::context::InputManager::motion(enum ewol::key::type _type,
                                         int _pointerID,
                                         vec2 _pos) {
	EVENT_DEBUG("motion event : " << _type << " " << _pointerID << " " << _pos);
	if (MAX_MANAGE_INPUT <= _pointerID) {
		// reject pointer  == > out of IDs...
		return;
	}
	InputPoperty *eventTable = nullptr;
	if (_type == ewol::key::typeMouse) {
		eventTable = m_eventMouseSaved;
	} else if (_type == ewol::key::typeFinger) {
		eventTable = m_eventInputSaved;
	} else {
		EWOL_ERROR("Unknown type of event");
		return;
	}
	if(    _pointerID > MAX_MANAGE_INPUT
	    || _pointerID < 0) {
		// not manage input
		return;
	}
	std::shared_ptr<ewol::widget::Windows> tmpWindows = m_context.getWindows();
	// special case for the mouse event 0 that represent the hover event of the system :
	if (_type == ewol::key::typeMouse && _pointerID == 0) {
		// this event is all time on the good widget ... and manage the enter and leave ...
		// NOTE : the "layer widget" force us to get the widget at the specific position all the time :
		std::shared_ptr<ewol::Widget> tmpWidget;
		if (m_grabWidget.lock() != nullptr) {
			// grab all events ...
			tmpWidget = m_grabWidget.lock();
		} else {
			if (nullptr != tmpWindows) {
				tmpWidget = tmpWindows->getWidgetAtPos(_pos);
			}
		}
		if(    tmpWidget != eventTable[_pointerID].curentWidgetEvent.lock()
		    || (    true == eventTable[_pointerID].isInside
		         && (     eventTable[_pointerID].origin.x() > _pos.x()
		              ||  eventTable[_pointerID].origin.y() > _pos.y()
		              || (eventTable[_pointerID].origin.x() + eventTable[_pointerID].size.x()) < _pos.x()
		              || (eventTable[_pointerID].origin.y() + eventTable[_pointerID].size.y()) < _pos.y()) ) ) {
			eventTable[_pointerID].isInside = false;
			EVENT_DEBUG("GUI : Input ID=" << _pointerID << " == >" << eventTable[_pointerID].destinationInputId << " [LEAVE] " << _pos);
			eventTable[_pointerID].posEvent = _pos;
			localEventInput(_type,
			                eventTable[_pointerID].curentWidgetEvent.lock(),
			                eventTable[_pointerID].destinationInputId,
			                ewol::key::statusLeave,
			                _pos);
		}
		if (eventTable[_pointerID].isInside == false) {
			// set the element inside ...
			eventTable[_pointerID].isInside = true;
			// get destination widget :
			eventTable[_pointerID].curentWidgetEvent = tmpWidget;
			if (tmpWidget == nullptr) {
				eventTable[_pointerID].isInside = false;
			} else {
				eventTable[_pointerID].origin = tmpWidget->getOrigin();
				eventTable[_pointerID].size = tmpWidget->getSize();
			}
			eventTable[_pointerID].destinationInputId = 0;
			EVENT_DEBUG("GUI : Input ID=" << _pointerID
			            << " == >" << eventTable[_pointerID].destinationInputId
			            << " [ENTER] " << _pos);
			eventTable[_pointerID].posEvent = _pos;
			localEventInput(_type,
			                tmpWidget,
			                eventTable[_pointerID].destinationInputId,
			                ewol::key::statusEnter,
			                _pos);
		}
		EVENT_DEBUG("GUI : Input ID=" << _pointerID
		            << " == >" << eventTable[_pointerID].destinationInputId
		            << " [MOVE]  " << _pos);
		eventTable[_pointerID].posEvent = _pos;
		localEventInput(_type,
		                tmpWidget,
		                eventTable[_pointerID].destinationInputId,
		                ewol::key::statusMove,
		                _pos);
	} else if (true == eventTable[_pointerID].isUsed) {
		if (true == eventTable[_pointerID].isInside) {
			if(     eventTable[_pointerID].origin.x() > _pos.x()
			    ||  eventTable[_pointerID].origin.y() > _pos.y()
			    || (eventTable[_pointerID].origin.x() + eventTable[_pointerID].size.x()) < _pos.x()
			    || (eventTable[_pointerID].origin.y() + eventTable[_pointerID].size.y()) < _pos.y()) {
				eventTable[_pointerID].isInside = false;
				EVENT_DEBUG("GUI : Input ID=" << _pointerID
				            << " == >" << eventTable[_pointerID].destinationInputId
				            << " [LEAVE] " << _pos);
				eventTable[_pointerID].posEvent = _pos;
				localEventInput(_type,
				                eventTable[_pointerID].curentWidgetEvent.lock(),
				                eventTable[_pointerID].destinationInputId,
				                ewol::key::statusLeave,
				                _pos);
			}
		} else {
			if(    (     eventTable[_pointerID].origin.x() <= _pos.x()
			         && (eventTable[_pointerID].origin.x() + eventTable[_pointerID].size.x()) >= _pos.x() )
			    && (     eventTable[_pointerID].origin.y() <= _pos.y()
			         && (eventTable[_pointerID].origin.y() + eventTable[_pointerID].size.y()) >= _pos.y() ) ) {
				eventTable[_pointerID].isInside = true;
				EVENT_DEBUG("GUI : Input ID=" << _pointerID
				            << " == >" << eventTable[_pointerID].destinationInputId
				            << " [ENTER] " << _pos);
				eventTable[_pointerID].posEvent = _pos;
				localEventInput(_type,
				                eventTable[_pointerID].curentWidgetEvent.lock(),
				                eventTable[_pointerID].destinationInputId,
				                ewol::key::statusEnter,
				                _pos);
			}
		}
		EVENT_DEBUG("GUI : Input ID=" << _pointerID
		            << " == >" << eventTable[_pointerID].destinationInputId
		            << " [MOVE]  " << _pos);
		eventTable[_pointerID].posEvent = _pos;
		localEventInput(_type,
		                eventTable[_pointerID].curentWidgetEvent.lock(),
		                eventTable[_pointerID].destinationInputId,
		                ewol::key::statusMove,
		                _pos);
	}
}

void ewol::context::InputManager::state(enum ewol::key::type _type,
                                        int _pointerID,
                                        bool _isDown,
                                        vec2 _pos)
{
	if (MAX_MANAGE_INPUT <= _pointerID) {
		// reject pointer  == > out of IDs...
		return;
	}
	EVENT_DEBUG("event pointerId=" << _pointerID);
	// convert position in open-GL coordonates ...
	InputPoperty *eventTable = nullptr;
	InputLimit   localLimit;
	if (_type == ewol::key::typeMouse) {
		eventTable = m_eventMouseSaved;
		localLimit = m_eventMouseLimit;
	} else if (_type == ewol::key::typeFinger) {
		eventTable = m_eventInputSaved;
		localLimit = m_eventInputLimit;
	} else {
		EWOL_ERROR("Unknown type of event");
		return;
	}
	if(    _pointerID > MAX_MANAGE_INPUT
	    || _pointerID <= 0) {
		// not manage input
		return;
	}
	// get the curent time ...
	int64_t currentTime = ewol::getTime();
	std::shared_ptr<ewol::widget::Windows> tmpWindows = m_context.getWindows();
	
	if (true == _isDown) {
		EVENT_DEBUG("GUI : Input ID=" << _pointerID
		             << " == >" << eventTable[_pointerID].destinationInputId
		             << " [DOWN] " << _pos);
		if(true == eventTable[_pointerID].isUsed) {
			// we have an event previously ... check delay between click and offset position
			if (currentTime - eventTable[_pointerID].lastTimeEvent > localLimit.sepatateTime) {
				cleanElement(eventTable, _pointerID);
			} else if(    abs(eventTable[_pointerID].downStart.x() - _pos.x()) >= localLimit.DpiOffset
			           || abs(eventTable[_pointerID].downStart.y() - _pos.y()) >= localLimit.DpiOffset ){
				cleanElement(eventTable, _pointerID);
			}
		}
		if(true == eventTable[_pointerID].isUsed) {
			// save start time
			eventTable[_pointerID].lastTimeEvent = currentTime;
			// generate DOWN Event
			EVENT_DEBUG("GUI : Input ID=" << _pointerID
			            << " == >" << eventTable[_pointerID].destinationInputId
			            << " [DOWN]   " << _pos);
			eventTable[_pointerID].posEvent = _pos;
			localEventInput(_type,
			                eventTable[_pointerID].curentWidgetEvent.lock(),
			                eventTable[_pointerID].destinationInputId,
			                ewol::key::statusDown,
			                _pos);
		} else {
			// Mark it used :
			eventTable[_pointerID].isUsed = true;
			// Save current position :
			eventTable[_pointerID].downStart = _pos;
			// save start time
			eventTable[_pointerID].lastTimeEvent = currentTime;
			// set the element inside ...
			eventTable[_pointerID].isInside = true;
			std::shared_ptr<ewol::Widget> tmpWidget = m_grabWidget.lock();
			// get destination widget :
			if(nullptr != tmpWindows) {
				if (    tmpWidget != nullptr
				     && _type == ewol::key::typeMouse) {
					eventTable[_pointerID].curentWidgetEvent = tmpWidget;
				} else {
					eventTable[_pointerID].curentWidgetEvent = tmpWindows->getWidgetAtPos(_pos);
				}
			} else {
				eventTable[_pointerID].curentWidgetEvent.reset();
			}
			tmpWidget = eventTable[_pointerID].curentWidgetEvent.lock();
			if (tmpWidget != nullptr) {
				eventTable[_pointerID].origin = tmpWidget->getOrigin();
				eventTable[_pointerID].size = tmpWidget->getSize();
				eventTable[_pointerID].destinationInputId = localGetDestinationId(_type, tmpWidget, _pointerID);
			} else {
				eventTable[_pointerID].destinationInputId = -1;
			}
			// generate DOWN Event
			EVENT_DEBUG("GUI : Input ID=" << _pointerID
			            << " == >" << eventTable[_pointerID].destinationInputId
			            << " [DOWN]   " << _pos);
			eventTable[_pointerID].posEvent = _pos;
			localEventInput(_type,
			                tmpWidget,
			                eventTable[_pointerID].destinationInputId,
			                ewol::key::statusDown,
			                _pos);
		}
	} else {
		EVENT_DEBUG("GUI : Input ID=" << _pointerID
		             << " == >" << eventTable[_pointerID].destinationInputId
		             << " [UP]     " << _pos);
		if(false == eventTable[_pointerID].isUsed) {
			// bad case ... ???
			EWOL_DEBUG("Up event without previous down ... ");
			// Mark it un-used :
			eventTable[_pointerID].isUsed = false;
			// revove the widget ...
			eventTable[_pointerID].curentWidgetEvent.reset();
		} else {
			std::shared_ptr<ewol::Widget> tmpWidget = eventTable[_pointerID].curentWidgetEvent.lock();
			// generate UP Event
			EVENT_DEBUG("GUI : Input ID=" << _pointerID
			            << " == >" << eventTable[_pointerID].destinationInputId
			            << " [UP]     " << _pos);
			eventTable[_pointerID].posEvent = _pos;
			// send up event after the single event to prevent multiple widget getting elements
			localEventInput(_type,
			                tmpWidget,
			                _pointerID,
			                ewol::key::statusUp,
			                _pos);
			// generate event (single)
			if(    abs(eventTable[_pointerID].downStart.x() - _pos.x()) < localLimit.DpiOffset
			    && abs(eventTable[_pointerID].downStart.y() - _pos.y()) < localLimit.DpiOffset ){
				// Save current position :
				eventTable[_pointerID].downStart = _pos;
				// save start time
				eventTable[_pointerID].lastTimeEvent = currentTime;
				int32_t nbClickMax = 0;
				if(tmpWidget != nullptr) {
					nbClickMax = tmpWidget->getMouseLimit();
					if (nbClickMax>5) {
						nbClickMax = 5;
					}
				}
				// in grab mode the single to quinte event are not generated ....
				if(    (    m_grabWidget.lock() == nullptr
				         || _type != ewol::key::typeMouse )
				    && eventTable[_pointerID].nbClickEvent < nbClickMax) {
					// generate event SINGLE :
					eventTable[_pointerID].nbClickEvent++;
					EVENT_DEBUG("GUI : Input ID=" << _pointerID
					            << " == >" << eventTable[_pointerID].destinationInputId
					            << " [" << eventTable[_pointerID].nbClickEvent << "] " << _pos);
					eventTable[_pointerID].posEvent = _pos;
					localEventInput(_type,
					                tmpWidget,
					                eventTable[_pointerID].destinationInputId,
					                (enum ewol::key::status)(ewol::key::statusSingle + eventTable[_pointerID].nbClickEvent-1),
					                _pos);
					if( eventTable[_pointerID].nbClickEvent >= nbClickMax) {
						eventTable[_pointerID].nbClickEvent = 0;
					}
				} else {
					eventTable[_pointerID].nbClickEvent = 0;
				}
			}
			// send up event after the single event to prevent multiple widget getting elements
			localEventInput(_type,
			                tmpWidget,
			                _pointerID,
			                ewol::key::statusUpAfter,
			                _pos);
			// specific for tuch event
			if (_type == ewol::key::typeFinger) {
				cleanElement(eventTable, _pointerID);
			}
		}
	}
}


