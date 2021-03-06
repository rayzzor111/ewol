/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <etk/types.h>

#include <ewol/debug.h>
#include <ewol/context/clipBoard.h>
#include <ewol/context/Context.h>

#undef __class__
#define __class__ "ClipBoard"


/*
note: la copy dans le :
        0 : copy standard
   [1..9] : copy interne
       10 : bouton du milieux
*/
//!< Local copy of the clipboards
static std::string mesCopy[ewol::context::clipBoard::clipboardCount];

static const char* clipboardDescriptionString[ewol::context::clipBoard::clipboardCount+1] = {
	"clipboard0",
	"clipboard1",
	"clipboard2",
	"clipboard3",
	"clipboard4",
	"clipboard5",
	"clipboard6",
	"clipboard7",
	"clipboard8",
	"clipboard9",
	"clipboardStd",
	"clipboardSelection",
	"clipboardCount"
};

std::ostream& ewol::operator <<(std::ostream& _os, const enum ewol::context::clipBoard::clipboardListe _obj) {
	if (_obj >= 0 && _obj <ewol::context::clipBoard::clipboardCount) {
		_os << clipboardDescriptionString[_obj];
	} else {
		_os << "[ERROR]";
	}
	return _os;
}


void ewol::context::clipBoard::init() {
	EWOL_INFO("Initialyse ClipBoards");
	for(int32_t i=0; i<ewol::context::clipBoard::clipboardCount; i++) {
		mesCopy[i].clear();
	}
}


void ewol::context::clipBoard::unInit() {
	EWOL_INFO("Initialyse ClipBoards");
	for(int32_t i=0; i<ewol::context::clipBoard::clipboardCount; i++) {
		mesCopy[i].clear();
	}
}


void ewol::context::clipBoard::set(enum ewol::context::clipBoard::clipboardListe _clipboardID, const std::string& _data) {
	// check if ID is correct
	if(0 == _data.size()) {
		EWOL_INFO("request a copy of nothing");
		return;
	} else 
	
	if(_clipboardID >= ewol::context::clipBoard::clipboardCount) {
		EWOL_WARNING("request ClickBoard id error");
		return;
	}
	
	ewol::context::clipBoard::setSystem(_clipboardID, _data);
	
	if(    ewol::context::clipBoard::clipboardStd == _clipboardID
	    || ewol::context::clipBoard::clipboardSelection == _clipboardID) {
		ewol::getContext().clipBoardSet(_clipboardID);
	}
}


void ewol::context::clipBoard::request(enum ewol::context::clipBoard::clipboardListe _clipboardID) {
	if(_clipboardID >= ewol::context::clipBoard::clipboardCount) {
		EWOL_WARNING("request ClickBoard id error");
		return;
	}
	
	if(    ewol::context::clipBoard::clipboardStd == _clipboardID
	    || ewol::context::clipBoard::clipboardSelection == _clipboardID) {
		ewol::getContext().clipBoardGet(_clipboardID);
	} else {
		// generate an event on the main thread ...
		ewol::getContext().OS_ClipBoardArrive(_clipboardID);
	}
}


void ewol::context::clipBoard::setSystem(enum ewol::context::clipBoard::clipboardListe _clipboardID, const std::string& _data) {
	if(_clipboardID >= ewol::context::clipBoard::clipboardCount) {
		EWOL_WARNING("request ClickBoard id error");
		return;
	}
	// Copy datas ...
	mesCopy[_clipboardID] = _data;
}


const std::string& ewol::context::clipBoard::get(enum ewol::context::clipBoard::clipboardListe _clipboardID) {
	static const std::string emptyString("");
	if(_clipboardID >= ewol::context::clipBoard::clipboardCount) {
		EWOL_WARNING("request ClickBoard id error");
		return emptyString;
	}
	// Copy datas ...
	return mesCopy[_clipboardID];
}





