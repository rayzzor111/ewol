/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <ewol/ewol.h>
#include <ewol/widget/Manager.h>
#include <ewol/widget/ContextMenu.h>
#include <ewol/compositing/Drawing.h>
#include <ewol/widget/Manager.h>
#include <ewol/Padding.h>

#undef __class__
#define __class__	"ContextMenu"


ewol::widget::ContextMenu::ContextMenu():
  m_shaper(*this, "shaper", "the display name for config file"),
  m_arrowPos(*this, "arrow-position", vec2(0,0), "Position of the arrow in the pop-up"),
  m_arrawBorder(*this, "arrow-mode", markTop, "position of the arrow") {
	addObjectType("ewol::widget::ContextMenu");
	m_arrawBorder.add(markTop, "top");
	m_arrawBorder.add(markRight, "right");
	m_arrawBorder.add(markButtom, "buttom");
	m_arrawBorder.add(markLeft, "left");
	m_arrawBorder.add(markNone, "none");
	
	m_userExpand.set(bvec2(false,false));
	
	m_offset = 20;
	
	m_colorBackGroung = etk::color::white;
	
	m_colorBorder = etk::color::black;
	m_colorBorder.setA(0x7F);
	
	setMouseLimit(1);
}

void ewol::widget::ContextMenu::init(const std::string& _shaperName) {
	ewol::widget::Container::init();
	m_shaper.set(_shaperName);
}

ewol::widget::ContextMenu::~ContextMenu() {
	
}

void ewol::widget::ContextMenu::calculateSize(const vec2& _availlable) {
	//EWOL_DEBUG("CalculateSize=" << availlable);
	// pop-up fill all the display :
	m_size = _availlable;
	ewol::Padding padding = m_shaper->getPadding();
	EWOL_VERBOSE("our origin=" << m_origin << " size=" << m_size);
	if (nullptr != m_subWidget) {
		vec2 subWidgetSize;
		vec2 subWidgetOrigin;
		subWidgetSize = m_subWidget->getCalculateMinSize();
		if (true == m_subWidget->canExpand().x()) {
			subWidgetSize.setX(m_size.x());
		}
		if (true == m_subWidget->canExpand().y()) {
			subWidgetSize.setY(m_size.y());
		}
		int32_t minWidth = 100;
		int32_t maxWidth = 300;
		subWidgetSize.setX((int32_t)std::max(minWidth, (int32_t)subWidgetSize.x()));
		subWidgetSize.setX((int32_t)std::min(maxWidth, (int32_t)subWidgetSize.x()));
		subWidgetSize.setY((int32_t)subWidgetSize.y());
		
		// set config to the Sub-widget
		switch (m_arrawBorder) {
			case markTop:
				subWidgetOrigin.setX((int32_t)(m_arrowPos->x() - subWidgetSize.x()/2));
				subWidgetOrigin.setY((int32_t)(m_arrowPos->y() - m_offset - subWidgetSize.y()));
				break;
			case markButtom:
				subWidgetOrigin.setX((int32_t)(m_arrowPos->x() - subWidgetSize.x()/2));
				subWidgetOrigin.setY((int32_t)(m_arrowPos->y() + m_offset));
				break;
			case markRight:
			case markLeft:
			default:
				subWidgetOrigin.setX((int32_t)(m_size.x() - m_origin.x() - subWidgetSize.x())/2 + m_origin.x());
				subWidgetOrigin.setY((int32_t)(m_size.y() - m_origin.y() - subWidgetSize.y())/2 + m_origin.y());
				break;
		}
		// set the widget position at the border of the screen
		subWidgetOrigin.setX( (int32_t)(   std::max(0, (int32_t)(subWidgetOrigin.x()-padding.x()))
		                                 + padding.x()) );
		subWidgetOrigin.setY( (int32_t)(   std::max(0, (int32_t)(subWidgetOrigin.y()-padding.y()))
		                                 + padding.y()) );
		switch (m_arrawBorder) {
			default:
			case markTop:
			case markButtom:
				if (m_arrowPos->x() <= m_offset ) {
					subWidgetOrigin.setX(m_arrowPos->x()+padding.xLeft());
				}
				break;
			case markRight:
			case markLeft:
				if (m_arrowPos->y() <= m_offset ) {
					subWidgetOrigin.setY(m_arrowPos->y()+padding.yButtom());
				}
				break;
		}
		EWOL_VERBOSE("       == > sub origin=" << subWidgetOrigin << " size=" << subWidgetSize);
		m_subWidget->setOrigin(subWidgetOrigin);
		m_subWidget->calculateSize(subWidgetSize);
	}
	markToRedraw();
}


void ewol::widget::ContextMenu::calculateMinMaxSize() {
	// call main class to calculate the min size...
	ewol::widget::Container::calculateMinMaxSize();
	// add padding of the display
	ewol::Padding padding = m_shaper->getPadding();
	m_minSize += vec2(padding.x(), padding.y());
	//EWOL_DEBUG("CalculateMinSize=>>" << m_minSize);
	markToRedraw();
}


