/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <ewol/key/Special.h>

#define EWOL_FLAG_KEY_CAPS_LOCK 0x00000001
#define EWOL_FLAG_KEY_SHIFT     0x00000002
#define EWOL_FLAG_KEY_CTRL      0x00000004
#define EWOL_FLAG_KEY_META      0x00000008
#define EWOL_FLAG_KEY_ALT       0x00000010
#define EWOL_FLAG_KEY_ALTGR     0x00000020
#define EWOL_FLAG_KEY_NUM_LOCK  0x00000040
#define EWOL_FLAG_KEY_INSERT    0x00000080

// TODO : Update to support the Left and right of some key ...

ewol::key::Special::Special() :
  m_value(0) {
	
}
void ewol::key::Special::update(enum ewol::key::keyboard _move, bool _isDown) {
	switch (_move) {
		case keyboardInsert:
			setInsert(_isDown);
			break;
		case keyboardCapLock:
			setCapsLock(_isDown);
			break;
		case keyboardShiftLeft:
		case keyboardShiftRight:
			setShift(_isDown);
			break;
		case keyboardCtrlLeft:
		case keyboardCtrlRight:
			setCtrl(_isDown);
			break;
		case keyboardMetaLeft:
		case keyboardMetaRight:
			setMeta(_isDown);
			break;
		case keyboardAlt:
			setAlt(_isDown);
			break;
		case keyboardAltGr:
			setAltGr(_isDown);
			break;
		case keyboardNumLock:
			setNumLock(_isDown);
			break;
		default:
			break;
	}
}

bool ewol::key::Special::getCapsLock() const {
	if ((m_value & EWOL_FLAG_KEY_CAPS_LOCK) != 0) {
		return true;
	}
	return false;
}
void ewol::key::Special::setCapsLock(bool _value) {
	if ((m_value & EWOL_FLAG_KEY_CAPS_LOCK) != 0) {
		if (_value == false) {
			m_value -= EWOL_FLAG_KEY_CAPS_LOCK;
		}
	} else {
		if (_value == true) {
			m_value += EWOL_FLAG_KEY_CAPS_LOCK;
		}
	}
}

bool ewol::key::Special::getShift() const {
	if ((m_value & EWOL_FLAG_KEY_SHIFT) != 0) {
		return true;
	}
	return false;
}
void ewol::key::Special::setShift(bool _value) {
	if ((m_value & EWOL_FLAG_KEY_SHIFT) != 0) {
		if (_value == false) {
			m_value -= EWOL_FLAG_KEY_SHIFT;
		}
	} else {
		if (_value == true) {
			m_value += EWOL_FLAG_KEY_SHIFT;
		}
	}
}

bool ewol::key::Special::getCtrl() const {
	if ((m_value & EWOL_FLAG_KEY_CTRL) != 0) {
		return true;
	}
	return false;
}
void ewol::key::Special::setCtrl(bool _value) {
	if ((m_value & EWOL_FLAG_KEY_CTRL) != 0) {
		if (_value == false) {
			m_value -= EWOL_FLAG_KEY_CTRL;
		}
	} else {
		if (_value == true) {
			m_value += EWOL_FLAG_KEY_CTRL;
		}
	}
}

bool ewol::key::Special::getMeta() const {
	if ((m_value & EWOL_FLAG_KEY_META) != 0) {
		return true;
	}
	return false;
}
void ewol::key::Special::setMeta(bool _value) {
	if ((m_value & EWOL_FLAG_KEY_META) != 0) {
		if (_value == false) {
			m_value -= EWOL_FLAG_KEY_META;
		}
	} else {
		if (_value == true) {
			m_value += EWOL_FLAG_KEY_META;
		}
	}
}

bool ewol::key::Special::getAlt() const {
	if ((m_value & EWOL_FLAG_KEY_ALT) != 0) {
		return true;
	}
	return false;
}
void ewol::key::Special::setAlt(bool _value) {
	if ((m_value & EWOL_FLAG_KEY_ALT) != 0) {
		if (_value == false) {
			m_value -= EWOL_FLAG_KEY_ALT;
		}
	} else {
		if (_value == true) {
			m_value += EWOL_FLAG_KEY_ALT;
		}
	}
}

bool ewol::key::Special::getAltGr() const {
	if ((m_value & EWOL_FLAG_KEY_ALTGR) != 0) {
		return true;
	}
	return false;
}
void ewol::key::Special::setAltGr(bool _value) {
	if ((m_value & EWOL_FLAG_KEY_ALTGR) != 0) {
		if (_value == false) {
			m_value -= EWOL_FLAG_KEY_ALTGR;
		}
	} else {
		if (_value == true) {
			m_value += EWOL_FLAG_KEY_ALTGR;
		}
	}
}

bool ewol::key::Special::getNumLock() const {
	if ((m_value & EWOL_FLAG_KEY_NUM_LOCK) != 0) {
		return true;
	}
	return false;
}
void ewol::key::Special::setNumLock(bool _value) {
	if ((m_value & EWOL_FLAG_KEY_NUM_LOCK) != 0) {
		if (_value == false) {
			m_value -= EWOL_FLAG_KEY_NUM_LOCK;
		}
	} else {
		if (_value == true) {
			m_value += EWOL_FLAG_KEY_NUM_LOCK;
		}
	}
}

bool ewol::key::Special::getInsert() const {
	if ((m_value & EWOL_FLAG_KEY_INSERT) != 0) {
		return true;
	}
	return false;
}
void ewol::key::Special::setInsert(bool _value) {
	if ((m_value & EWOL_FLAG_KEY_INSERT) != 0) {
		if (_value == false) {
			m_value -= EWOL_FLAG_KEY_INSERT;
		}
	} else {
		if (_value == true) {
			m_value += EWOL_FLAG_KEY_INSERT;
		}
	}
}

std::ostream& ewol::key::operator <<(std::ostream& _os, const ewol::key::Special _obj) {
	_os << " capLock=" << _obj.getCapsLock();
	_os << " shift=" << _obj.getShift();
	_os << " ctrl=" << _obj.getCtrl();
	_os << " meta=" << _obj.getMeta();
	_os << " alt=" << _obj.getAlt();
	_os << " altGr=" << _obj.getAltGr();
	_os << " verNum=" << _obj.getNumLock();
	_os << " insert=" << _obj.getInsert();
	return _os;
}
