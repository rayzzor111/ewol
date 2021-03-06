/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <ewol/ewol.h>
#include <ewol/widget/Sizer.h>
#include <ewol/widget/Manager.h>

#undef __class__
#define __class__ "Sizer"

ewol::widget::Sizer::Sizer() :
  m_mode(*this, "mode", modeHori, "The display mode"),
  m_borderSize(*this, "border", vec2(0,0), "The sizer border size"),
  m_animation(animationNone),
  m_animationTime(0) {
	addObjectType("ewol::widget::Sizer");
	m_mode.add(modeHori, "hori");
	m_mode.add(modeVert, "vert");
}

void ewol::widget::Sizer::init(enum displayMode _mode) {
	ewol::widget::ContainerN::init();
	m_mode.set(_mode);
}

ewol::widget::Sizer::~Sizer() {
	// disable annimation to remore "remove" error
	m_animation = animationNone;
	m_animationTime = 0;
	//EWOL_DEBUG("[" << getId() << "]={" << getObjectType() << "}  sizer : destroy (mode=" << (m_mode == ewol::widget::Sizer::modeVert?"Vert":"Hori") << ")");
}


void ewol::widget::Sizer::calculateSize(const vec2& _availlable) {
	ewol::Widget::calculateSize(_availlable);
	vec2 tmpBorderSize = m_borderSize->getPixel();
	EWOL_VERBOSE("[" << getId() << "] update size : " << _availlable << " nbElement : " << m_subWidget.size() << " borderSize=" << tmpBorderSize << " from border=" << m_borderSize);
	m_size -= tmpBorderSize*2;
	// calculate unExpandable size :
	float unexpandableSize=0.0;
	int32_t nbWidgetFixedSize=0;
	int32_t nbWidgetNotFixedSize=0;
	for (auto &it : m_subWidget) {
		if (it != nullptr) {
			vec2 tmpSize = it->getCalculateMinSize();
			if (m_mode == ewol::widget::Sizer::modeVert) {
				unexpandableSize += tmpSize.y();
				if (false == it->canExpand().y()) {
					nbWidgetFixedSize++;
				} else {
					nbWidgetNotFixedSize++;
				}
			} else {
				unexpandableSize += tmpSize.x();
				if (false == it->canExpand().x()) {
					nbWidgetFixedSize++;
				} else {
					nbWidgetNotFixedSize++;
				}
			}
		}
	}
	// 2 cases : 1 or more can Expand, or all is done ...
	float sizeToAddAtEveryOne = 0;
	// 2 cases : 1 or more can Expand, or all is done ...
	if (0 != nbWidgetNotFixedSize) {
		if (m_mode == ewol::widget::Sizer::modeVert) {
			sizeToAddAtEveryOne = (m_size.y() - unexpandableSize) / nbWidgetNotFixedSize;
		} else {
			sizeToAddAtEveryOne = (m_size.x() - unexpandableSize) / nbWidgetNotFixedSize;
		}
		if (sizeToAddAtEveryOne<0.0) {
			sizeToAddAtEveryOne=0;
		}
	}
	vec2 tmpOrigin = m_origin + tmpBorderSize;
	for (auto &it : m_subWidget) {
		if (it != nullptr) {
			vec2 tmpSize = it->getCalculateMinSize();
			// set the origin :
			EWOL_VERBOSE("[" << getId() << "] set ORIGIN : " << tmpOrigin << " & offset=" << m_offset);
			it->setOrigin(vec2ClipInt32(tmpOrigin+m_offset));
			// Now update his size  his size in X and the curent sizer size in Y:
			if (m_mode == ewol::widget::Sizer::modeVert) {
				if (it->canExpand().y() == true) {
					it->calculateSize(vec2ClipInt32(vec2(m_size.x(), tmpSize.y()+sizeToAddAtEveryOne)));
					tmpOrigin.setY(tmpOrigin.y() + tmpSize.y()+sizeToAddAtEveryOne);
				} else {
					it->calculateSize(vec2ClipInt32(vec2(m_size.x(), tmpSize.y())));
					tmpOrigin.setY(tmpOrigin.y() + tmpSize.y());
				}
			} else {
				if (it->canExpand().x() == true) {
					it->calculateSize(vec2ClipInt32(vec2(tmpSize.x()+sizeToAddAtEveryOne, m_size.y())));
					tmpOrigin.setX(tmpOrigin.x() + tmpSize.x()+sizeToAddAtEveryOne);
				} else {
					it->calculateSize(vec2ClipInt32(vec2(tmpSize.x(), m_size.y())));
					tmpOrigin.setX(tmpOrigin.x() + tmpSize.x());
				}
			}
		}
	}
	m_size += tmpBorderSize*2;
	markToRedraw();
}

