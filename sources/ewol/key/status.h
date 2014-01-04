/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license BSD v3 (see license file)
 */

#ifndef __EWOL_KEY_STATUS_H__
#define __EWOL_KEY_STATUS_H__

#include <etk/types.h>
#include <etk/Stream.h>

namespace ewol {
	namespace key {
		/**
		 * @brief Keybord event or joyestick event
		 */
		enum status {
			statusUnknow = 0,
			statusDown, // availlable on Keyboard too
			statusMove,
			statusSingle,
			statusDouble,
			statusTriple,
			statusQuad,
			statusQuinte,
			statusUp, // availlable on Keyboard too
			statusEnter,
			statusLeave,
			statusAbort, // Appeare when an event is tranfert betwwen widgets (the widget which receive this has lost the events)
			statusTransfert, // Appeare when an event is tranfert betwwen widgets (the widget which receive this has receive the transfert of the event)
			statusCount, // number max of imput possible
		};
		/**
		 * @brief Debug operator To display the curent element in a Human redeable information
		 */
		etk::CCout& operator <<(etk::CCout& _os, const enum ewol::key::status _obj);
	};
};



#endif

