/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license BSD v3 (see license file)
 */


#include <ewol/debug.h>
#include <ewol/ewol.h>

#include <etk/math/Vector2D.h>
#include <etk/math/Vector3D.h>
#include <etk/UString.h>
#include <etk/unicode.h>
#include <ewol/widget/WidgetManager.h>

#include <ewol/resources/Texture.h>
#include <ewol/resources/Image.h>
#include <ewol/renderer/eContext.h>
#include <ewol/renderer/openGL.h>

#include <sys/time.h>
#include <windows.h>
#include <windowsx.h>



int64_t ewol::getTime(void) {
    struct timeval now;
    gettimeofday(&now, NULL);
    //EWOL_VERBOSE("current time : " << now.tv_sec << "s " << now.tv_usec << "us");
    return (int64_t)((int64_t)now.tv_sec*(int64_t)1000000 + (int64_t)now.tv_usec);
}


#undef __class__
#define __class__ "ContextWindows"


class WindowsContext : public ewol::eContext {
	private:
		int32_t m_currentHeight = 0;
		bool m_inputIsPressed[MAX_MANAGE_INPUT];
		ewol::SpecialKey m_guiKeyBoardMode;
		bool m_run = true;
		bool m_clipBoardOwnerStd = false;
	public:
		WindowsContext(int32_t _argc, char* _argv[]) :
		  ewol::eContext(_argc, _argv) {
			for (int32_t iii=0; iii<MAX_MANAGE_INPUT; ++iii) {
				m_inputIsPressed[iii] = false;
			}
		}
		
		~WindowsContext(void) {
			
		}
		
