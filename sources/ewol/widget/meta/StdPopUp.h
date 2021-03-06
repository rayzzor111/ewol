/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#ifndef __EWOL_STD_POP_UP_H__
#define __EWOL_STD_POP_UP_H__

#include <ewol/widget/PopUp.h>
#include <ewol/widget/Label.h>
#include <ewol/widget/Button.h>
#include <ewol/widget/Sizer.h>
namespace ewol {
	namespace widget {
		/**
		 * @brief The std pop up widget is a siple message widget to notify user of some simple things, like:
		 *
		 * [pre]
		 * 	+---------------------------------+---+---+---+
		 * 	| Windows name...                 | _ | O | X |
		 * 	+---------------------------------+---+---+---+
		 * 	|                                             |
		 * 	|                                             |
		 * 	|                                             |
		 * 	|            +-------------------+            |
		 * 	|            | Erreur:           |            |
		 * 	|            |                   |            |
		 * 	|            | Message to diplay |            |
		 * 	|            | to user           |            |
		 * 	|            |                   |            |
		 * 	|            |             Close |            |
		 * 	|            +-------------------+            |
		 * 	|                                             |
		 * 	|                                             |
		 * 	|                                             |
		 * 	+---------------------------------------------+
		 * [/pre]
		 */
		class StdPopUp : public ewol::widget::PopUp {
			protected:
				/**
				 * @brief std-pop-up constructor.
				 */
				StdPopUp();
				void init();
			public:
				DECLARE_WIDGET_FACTORY(StdPopUp, "StdPopUp");
				/**
				 * @brief std-pop-up destructor.
				 */
				virtual ~StdPopUp();
			protected:
				std::shared_ptr<ewol::widget::Label> m_title; //!< Title Label widget
			public:
				/**
				 * @brief Set the title string.
				 * @param[in] _text Decorated text to diplay in title.
				 */
				void setTitle(const std::string& _text);
			protected:
				std::shared_ptr<ewol::widget::Label> m_comment; //!< Comment label widget
			public:
				/**
				 * @brief Set the commentary string.
				 * @param[in] _text Decorated text to diplay in Comment.
				 */
				void setComment(const std::string& _text);
			protected:
				std::shared_ptr<ewol::widget::Sizer> m_subBar; //!< subwidget bar containing all the button.
			public:
				/**
				 * @brief Add a buttom button.
				 * @param[in] _text Decorated text to diplay in button.
				 */
				std::shared_ptr<ewol::widget::Button> addButton(const std::string& _text, bool _autoExit=false);
			public: // callback function
				void onCallBackButtonExit();
		};
	};
};

#endif