void ewol::widget::ContextMenu::onDraw() {
	m_compositing.draw();
	m_shaper->draw();
}


void ewol::widget::ContextMenu::onRegenerateDisplay() {
	// call upper class :
	ewol::widget::Container::onRegenerateDisplay();
	if (needRedraw() == false) {
		return;
	}
	m_compositing.clear();
	m_shaper->clear();
	ewol::Padding padding = m_shaper->getPadding();
	
	if (m_subWidget == nullptr) {
		return;
	}
	vec2 tmpSize = m_subWidget->getSize();
	vec2 tmpOrigin = m_subWidget->getOrigin();
	
	// display border ...
	m_compositing.setColor(m_colorBorder);
	switch (m_arrawBorder) {
		case markTop:
			m_compositing.setPos(vec3(m_arrowPos->x(), m_arrowPos->y(), 0.0f) );
			m_compositing.addVertex();
			if (m_arrowPos->x() <= tmpOrigin.x() ) {
				float laking = m_offset - padding.yTop();
				m_compositing.setPos(vec3(m_arrowPos->x()+laking, m_arrowPos->y()-laking, 0.0f) );
				m_compositing.addVertex();
				m_compositing.setPos(vec3(m_arrowPos->x(),        m_arrowPos->y()-laking, 0.0f) );
				m_compositing.addVertex();
			} else {
				float laking = m_offset - padding.yTop();
				m_compositing.setPos(vec3(m_arrowPos->x()+laking, m_arrowPos->y()-laking, 0.0f) );
				m_compositing.addVertex();
				m_compositing.setPos(vec3(m_arrowPos->x()-laking, m_arrowPos->y()-laking, 0.0f) );
				m_compositing.addVertex();
			}
			break;
		case markButtom:
			m_compositing.setPos(vec3(m_arrowPos->x(), m_arrowPos->y(), 0) );
			m_compositing.addVertex();
			if (m_arrowPos->x() <= tmpOrigin.x() ) {
				int32_t laking = m_offset - padding.yTop();
				m_compositing.setPos(vec3(m_arrowPos->x()+laking, m_arrowPos->y()+laking, 0.0f) );
				m_compositing.addVertex();
				m_compositing.setPos(vec3(m_arrowPos->x(),        m_arrowPos->y()+laking, 0.0f) );
				m_compositing.addVertex();
			} else {
				int32_t laking = m_offset - padding.yTop();
				m_compositing.setPos(vec3(m_arrowPos->x()+laking, m_arrowPos->y()+laking, 0.0f) );
				m_compositing.addVertex();
				m_compositing.setPos(vec3(m_arrowPos->x()-laking, m_arrowPos->y()+laking, 0.0f) );
				m_compositing.addVertex();
			}
			break;
		default:
		case markRight:
		case markLeft:
			EWOL_TODO("later");
			break;
	}
	
	vec2 shaperOrigin = tmpOrigin-vec2(padding.xLeft(), padding.yButtom());
	vec2 shaperSize = tmpSize+vec2(padding.x(), padding.y());
	m_shaper->setShape(vec2ClipInt32(shaperOrigin),
	                   vec2ClipInt32(shaperSize));
}

bool ewol::widget::ContextMenu::onEventInput(const ewol::event::Input& _event) {
	if (_event.getId() > 0) {
		if (nullptr != ewol::widget::Container::getWidgetAtPos(_event.getPos())) {
			return false;
		}
		if(    _event.getStatus() == ewol::key::statusDown
		    || _event.getStatus() == ewol::key::statusMove
		    || _event.getStatus() == ewol::key::statusSingle
		    || _event.getStatus() == ewol::key::statusUp
		    || _event.getStatus() == ewol::key::statusEnter
		    || _event.getStatus() == ewol::key::statusLeave ) {
			// Auto-remove ...
			autoDestroy();
			return true;
		}
	}
	return false;
}

std::shared_ptr<ewol::Widget> ewol::widget::ContextMenu::getWidgetAtPos(const vec2& _pos) {
	std::shared_ptr<ewol::Widget> val = ewol::widget::Container::getWidgetAtPos(_pos);
	if (nullptr != val) {
		return val;
	}
	return std::dynamic_pointer_cast<ewol::Widget>(shared_from_this());
}

void ewol::widget::ContextMenu::onParameterChangeValue(const ewol::object::ParameterRef& _paramPointer) {
	ewol::widget::Container::onParameterChangeValue(_paramPointer);
	if (_paramPointer == m_shaper) {
		markToRedraw();
	} else if (_paramPointer == m_arrowPos) {
		markToRedraw();
	} else if (_paramPointer == m_arrawBorder) {
		markToRedraw();
	}
}
