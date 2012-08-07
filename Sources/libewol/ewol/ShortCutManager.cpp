/**
 *******************************************************************************
 * @file ewol/ShortCutManager.cpp
 * @brief ewol shortCut manager (Sources)
 * @author Edouard DUPIN
 * @date 22/02/2012
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


#include <ewol/ShortCutManager.h>
#include <ewol/EObject.h>
#include <ewol/ewol.h>


class EventShortCut {
	public:
		const char *  generateEventId; // event generate ID (to be unique it was pointer on the string name)
		etk::UString  eventData;
		bool          shift;
		bool          control;
		bool          alt;
		bool          meta;
		uniChar_t     UnicodeValue;
};

static std::vector<EventShortCut *> l_inputShortCutEvent;  //!< generic short-cut event


void ewol::shortCut::Add(bool shift, bool control, bool alt, bool meta, uniChar_t unicodeValue, const char * generateEventId, etk::UString data)
{
	EventShortCut * newEvent = new EventShortCut();
	if (NULL == newEvent) {
		EWOL_ERROR("Allocation Error on the shortcut ...");
		return;
	}
	newEvent->generateEventId = generateEventId;
	newEvent->shift = shift;
	newEvent->control = control;
	newEvent->alt = alt;
	newEvent->meta = meta;
	newEvent->UnicodeValue = unicodeValue;
	newEvent->eventData = data;
	l_inputShortCutEvent.push_back(newEvent);
	return;
}


void ewol::shortCut::Add(const char * descriptiveString, const char * generateEventId, etk::UString data)
{
	if(		NULL==descriptiveString
		||	0==strlen(descriptiveString))
	{
		return;
	}
	bool shift = false;
	bool control = false;
	bool alt = false;
	bool meta = false;
	uint32_t UnicodeValue = 0;
	
	// parsing of the string :
	//"ctrl+shift+alt+meta+s"
	const char * tmp = strstr(descriptiveString, "ctrl");
	if(NULL != tmp) {
		control = true;
	}
	tmp = strstr(descriptiveString, "shift");
	if(NULL != tmp) {
		shift = true;
	}
	tmp = strstr(descriptiveString, "alt");
	if(NULL != tmp) {
		alt = true;
	}
	tmp = strstr(descriptiveString, "meta");
	if(NULL != tmp) {
		meta = true;
	}
	UnicodeValue = descriptiveString[strlen(descriptiveString) -1];
	// add with generic Adding function ...
	ewol::shortCut::Add(shift, control, alt, meta, UnicodeValue, generateEventId, data);
}


void ewol::shortCut::Init(void)
{
	if (l_inputShortCutEvent.size()>0) {
		EWOL_WARNING("Old element error in the shortCut system");
		for(int32_t iii=0; iii< l_inputShortCutEvent.size(); iii++) {
			delete(l_inputShortCutEvent[iii]);
			l_inputShortCutEvent[iii] = NULL;
		}
	}
	l_inputShortCutEvent.clear();
}

void ewol::shortCut::UnInit(void)
{
	if (l_inputShortCutEvent.size()>0) {
		for(int32_t iii=0; iii< l_inputShortCutEvent.size(); iii++) {
			delete(l_inputShortCutEvent[iii]);
			l_inputShortCutEvent[iii] = NULL;
		}
	}
	l_inputShortCutEvent.clear();
}



bool ewol::shortCut::Process(bool shift, bool control, bool alt, bool meta, uniChar_t unicodeValue, bool isDown)
{
	if (unicodeValue >= 'A' && unicodeValue <='Z') {
		unicodeValue += 'a' - 'A';
	}
	//EWOL_INFO("Try to find generic shortcut ...");
	for(int32_t iii=l_inputShortCutEvent.size()-1; iii>=0; iii--) {
		if(    l_inputShortCutEvent[iii]->shift == shift
		    && l_inputShortCutEvent[iii]->control == control
		    && l_inputShortCutEvent[iii]->alt == alt
		    && l_inputShortCutEvent[iii]->meta == meta
		    && l_inputShortCutEvent[iii]->UnicodeValue == unicodeValue)
		{
			if (isDown) {
				ewol::EObjectMessageMultiCast::AnonymousSend(l_inputShortCutEvent[iii]->generateEventId, l_inputShortCutEvent[iii]->eventData);
			} // no else
			return true;
		}
	}
	return false;
}


