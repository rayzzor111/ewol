/**
 *******************************************************************************
 * @file ewol/ewol.h
 * @brief Main include of ewol (header)
 * @author Edouard DUPIN
 * @date 17/10/2011
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


#ifndef __EWOL_H__
#define __EWOL_H__

#include <etk/Types.h>
#include <etk/String.h>
#include <ewol/Widget.h>
#include <ewol/Windows.h>

namespace ewol {
	void Init(int32_t argc, char *argv[]);
	void Run(void);
	void Stop(void);
	void UnInit(void);
	void DisplayWindows(ewol::Windows * windows);
	void ChangeSize(int32_t w, int32_t h);
	void ChangePos(int32_t x, int32_t y);
	void GetAbsPos(int32_t & x, int32_t & y);
	void StartResizeSystem(void);
	void StartMoveSystem(void);
	bool IsPressedInput(int32_t inputID);
};



#endif