/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */


#include <ewol/widget/ButtonColor.h>

#include <ewol/compositing/Drawing.h>
#include <ewol/widget/Manager.h>
#include <ewol/widget/meta/ColorChooser.h>
#include <ewol/widget/Windows.h>
#include <ewol/ewol.h>

// DEFINE for the shader display system :
#define STATUS_UP        (0)
#define STATUS_HOVER     (2)
#define STATUS_PRESSED   (1)
#define STATUS_DOWN      (3)


#undef __class__
#define __class__	"ButtonColor"

static const char* const eventColorHasChange = "ewol-widget-ButtonColor-colorChange";

ewol::widget::ButtonColor::ButtonColor() :
  signalChange(*this, "change", "Button color change value"),
  m_widgetContextMenu(nullptr),
  m_textColorFg(*this, "color", etk::color::black, "Current color") {
	addObjectType("ewol::widget::ButtonColor");
	changeStatusIn(STATUS_UP);
	setCanHaveFocus(true);
	// Limit event at 1:
	setMouseLimit(1);
}

void ewol::widget::ButtonColor::init(etk::Color<> _baseColor, std::string _shaperName) {
	ewol::Widget::init();
	m_shaper.setSource(_shaperName);
	m_textColorFg.set(_baseColor);
}

ewol::widget::ButtonColor::~ButtonColor() {
	
}


void ewol::widget::ButtonColor::setShaperName(std::string _shaperName) {
	m_shaper.setSource(_shaperName);
}


void ewol::widget::ButtonColor::calculateMinMaxSize() {
	ewol::Padding padding = m_shaper.getPadding();
	std::string label = m_textColorFg.getString();
	vec3 minSize = m_text.calculateSize(label);
	m_minSize.setX(padding.x()*2 + minSize.x() + 7);
	m_minSize.setY(padding.y()*2 + minSize.y() );
	markToRedraw();
}



void ewol::widget::ButtonColor::onDraw() {
	m_shaper.draw();
	m_text.draw();
}


void ewol::widget::ButtonColor::onRegenerateDisplay() {
	if (needRedraw() == false) {
		return;
	}
	EWOL_DEBUG("redraw");
	m_text.clear();
	m_shaper.clear();
	
	ewol::Padding padding = m_shaper.getPadding();
	
	std::string label = m_textColorFg.getString();
	
	ivec2 localSize = m_minSize;
	
	vec3 tmpOrigin((m_size.x() - m_minSize.x()) / 2.0,
	               (m_size.y() - m_minSize.y()) / 2.0,
	               0);
	// no change for the text orogin : 
	vec3 tmpTextOrigin((m_size.x() - m_minSize.x()) / 2.0,
	                   (m_size.y() - m_minSize.y()) / 2.0,
	                   0);
	
	if (true == m_userFill->x()) {
		localSize.setX(m_size.x());
		tmpOrigin.setX(0);
		tmpTextOrigin.setX(0);
	}
	if (true == m_userFill->y()) {
		localSize.setY(m_size.y());
	}
	tmpOrigin += vec3(padding.xLeft(), padding.yButtom(), 0);
	tmpTextOrigin += vec3(padding.xLeft(), padding.yButtom(), 0);
	localSize -= ivec2(padding.x(), padding.y());
	
	// clean the element
	m_text.reset();
	if(    m_textColorFg.get().r() < 100
	    || m_textColorFg.get().g() < 100
	    || m_textColorFg.get().b() < 100) {
		m_text.setColor(etk::color::white);
	} else {
		m_text.setColor(etk::color::black);
	}
	m_text.setPos(tmpTextOrigin);
	m_text.setColorBg(m_textColorFg.get());
	m_text.setTextAlignement(tmpTextOrigin.x(), tmpTextOrigin.x()+localSize.x(), ewol::compositing::alignCenter);
	m_text.print(label);
	
	
	if (true == m_userFill->y()) {
		tmpOrigin.setY(padding.yButtom());
	}
	
	// selection area :
	m_selectableAreaPos = vec2(tmpOrigin.x()-padding.xLeft(), tmpOrigin.y()-padding.yButtom());
	m_selectableAreaSize = localSize + vec2(padding.x(),padding.y());
	vec3 tmpp = m_text.calculateSize(label);
	m_shaper.setShape(m_selectableAreaPos,
	                  m_selectableAreaSize,
	                  vec2(tmpTextOrigin.x(), tmpTextOrigin.y()),
	                  vec2(tmpp.x(), tmpp.y()));
}