void ewol::widget::Sizer::calculateMinMaxSize() {
	EWOL_VERBOSE("[" << getId() << "] update minimum size");
	m_subExpend.setValue(false, false);
	m_minSize = m_userMinSize->getPixel();
	vec2 tmpBorderSize = m_borderSize->getPixel();
	EWOL_VERBOSE("[" << getId() << "] {" << getObjectType() << "} set min size : " <<  m_minSize);
	m_minSize += tmpBorderSize*2;
	for (auto &it : m_subWidget) {
		if (it != nullptr) {
			it->calculateMinMaxSize();
			if (it->canExpand().x() == true) {
				m_subExpend.setX(true);
			}
			if (it->canExpand().y() == true) {
				m_subExpend.setY(true);
			}
			vec2 tmpSize = it->getCalculateMinSize();
			EWOL_VERBOSE("[" << getId() << "] NewMinSize=" << tmpSize);
			EWOL_VERBOSE("[" << getId() << "] {" << getObjectType() << "}     Get minSize="<< tmpSize);
			if (m_mode == ewol::widget::Sizer::modeVert) {
				m_minSize.setY(m_minSize.y() + tmpSize.y());
				if (tmpSize.x()>m_minSize.x()) {
					m_minSize.setX(tmpSize.x());
				}
			} else {
				m_minSize.setX(m_minSize.x() + tmpSize.x());
				if (tmpSize.y()>m_minSize.y()) {
					m_minSize.setY(tmpSize.y());
				}
			}
		}
	}
	//EWOL_ERROR("[" << getId() << "] {" << getObjectType() << "} Result min size : " <<  m_minSize);
}

int32_t ewol::widget::Sizer::subWidgetAdd(std::shared_ptr<ewol::Widget> _newWidget) {
	if (m_animation == animationNone) {
		return ewol::widget::ContainerN::subWidgetAdd(_newWidget);
	}
	// TODO : ...
	return ewol::widget::ContainerN::subWidgetAdd(_newWidget);
}

int32_t ewol::widget::Sizer::subWidgetAddStart(std::shared_ptr<ewol::Widget> _newWidget) {
	if (m_animation == animationNone) {
		return ewol::widget::ContainerN::subWidgetAddStart(_newWidget);
	}
	// TODO : ...
	return ewol::widget::ContainerN::subWidgetAddStart(_newWidget);
}

void ewol::widget::Sizer::subWidgetRemove(std::shared_ptr<ewol::Widget> _newWidget) {
	if (m_animation == animationNone) {
		ewol::widget::ContainerN::subWidgetRemove(_newWidget);
		return;
	}
	// TODO : ...
	ewol::widget::ContainerN::subWidgetRemove(_newWidget);
}

void ewol::widget::Sizer::subWidgetUnLink(std::shared_ptr<ewol::Widget> _newWidget) {
	if (m_animation == animationNone) {
		ewol::widget::ContainerN::subWidgetUnLink(_newWidget);
		return;
	}
	// TODO : ...
	ewol::widget::ContainerN::subWidgetUnLink(_newWidget);
}

void ewol::widget::Sizer::onParameterChangeValue(const ewol::object::ParameterRef& _paramPointer) {
	ewol::widget::ContainerN::onParameterChangeValue(_paramPointer);
	if (_paramPointer == m_mode) {
		markToRedraw();
		requestUpdateSize();
	} else if (_paramPointer == m_borderSize) {
		markToRedraw();
		requestUpdateSize();
	}
}