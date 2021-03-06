/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#ifndef __EWOL_WIDGET_CONTAINER_H__
#define __EWOL_WIDGET_CONTAINER_H__

#include <etk/types.h>
#include <ewol/debug.h>
#include <ewol/widget/Widget.h>

namespace ewol {
	namespace widget {
		/**
		 * @ingroup ewolWidgetGroup
		 * @brief the Cotainer widget is a widget that have an only one subWidget
		 */
		class Container : public ewol::Widget {
			protected:
				std::shared_ptr<ewol::Widget> m_subWidget;
			protected:
				/**
				 * @brief Constructor
				 */
				Container();
				void init(std::shared_ptr<ewol::Widget> _subElement=nullptr);
			public:
				/**
				 * @brief Destructor
				 */
				virtual ~Container();
			public:
				/**
				 * @brief get the main node widget
				 * @return the requested pointer on the node
				 */
				std::shared_ptr<ewol::Widget> getSubWidget();
				/**
				 * @brief set the subWidget node widget.
				 * @param[in] _newWidget The widget to add.
				 */
				void setSubWidget(std::shared_ptr<ewol::Widget> _newWidget);
				/**
				 * @brief remove the subWidget node (async).
				 */
				void subWidgetRemove();
				/**
				 * @brief Unlink the subwidget Node.
				 */
				void subWidgetUnLink();
			public: // Derived function
				virtual void systemDraw(const ewol::DrawProperty& _displayProp);
				virtual void onRegenerateDisplay();
				virtual void calculateSize(const vec2& _availlable);
				virtual void calculateMinMaxSize();
				virtual std::shared_ptr<ewol::Widget> getWidgetAtPos(const vec2& _pos);
				virtual std::shared_ptr<ewol::Object> getSubObjectNamed(const std::string& _objectName);
				virtual bool loadXML(exml::Element* _node);
				virtual void setOffset(const vec2& _newVal);
				virtual void requestDestroyFromChild(const std::shared_ptr<Object>& _child);
		};
	};
};

#endif