		int32_t Run(void) {
			HINSTANCE hInstance = 0;
			WNDCLASS wc;
			HWND hWnd;
			HDC hDC;
			HGLRC hRC;
			MSG msg;
			
			// register window class
			wc.style = CS_OWNDC;
			wc.lpfnWndProc = WndProc;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.hInstance = hInstance;
			wc.hIcon = loadIcon( NULL, IDI_APPLICATION );
			wc.hCursor = loadCursor( NULL, IDC_ARROW );
			wc.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
			wc.lpszMenuName = NULL;
			wc.lpszClassName = "EwolMainWindows";
			RegisterClass( &wc );
			
			// create main window
			hWnd = CreateWindow( "EwolMainWindows", "Ewol ... TODO Title", 
			                     WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE | WS_SIZEBOX,
			                     0, 0, 800, 600,
			                     NULL, NULL, hInstance, NULL );
			int border_thickness = getSystemMetrics(SM_CXSIZEFRAME);
			int title_size = getSystemMetrics(SM_CYCAPTION);
			m_currentHeight = 600-2*border_thickness -title_size;
			OS_Resize(800-2*border_thickness, m_currentHeight);
			
			// enable openGL for the window
			enableOpenGL( hWnd, &hDC, &hRC );
			
			// program main loop
			while(true == m_run) {
				// check for messages
				if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )) {
					// handle or dispatch messages
					if ( msg.message == WM_QUIT ) {
						m_run = false;
					} else {
						translateMessage( &msg );
						DispatchMessage( &msg );
					}
				} else {
					(void)Draw(true);
					SwapBuffers( hDC );
				}
			}
			// shutdown openGL
			disableOpenGL( hWnd, hDC, hRC );
			// destroy the window explicitly
			DestroyWindow( hWnd );
			return msg.wParam;
		}
		
		void Stop(void) {
			m_run = false;
			// To exit program ...
			PostQuitMessage(0);
		}
		
		void setSize(const vec2& _size) {
			int border_thickness = getSystemMetrics(SM_CXSIZEFRAME);
			int title_size = getSystemMetrics(SM_CYCAPTION);
			size.setValue(_size.x() + border_thickness*2,
			              _size.y() + border_thickness*2 + title_size);
			//m_currentHeight = size.y;
			// TODO : Later
		}
		
		void ClipBoardGet(enum ewol::clipBoard::clipboardListe _clipboardID) {
			// this is to force the local system to think we have the buffer
			// TODO : remove this 2 line when code will be writen
			l_clipBoardOwnerStd = true;
			switch (_clipboardID)
			{
				case ewol::clipBoard::clipboardSelection:
					// NOTE : Windows does not support the middle button the we do it internaly
					// just transmit an event , we have the data in the system
					ClipBoardArrive(_clipboardID);
					break;
				case ewol::clipBoard::clipboardStd:
					if (false == l_clipBoardOwnerStd) {
						// generate a request TO the OS
						// TODO : Send the message to the OS "We disire to receive the copy buffer ...
					} else {
						// just transmit an event , we have the data in the system
						ClipBoardArrive(_clipboardID);
					}
					break;
				default:
					EWOL_ERROR("Request an unknow ClipBoard ...");
					break;
			}
		}
		
		void ClipBoardSet(enum ewol::clipBoard::clipboardListe _clipboardID) {
			switch (_clipboardID)
			{
				case ewol::clipBoard::clipboardSelection:
					// NOTE : nothing to do : Windows deas ot supported Middle button
					break;
				case ewol::clipBoard::clipboardStd:
					// Request the clipBoard :
					if (false == m_clipBoardOwnerStd) {
						// TODO : Inform the OS that we have the current buffer of copy ...
						m_clipBoardOwnerStd = true;
					}
					break;
				default:
					EWOL_ERROR("Request an unknow ClipBoard ...");
					break;
			}
		}
		
		
		
		// enable openGL
		void enableOpenGL(HWND _hWnd, HDC* _hDC, HGLRC* _hRC) {
			// get the device context (DC)
			*hDC = getDC( _hWnd );
			
			PIXELFORMATDESCRIPTOR pfd;
			// set the pixel format for the DC
			ZeroMemory( &pfd, sizeof( pfd ) );
			pfd.nSize = sizeof( pfd );
			pfd.nVersion = 1;
			pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.cColorBits = 24;
			pfd.cDepthBits = 16;
			pfd.iLayerType = PFD_MAIN_PLANE;
			int format = ChoosePixelFormat( *_hDC, &pfd );
			setPixelFormat( *_hDC, format, &pfd );
			
			// create and enable the render context (RC)
			*_hRC = wglCreateContext( *_hDC );
			wglMakeCurrent( *_hDC, *_hRC );
			
		}
		
		// disable openGL
		
		void disableOpenGL(HWND _hWnd, HDC _hDC, HGLRC _hRC) {
			wglMakeCurrent( NULL, NULL );
			wglDeleteContext( _hRC );
			releaseDC( _hWnd, _hDC );
		}
		
		
		// Window Procedure
		static LRESULT CALLBACK WndProc(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam) {
			// TODO : set this function really work...
			classPointer->WndProcReal(_hWnd, _message, _wParam, _lParam);
		}
		
		LRESULT CALLBACK WndProcReal(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam) {
			bool buttonIsDown = true;
			int32_t mouseButtonId = 0;
			ivec2 pos;
			// to know all _message : http://wiki.winehq.org/List_Of_Windows__messages
			switch (_message)
			{
				/* **************************************************************************
				 *                             Gui event
				 * **************************************************************************/
				case WM_CREATE:
					EWOL_DEBUG("WM_CREATE");
					return 0;
				case WM_CLOSE:
					EWOL_DEBUG("WM_CLOSE");
					PostQuitMessage( 0 );
					return 0;
				case WM_DESTROY:
					EWOL_DEBUG("WM_DESTROY");
					return 0;
				case WM_MOVE:
					EWOL_DEBUG("WM_MOVE");
					return 0;
				case WM_SIZE:
					EWOL_DEBUG("WM_SIZE");
					return 0;
				/*
				case WM_GETMINMAXINFO:
					{
						MINMAXINFO* tmpVal = (MINMAXINFO*)lParam;
						EWOL_DEBUG("WM_GETMINMAXINFO : ");
						EWOL_DEBUG("    ptMaxSize : " << tmpVal->ptMaxSize.x << "," << tmpVal->ptMaxSize.y << ")");
						EWOL_DEBUG("    ptMaxPosition : " << tmpVal->ptMaxPosition.x << "," << tmpVal->ptMaxPosition.y << ")");
						EWOL_DEBUG("    ptMinTrackSize : " << tmpVal->ptMinTrackSize.x << "," << tmpVal->ptMinTrackSize.y << ")");
						EWOL_DEBUG("    ptMaxTrackSize : " << tmpVal->ptMaxTrackSize.x << "," << tmpVal->ptMaxTrackSize.y << ")");
					}
					return 0;
				*/
				case WM_WINDOWPOSCHANGING: {
					WINDOWPOS* tmpVal = (WINDOWPOS*)_lParam;
					if (NULL != tmpVal) {
						//EWOL_DEBUG("WM_WINDOWPOSCHANGING : : (" << tmpVal->x << "," << tmpVal->y << ") ( " << tmpVal->cx << "," << tmpVal->cy << ")");
						// in windows system, we need to remove the size of the border elements : 
						int border_thickness = getSystemMetrics(SM_CXSIZEFRAME);
						int title_size = getSystemMetrics(SM_CYCAPTION);
						m_currentHeight = tmpVal->cy - 2*border_thickness - title_size;
						OS_Resize(tmpVal->cx-2*border_thickness, m_currentHeight);
					}
					return 0;
				}
				// these message are not parse by us ...
				case WM_SETCURSOR: // Call the windows if we want the mouse event :
				case WM_NCHITTEST: // inform the application the position of the mouse is moving
					return DefWindowProc( _hWnd, _message, _wParam, _lParam );
				
				/* **************************************************************************
				 *                             Keyboard management
				 * **************************************************************************/
				case WM_KEYUP:
					buttonIsDown = false;
				case WM_KEYDOWN: {
					etk::UChar tmpChar = 0;
					enum ewol::keyEvent::keyboard keyInput;
					switch (_wParam) {
						//case 80: // keypad
						case VK_UP:     keyInput = ewol::keyEvent::keyboardUp; break;
						//case 83: // keypad
						case VK_LEFT:   keyInput = ewol::keyEvent::keyboardLeft; break;
						//case 85: // keypad
						case VK_RIGHT:  keyInput = ewol::keyEvent::keyboardRight; break;
						//case 88: // keypad
						case VK_DOWN:   keyInput = ewol::keyEvent::keyboardDown; break;
						//case 81: // keypad
						case VK_PRIOR:  keyInput = ewol::keyEvent::keyboardPageUp; break;
						//case 89: // keypad
						case VK_NEXT:   keyInput = ewol::keyEvent::keyboardPageDown; break;
						//case 79: // keypad
						case VK_HOME:   keyInput = ewol::keyEvent::keyboardStart; break;
						//case 87: // keypad
						case VK_END:    keyInput = ewol::keyEvent::keyboardEnd; break;
						//case VK_:     keyInput = ewol::keyEvent::keyboardStopDefil; break;
						case VK_PAUSE:  keyInput = ewol::keyEvent::keyboardWait; break;
						//case 90: // keypad
						case VK_INSERT:
							keyInput = ewol::keyEvent::keyboardInsert;
							guiKeyBoardMode.insert = buttonIsDown;
							break;
						case VK_F1: keyInput = ewol::keyEvent::keyboardF1; break;
						case VK_F2: keyInput = ewol::keyEvent::keyboardF2; break;
						case VK_F3: keyInput = ewol::keyEvent::keyboardF3; break;
						case VK_F4: keyInput = ewol::keyEvent::keyboardF4; break;
						case VK_F5: keyInput = ewol::keyEvent::keyboardF5; break;
						case VK_F6: keyInput = ewol::keyEvent::keyboardF6; break;
						case VK_F7: keyInput = ewol::keyEvent::keyboardF7; break;
						case VK_F8: keyInput = ewol::keyEvent::keyboardF8; break;
						case VK_F9: keyInput = ewol::keyEvent::keyboardF9; break;
						case VK_F10: keyInput = ewol::keyEvent::keyboardF10; break;
						case VK_F11: keyInput = ewol::keyEvent::keyboardF11; break;
						case VK_F12:
						case VK_F13:
						case VK_F14:
						case VK_F15:
						case VK_F16:
						case VK_F17:
						case VK_F18:
						case VK_F19:
						case VK_F20:
						case VK_F21:
						case VK_F22:
						case VK_F23:
						case VK_F24: keyInput = ewol::keyEvent::keyboardF12; break;
						case VK_CAPITAL:   keyInput = ewol::keyEvent::keyboardCapLock;    guiKeyBoardMode.capLock = buttonIsDown; break;
						
						case VK_SHIFT:
						case VK_LSHIFT:    keyInput = ewol::keyEvent::keyboardShiftLeft;  guiKeyBoardMode.shift   = buttonIsDown; break;
						case VK_RSHIFT:    keyInput = ewol::keyEvent::keyboardShiftRight; guiKeyBoardMode.shift   = buttonIsDown; break;
						
						case VK_CONTROL:
						case VK_LCONTROL:  keyInput = ewol::keyEvent::keyboardCtrlLeft;   guiKeyBoardMode.ctrl    = buttonIsDown; break;
						case VK_RCONTROL:  keyInput = ewol::keyEvent::keyboardCtrlRight;  guiKeyBoardMode.ctrl    = buttonIsDown; break;
						
						case VK_LWIN:      keyInput = ewol::keyEvent::keyboardMetaLeft;   guiKeyBoardMode.meta    = buttonIsDown; break;
						case VK_RWIN:      keyInput = ewol::keyEvent::keyboardMetaRight;  guiKeyBoardMode.meta    = buttonIsDown; break;
						
						case VK_MENU:
						case VK_LMENU:     keyInput = ewol::keyEvent::keyboardAlt;        guiKeyBoardMode.alt     = buttonIsDown; break;
						case VK_RMENU:     keyInput = ewol::keyEvent::keyboardAltGr;      guiKeyBoardMode.altGr   = buttonIsDown; break;
						//case :   keyInput = ewol::keyEvent::keyboardContextMenu; break;
						case VK_NUMLOCK:   keyInput = ewol::keyEvent::keyboardNumLock;    guiKeyBoardMode.numLock = buttonIsDown; break;
						case VK_BACK: // DEL
							tmpChar.set(0x08);
							break;
						// TODO : Really strang, need to understand why ...
						case 46: // Suppr
							tmpChar.set(0x7F);
							break;
						case VK_TAB: // special case for TAB
							tmpChar.set(0x09);
							break;
						case VK_RETURN: // special case for TAB
							tmpChar = '\n';
							break;
						default:
							{
								BYTE kbd[256];
								getKeyboardState(kbd);
								const int BUFFER_LENGTH = 8; //Length of the buffer
								WCHAR chars[BUFFER_LENGTH];
								
								ToUnicode(wParam,lParam,kbd,chars,BUFFER_LENGTH,0);
								tmpChar.setUtf8((char*)chars);
							}
							break;
					}
					EWOL_DEBUG("kjhkjhkjhkjhkj = " << _wParam);
					if (tmpChar == 0) {
						//EWOL_DEBUG("eventKey Move type : " << getCharTypeMoveEvent(keyInput) );
						OS_SetKeyboardMove(guiKeyBoardMode, keyInput, buttonIsDown);
					} else {
						OS_SetKeyboard(guiKeyBoardMode, tmpChar, buttonIsDown);
					}
					return 0;
				}
				/* **************************************************************************
				 *                             Mouse management
				 * **************************************************************************/
				case WM_LBUTTONUP:
					buttonIsDown = false;
				case WM_LBUTTONDOWN:
					mouseButtonId = 1;
					pos.setValue(GET_X_LPARAM(_lParam),
					             m_currentHeight-GET_Y_LPARAM(_lParam));
					inputIsPressed[mouseButtonId] = buttonIsDown;
					OS_SetMouseState(mouseButtonId, buttonIsDown, (float)pos.x(), (float)pos.y());
					return 0;
				
				case WM_MBUTTONUP:
					buttonIsDown = false;
				case WM_MBUTTONDOWN:
					mouseButtonId = 2;
					pos.setValue(GET_X_LPARAM(_lParam),
					             m_currentHeight-GET_Y_LPARAM(_lParam));
					inputIsPressed[mouseButtonId] = buttonIsDown;
					OS_SetMouseState(mouseButtonId, buttonIsDown, (float)pos.x(), (float)pos.y());
					return 0;
				
				case WM_RBUTTONUP:
					buttonIsDown = false;
				case WM_RBUTTONDOWN:
					mouseButtonId = 3;
					pos.setValue(GET_X_LPARAM(_lParam),
					             m_currentHeight-GET_Y_LPARAM(_lParam));
					inputIsPressed[mouseButtonId] = buttonIsDown;
					OS_SetMouseState(mouseButtonId, buttonIsDown, (float)pos.x(), (float)pos.y());
					return 0;
				
				case WM_MOUSEWHEEL:
					if (wParam & 0x200000) {
						EWOL_DEBUG("event SCROOL UP");
						mouseButtonId = 4;
					} else{
						EWOL_DEBUG("event SCROOL DOWN");
						mouseButtonId = 5;
					}
					pos.setValue(GET_X_LPARAM(_lParam),
					             m_currentHeight-GET_Y_LPARAM(_lParam));
					OS_SetMouseState(mouseButtonId, true,  (float)pos.x(), (float)pos.y());
					OS_SetMouseState(mouseButtonId, false, (float)pos.x(), (float)pos.y());
					return 0;
				
				case WM_MOUSEHOVER:
				case WM_MOUSEMOVE:
					pos.setValue(GET_X_LPARAM(_lParam),
					             m_currentHeight-GET_Y_LPARAM(_lParam));
					for (int32_t iii=0; iii<NB_MAX_INPUT ; iii++) {
						if (true == inputIsPressed[iii]) {
							EWOL_VERBOSE("Windows event: bt=" << iii << " " << _message << " = \"WM_MOUSEMOVE\" " << pos );
							OS_SetMouseMotion(iii, (float)pos.x(), (float)pos.y());
							return 0;
						}
					}
					EWOL_VERBOSE("Windows event: bt=" << 0 << " " << _message << " = \"WM_MOUSEMOVE\" " << pos );
					OS_SetMouseMotion(0, (float)pos.x(), (float)pos.y());
					return 0;
				
				default:
					EWOL_DEBUG("event ..." << message );
					return DefWindowProc( _hWnd, _message, _wParam, _lParam );
			}
			
		}
}

/**
 * @brief Main of the program
 * @param std IO
 * @return std IO
 */
int ewol::Run(int _argc, const char *_argv[]) {
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		/* Problem: glewInit failed, something is seriously wrong. */
		EWOL_ERROR("Error:" << glewGetErrorString(err));
	}
	if (!glewIsSupported("GL_VERSION_2_0")) {
		EWOL_ERROR("OpenGL 2.0 not available");
		//return 1;
	}
	WindowsContext* interface = new WindowsContext(_argc, _argv);
	if (NULL == interface) {
		EWOL_CRITICAL("Can not create the X11 interface ... MEMORY allocation error");
		return -2;
	}
	
	int32_t retValue = interface->Run();
	delete(interface);
	interface = NULL;
	return retValue;
}

