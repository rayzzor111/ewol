/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */


#include <ewol/ewol.h>
#include <ewol/widget/Container.h>
#include <ewol/widget/Manager.h>


#undef __class__
#define __class__	"Container"


ewol::widget::Container::Container() {
	addObjectType("ewol::widget::Container");
	// nothing to do ...
}

void ewol::widget::Container::init(std::shared_ptr<ewol::Widget> _subElement) {
	ewol::Widget::init();
	m_subWidget = _subElement;
}


ewol::widget::Container::~Container() {
	subWidgetRemove();
}

std::shared_ptr<ewol::Widget> ewol::widget::Container::getSubWidget() {
	return m_subWidget;
}

void ewol::widget::Container::setSubWidget(std::shared_ptr<ewol::Widget> _newWidget) {
	if (_newWidget == nullptr) {
		return;
	}
	subWidgetRemove();
	m_subWidget = _newWidget;
	if (m_subWidget != nullptr) {
		m_subWidget->setParent(shared_from_this());
	}
	markToRedraw();
	requestUpdateSize();
}

void ewol::widget::Container::subWidgetRemove() {
	if (m_subWidget != nullptr) {
		m_subWidget->removeParent();
		m_subWidget.reset();
		markToRedraw();
		requestUpdateSize();
	}
}

void ewol::widget::Container::subWidgetUnLink() {
	if (m_subWidget != nullptr) {
		m_subWidget->removeParent();
	}
	m_subWidget.reset();
}

std::shared_ptr<ewol::Object> ewol::widget::Container::getSubObjectNamed(const std::string& _objectName) {
	std::shared_ptr<ewol::Object> tmpObject = ewol::Widget::getSubObjectNamed(_objectName);
	if (nullptr!=tmpObject) {
		return tmpObject;
	}
	if (nullptr != m_subWidget) {
		return m_subWidget->getSubObjectNamed(_objectName);
	}
	return nullptr;
}

void ewol::widget::Container::systemDraw(const ewol::DrawProperty& _displayProp) {
	if (true == m_hide){
		// widget is hidden ...
		return;
	}
	ewol::Widget::systemDraw(_displayProp);
	if (nullptr!=m_subWidget) {
		ewol::DrawProperty prop = _displayProp;
		prop.limit(m_origin, m_size);
		m_subWidget->systemDraw(prop);
	}
}

void ewol::widget::Container::calculateSize(const vec2& _availlable) {
	if (nullptr!=m_subWidget) {
		vec2 origin = m_origin+m_offset;
		vec2 minSize = m_subWidget->getCalculateMinSize();
		bvec2 expand = m_subWidget->getExpand();
		if (    expand.x() == false
		    || minSize.x()>_availlable.x()) {
			if (m_gravity == ewol::gravityCenter) {
				origin -= vec2((minSize.x() - _availlable.x())/2.0f, 0);
			} else if (((int32_t)m_gravity & (int32_t)ewol::gravityRight) != 0) {
				origin -= vec2((minSize.x() - _availlable.x()), 0);
			}
		}
		if(    expand.y() == false
		    || minSize.y()>_availlable.y()) {
			if (m_gravity == ewol::gravityCenter) {
				origin -= vec2(0, (minSize.y() - _availlable.y())/2.0f);
			} else if (((int32_t)m_gravity & (int32_t)ewol::gravityTop) != 0) {
				origin -= vec2(0, (minSize.y() - _availlable.y()));
			}
		}
		m_subWidget->setOrigin(origin);
		m_subWidget->calculateSize(_availlable);
	}
	ewol::Widget::calculateSize(_availlable);
}

void ewol::widget::Container::calculateMinMaxSize() {
	// call main class
	ewol::Widget::calculateMinMaxSize();
	// call sub classes
	if (nullptr!=m_subWidget) {
		m_subWidget->calculateMinMaxSize();
		vec2 min = m_subWidget->getCalculateMinSize();
		m_minSize.setMax(min);
	}
	//EWOL_ERROR("[" << getId() << "] Result min size : " <<  m_minSize);
}

void ewol::widget::Container::onRegenerateDisplay() {
	if (nullptr!=m_subWidget) {
		m_subWidget->onRegenerateDisplay();
	}
}

std::shared_ptr<ewol::Widget> ewol::widget::Container::getWidgetAtPos(const vec2& _pos) {
	if (false == isHide()) {
		if (nullptr!=m_subWidget) {
			return m_subWidget->getWidgetAtPos(_pos);
		}
	}
	return nullptr;
};


bool ewol::widget::Container::loadXML(exml::Element* _node) {
	if (nullptr == _node) {
		return false;
	}
	// parse generic properties :
	ewol::Widget::loadXML(_node);
	// remove previous element :
	subWidgetRemove();
	
	// parse all the elements :
	for(size_t iii=0; iii< _node->size(); iii++) {
		exml::Element* pNode = _node->getElement(iii);
		if (pNode == nullptr) {
			// trash here all that is not element
			continue;
		}
		std::string widgetName = pNode->getValue();
		if (getWidgetManager().exist(widgetName) == false) {
			EWOL_ERROR("(l "<<pNode->getPos()<<") Unknown basic node=\"" << widgetName << "\" not in : [" << getWidgetManager().list() << "]" );
			continue;
		}
		if (nullptr != getSubWidget()) {
			EWOL_ERROR("(l "<<pNode->getPos()<<") " << __class__ << " Can only have one subWidget ??? node=\"" << widgetName << "\"" );
			continue;
		}
		EWOL_DEBUG("try to create subwidget : '" << widgetName << "'");
		std::shared_ptr<ewol::Widget> tmpWidget = getWidgetManager().create(widgetName);
		if (tmpWidget == nullptr) {
			EWOL_ERROR ("(l "<<pNode->getPos()<<") Can not create the widget : \"" << widgetName << "\"");
			continue;
		}
		// add widget :
		setSubWidget(tmpWidget);
		if (false == tmpWidget->loadXML(pNode)) {
			EWOL_ERROR ("(l "<<pNode->getPos()<<") can not load widget properties : \"" << widgetName << "\"");
			return false;
		}
	}
	return true;
}

void ewol::widget::Container::setOffset(const vec2& _newVal) {
	if (m_offset != _newVal) {
		ewol::Widget::setOffset(_newVal);
		// recalculate the new sise and position of sub widget ...
		calculateSize(m_size);
	}
}

void ewol::widget::Container::requestDestroyFromChild(const std::shared_ptr<Object>& _child) {
	if (m_subWidget != _child) {
		return;
	}
	if (m_subWidget == nullptr) {
		return;
	}
	m_subWidget->removeParent();
	m_subWidget.reset();
	markToRedraw();
}