bool ewol::widget::ButtonColor::onEventInput(const ewol::event::Input& _event) {
	bool previousHoverState = m_mouseHover;
	if(ewol::key::statusLeave == _event.getStatus()) {
		m_mouseHover = false;
		m_buttonPressed = false;
	} else {
		vec2 relativePos = relativePosition(_event.getPos());
		// prevent error from ouside the button
		if(    relativePos.x() < m_selectableAreaPos.x()
		    || relativePos.y() < m_selectableAreaPos.y()
		    || relativePos.x() > m_selectableAreaPos.x() + m_selectableAreaSize.x()
		    || relativePos.y() > m_selectableAreaPos.y() + m_selectableAreaSize.y() ) {
			m_mouseHover = false;
			m_buttonPressed = false;
		} else {
			m_mouseHover = true;
		}
	}
	bool previousPressed = m_buttonPressed;
	//EWOL_DEBUG("Event on BT ... mouse position : " << m_mouseHover);
	if (true == m_mouseHover) {
		if (1 == _event.getId()) {
			if(ewol::key::statusDown == _event.getStatus()) {
				m_buttonPressed = true;
				markToRedraw();
			}
			if(ewol::key::statusUp == _event.getStatus()) {
				m_buttonPressed = false;
				markToRedraw();
			}
			if(ewol::key::statusSingle == _event.getStatus()) {
				m_buttonPressed = false;
				m_mouseHover = false;
				// create a context menu : 
				m_widgetContextMenu = ewol::widget::ContextMenu::create();
				if (nullptr == m_widgetContextMenu) {
					EWOL_ERROR("Allocation Error");
					return true;
				}
				vec2 tmpPos = m_origin + m_selectableAreaPos + m_selectableAreaSize;
				tmpPos.setX( tmpPos.x() - m_minSize.x()/2.0);
				m_widgetContextMenu->setPositionMark(ewol::widget::ContextMenu::markButtom, tmpPos );
				
				std::shared_ptr<ewol::widget::ColorChooser> myColorChooser = widget::ColorChooser::create();
				myColorChooser->setColor(m_textColorFg.get());
				// set it in the pop-up-system : 
				m_widgetContextMenu->setSubWidget(myColorChooser);
				myColorChooser->signalChange.bind(shared_from_this(), &ewol::widget::ButtonColor::onCallbackColorChange);
				std::shared_ptr<ewol::widget::Windows> currentWindows = getWindows();
				if (currentWindows == nullptr) {
					EWOL_ERROR("Can not get the curent Windows...");
					m_widgetContextMenu.reset();
				} else {
					currentWindows->popUpWidgetPush(m_widgetContextMenu);
				}
				markToRedraw();
			}
		}
	}
	if(    m_mouseHover != previousHoverState
	    || m_buttonPressed != previousPressed) {
		if (true == m_buttonPressed) {
			changeStatusIn(STATUS_PRESSED);
		} else {
			if (true == m_mouseHover) {
				changeStatusIn(STATUS_HOVER);
			} else {
				changeStatusIn(STATUS_UP);
			}
		}
	}
	return m_mouseHover;
}

void ewol::widget::ButtonColor::onCallbackColorChange(const etk::Color<>& _color) {
	setValue(_color);
}

void ewol::widget::ButtonColor::changeStatusIn(int32_t _newStatusId) {
	if (true == m_shaper.changeStatusIn(_newStatusId) ) {
		periodicCallEnable();
		markToRedraw();
	}
}

void ewol::widget::ButtonColor::periodicCall(const ewol::event::Time& _event) {
	if (false == m_shaper.periodicCall(_event) ) {
		periodicCallDisable();
	}
	markToRedraw();
}


void ewol::widget::ButtonColor::onParameterChangeValue(const ewol::object::ParameterRef& _paramPointer) {
	ewol::Widget::onParameterChangeValue(_paramPointer);
	if (_paramPointer == m_textColorFg) {
		signalChange.emit(m_textColorFg);
		markToRedraw();
	}
}