/**
 * \file NeLInputDriver.h
 * \date November 2009
 * \author Matt Raykowski
 * \author Henri Kuuste
 */

/* Copyright, 2004 Werewolf
 *
 * This file is part of Werewolf.
 * Werewolf is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.

 * Werewolf is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Werewolf; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#ifndef __NELINPUTDRIVER_H__
#define __NELINPUTDRIVER_H__

//
// NeL Includes
//
#include <nel/misc/common.h>
#include <nel/misc/events.h>

// input handler class
class NeLInputDriver : public NLMISC::IEventListener
{
public:
	NeLInputDriver() {
		m_MouseX=0.5f;
		m_MouseY=0.5f;
		m_Active=false;
		m_MouseActive=false;

		// init the massive key map.
		initKeyMap();
	}
	virtual ~NeLInputDriver() { ; }

	void addToServer(NLMISC::CEventServer& server) {
		server.addListener(NLMISC::EventMouseMoveId, this);
		server.addListener(NLMISC::EventMouseDownId, this);
		server.addListener(NLMISC::EventMouseUpId, this);
		server.addListener(NLMISC::EventMouseWheelId, this);
		server.addListener(NLMISC::EventKeyDownId, this);
		server.addListener(NLMISC::EventCharId, this);
		server.addListener(NLMISC::EventMouseWheelId, this);
		m_AsyncListener.addToServer(server);
	}

	void removeFromServer(NLMISC::CEventServer& server) {
		server.removeListener(NLMISC::EventMouseMoveId, this);
		server.removeListener(NLMISC::EventMouseDownId, this);
		server.removeListener(NLMISC::EventMouseUpId, this);
		server.removeListener(NLMISC::EventMouseWheelId, this);
		server.removeListener(NLMISC::EventKeyDownId, this);
		server.removeListener(NLMISC::EventCharId, this);
		server.removeListener(NLMISC::EventMouseWheelId, this);
		m_AsyncListener.removeFromServer (server);
	}
	void setScreenMode(float width, float height, float depth) { m_Width=width; m_Height=height; m_Depth=depth; }

	void setDriver(NL3D::UDriver *driver) { m_Driver=driver; }
	void deactivate() { m_Active=false; m_MouseActive=false; }
	void activate() { m_Active=true; m_MouseActive=true; }
	void activateMouse() { m_MouseActive=true; };
	void deactivateMouse() { m_MouseActive=false; };
	bool isActive() { return m_Active; }

private:
	/**
	 * \brief Accepts events from the NeL event server.
	 *
	 * \param event An event, probably a CEventMouse or CEventKey/Char.
	 */
	virtual void operator ()(const NLMISC::CEvent& event) {
		// don't process any input if we're inactive.
		if(m_Active==false) {
			return; // not processing ANY input
		}

		try
		{
			// otherwise, on with the festivities.
			 // catch ALL mouse event, just in case.
			if(event==NLMISC::EventMouseDownId||event==NLMISC::EventMouseUpId||event==NLMISC::EventMouseMoveId||event==NLMISC::EventMouseDblClkId||event==NLMISC::EventMouseWheelId) {
				if(!m_MouseActive) {
					// we're not processing any mouse activity. The cursor isn't captured maybe?
					return;
				}

				NLMISC::CEventMouse *mouseEvent=(NLMISC::CEventMouse *)&event;
				// a mouse button was pressed.
				if(event == NLMISC::EventMouseDownId) {
					// it was the left button...
					if (mouseEvent->Button & NLMISC::leftButton) {
						CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::LeftButton);
					// it was the right button...
					} else if (mouseEvent->Button & NLMISC::rightButton) {
						CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::RightButton);
					} else if (mouseEvent->Button & NLMISC::middleButton) {
						CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::MiddleButton);
					}
				// a mouse button was released
				} else if (event == NLMISC::EventMouseUpId) {
					// it was the left button...
					if(mouseEvent->Button & NLMISC::leftButton) {
						CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::LeftButton);
					// it was the right button...
					} else if (mouseEvent->Button & NLMISC::rightButton) {
						CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::RightButton);
					} else if (mouseEvent->Button & NLMISC::middleButton) {
						CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::MiddleButton);
					}
				} else if (event == NLMISC::EventMouseMoveId) {
					// convert into screen coordinates.
					float delta_x=(float)(mouseEvent->X - m_MouseX)*m_Width;
					float delta_y=(float)((1.0f-mouseEvent->Y) - m_MouseY)*m_Height;

					// inject into CEGUI
					CEGUI::System::getSingleton().injectMouseMove( delta_x, delta_y);

					// and save for delta.
					m_MouseX=mouseEvent->X;
					m_MouseY=1.0f-mouseEvent->Y;
				} else if (event == NLMISC::EventMouseWheelId) {
					NLMISC::CEventMouseWheel *ev=(NLMISC::CEventMouseWheel *)&event;
					float dir=0.0f;
					if(ev->Direction) dir=0.5f;
					else dir=-0.5f;
					CEGUI::System::getSingleton().injectMouseWheelChange(dir);
				}
			} else { // assume otherwise that it's a character.
				if(event==NLMISC::EventCharId) {
					unsigned char c = (char)((NLMISC::CEventChar&)event).Char;
					CEGUI::System::getSingleton().injectChar((CEGUI::utf32)c);
				} else if(event==NLMISC::EventKeyDownId) {
					NLMISC::CEventKeyDown *keyvent=(NLMISC::CEventKeyDown *)&event;
					CEGUI::System::getSingleton().injectKeyDown(m_KeyMap[keyvent->Key]);
				}
			}
		}
		catch (CEGUI::Exception) { }
	}

	void initKeyMap() {
		m_KeyMap[NLMISC::Key0              ]=CEGUI::Key::Zero;
		m_KeyMap[NLMISC::Key1              ]=CEGUI::Key::One;
		m_KeyMap[NLMISC::Key2              ]=CEGUI::Key::Two;
		m_KeyMap[NLMISC::Key3              ]=CEGUI::Key::Three;
		m_KeyMap[NLMISC::Key4              ]=CEGUI::Key::Four;
		m_KeyMap[NLMISC::Key5              ]=CEGUI::Key::Five;
		m_KeyMap[NLMISC::Key6              ]=CEGUI::Key::Six;
		m_KeyMap[NLMISC::Key7              ]=CEGUI::Key::Seven;
		m_KeyMap[NLMISC::Key8              ]=CEGUI::Key::Eight;
		m_KeyMap[NLMISC::Key9              ]=CEGUI::Key::Nine;
		m_KeyMap[NLMISC::KeyA              ]=CEGUI::Key::A;
		m_KeyMap[NLMISC::KeyB              ]=CEGUI::Key::B;
		m_KeyMap[NLMISC::KeyC              ]=CEGUI::Key::C;
		m_KeyMap[NLMISC::KeyD              ]=CEGUI::Key::D;
		m_KeyMap[NLMISC::KeyE              ]=CEGUI::Key::E;
		m_KeyMap[NLMISC::KeyF              ]=CEGUI::Key::F;
		m_KeyMap[NLMISC::KeyG              ]=CEGUI::Key::G;
		m_KeyMap[NLMISC::KeyH              ]=CEGUI::Key::H;
		m_KeyMap[NLMISC::KeyI              ]=CEGUI::Key::I;
		m_KeyMap[NLMISC::KeyJ              ]=CEGUI::Key::J;
		m_KeyMap[NLMISC::KeyK              ]=CEGUI::Key::K;
		m_KeyMap[NLMISC::KeyL              ]=CEGUI::Key::L;
		m_KeyMap[NLMISC::KeyM              ]=CEGUI::Key::M;
		m_KeyMap[NLMISC::KeyN              ]=CEGUI::Key::N;
		m_KeyMap[NLMISC::KeyO              ]=CEGUI::Key::O;
		m_KeyMap[NLMISC::KeyP              ]=CEGUI::Key::P;
		m_KeyMap[NLMISC::KeyQ              ]=CEGUI::Key::Q;
		m_KeyMap[NLMISC::KeyR              ]=CEGUI::Key::R;
		m_KeyMap[NLMISC::KeyS              ]=CEGUI::Key::S;
		m_KeyMap[NLMISC::KeyT              ]=CEGUI::Key::T;
		m_KeyMap[NLMISC::KeyU              ]=CEGUI::Key::U;
		m_KeyMap[NLMISC::KeyV              ]=CEGUI::Key::V;
		m_KeyMap[NLMISC::KeyW              ]=CEGUI::Key::W;
		m_KeyMap[NLMISC::KeyX              ]=CEGUI::Key::X;
		m_KeyMap[NLMISC::KeyY              ]=CEGUI::Key::Y;
		m_KeyMap[NLMISC::KeyZ              ]=CEGUI::Key::Z;
		m_KeyMap[NLMISC::KeySUBTRACT       ]=CEGUI::Key::Minus;
		m_KeyMap[NLMISC::KeyEQUALS         ]=CEGUI::Key::Equals;
		m_KeyMap[NLMISC::KeyBACK           ]=CEGUI::Key::Backspace;
		m_KeyMap[NLMISC::KeyTAB            ]=CEGUI::Key::Tab;
		m_KeyMap[NLMISC::KeyLBRACKET       ]=CEGUI::Key::LeftBracket;
		m_KeyMap[NLMISC::KeyRBRACKET       ]=CEGUI::Key::RightBracket;
		m_KeyMap[NLMISC::KeyRETURN         ]=CEGUI::Key::Return;
		m_KeyMap[NLMISC::KeyLCONTROL       ]=CEGUI::Key::LeftControl;
		m_KeyMap[NLMISC::KeySEMICOLON      ]=CEGUI::Key::Semicolon;
		m_KeyMap[NLMISC::KeyAPOSTROPHE     ]=CEGUI::Key::Apostrophe;
		m_KeyMap[NLMISC::KeyLSHIFT         ]=CEGUI::Key::LeftShift;
		m_KeyMap[NLMISC::KeyBACKSLASH      ]=CEGUI::Key::Backslash;
		m_KeyMap[NLMISC::KeyCOMMA          ]=CEGUI::Key::Comma;
		m_KeyMap[NLMISC::KeyPERIOD         ]=CEGUI::Key::Period;
		m_KeyMap[NLMISC::KeySLASH          ]=CEGUI::Key::Slash;
				m_KeyMap[NLMISC::KeyRSHIFT         ]=CEGUI::Key::RightShift;
		m_KeyMap[NLMISC::KeyMULTIPLY       ]=CEGUI::Key::Multiply;
		m_KeyMap[NLMISC::KeySPACE          ]=CEGUI::Key::Space;
		m_KeyMap[NLMISC::KeyCAPITAL        ]=CEGUI::Key::Capital;
		m_KeyMap[NLMISC::KeyF1             ]=CEGUI::Key::F1;
		m_KeyMap[NLMISC::KeyF2             ]=CEGUI::Key::F2;
		m_KeyMap[NLMISC::KeyF3             ]=CEGUI::Key::F3;
		m_KeyMap[NLMISC::KeyF4             ]=CEGUI::Key::F4;
		m_KeyMap[NLMISC::KeyF5             ]=CEGUI::Key::F5;
		m_KeyMap[NLMISC::KeyF6             ]=CEGUI::Key::F6;
		m_KeyMap[NLMISC::KeyF7             ]=CEGUI::Key::F7;
		m_KeyMap[NLMISC::KeyF8             ]=CEGUI::Key::F8;
		m_KeyMap[NLMISC::KeyF9             ]=CEGUI::Key::F9;
		m_KeyMap[NLMISC::KeyF10            ]=CEGUI::Key::F10;
		m_KeyMap[NLMISC::KeyNUMLOCK        ]=CEGUI::Key::NumLock;
		m_KeyMap[NLMISC::KeySCROLL         ]=CEGUI::Key::ScrollLock;
		m_KeyMap[NLMISC::KeyNUMPAD0        ]=CEGUI::Key::Numpad0;
		m_KeyMap[NLMISC::KeyNUMPAD1        ]=CEGUI::Key::Numpad1;
		m_KeyMap[NLMISC::KeyNUMPAD2        ]=CEGUI::Key::Numpad2;
		m_KeyMap[NLMISC::KeyNUMPAD3        ]=CEGUI::Key::Numpad3;
		m_KeyMap[NLMISC::KeyNUMPAD4        ]=CEGUI::Key::Numpad4;
		m_KeyMap[NLMISC::KeyNUMPAD5        ]=CEGUI::Key::Numpad5;
		m_KeyMap[NLMISC::KeyNUMPAD6        ]=CEGUI::Key::Numpad6;
		m_KeyMap[NLMISC::KeyNUMPAD7        ]=CEGUI::Key::Numpad7;
		m_KeyMap[NLMISC::KeyNUMPAD8        ]=CEGUI::Key::Numpad8;
		m_KeyMap[NLMISC::KeyNUMPAD9        ]=CEGUI::Key::Numpad9;
		m_KeyMap[NLMISC::KeyADD            ]=CEGUI::Key::Add;
		m_KeyMap[NLMISC::KeyDECIMAL        ]=CEGUI::Key::Decimal;
		m_KeyMap[NLMISC::KeyOEM_CLEAR      ]=CEGUI::Key::OEM_102;
		m_KeyMap[NLMISC::KeyF11            ]=CEGUI::Key::F11;
		m_KeyMap[NLMISC::KeyF12            ]=CEGUI::Key::F12;
		m_KeyMap[NLMISC::KeyF13            ]=CEGUI::Key::F13;
		m_KeyMap[NLMISC::KeyF14            ]=CEGUI::Key::F14;
		m_KeyMap[NLMISC::KeyF15            ]=CEGUI::Key::F15;
		m_KeyMap[NLMISC::KeyKANA           ]=CEGUI::Key::Kana;
		m_KeyMap[NLMISC::KeyCONVERT        ]=CEGUI::Key::Convert;
		m_KeyMap[NLMISC::KeyNONCONVERT     ]=CEGUI::Key::NoConvert;
		m_KeyMap[NLMISC::KeySEPARATOR      ]=CEGUI::Key::Colon;
		m_KeyMap[NLMISC::KeyKANJI          ]=CEGUI::Key::Kanji;
		m_KeyMap[NLMISC::KeyRCONTROL       ]=CEGUI::Key::RightControl;
		m_KeyMap[NLMISC::KeyLMENU          ]=CEGUI::Key::LeftAlt;
		m_KeyMap[NLMISC::KeyDIVIDE         ]=CEGUI::Key::Divide;
		m_KeyMap[NLMISC::KeyPRINT          ]=CEGUI::Key::SysRq;
		m_KeyMap[NLMISC::KeyRMENU          ]=CEGUI::Key::RightAlt;
		m_KeyMap[NLMISC::KeyPAUSE          ]=CEGUI::Key::Pause;
		m_KeyMap[NLMISC::KeyHOME           ]=CEGUI::Key::Home;
		m_KeyMap[NLMISC::KeyUP             ]=CEGUI::Key::ArrowUp;
		m_KeyMap[NLMISC::KeyPRIOR          ]=CEGUI::Key::PageUp;
		m_KeyMap[NLMISC::KeyLEFT           ]=CEGUI::Key::ArrowLeft;
		m_KeyMap[NLMISC::KeyRIGHT          ]=CEGUI::Key::ArrowRight;
		m_KeyMap[NLMISC::KeyEND            ]=CEGUI::Key::End;
		m_KeyMap[NLMISC::KeyDOWN           ]=CEGUI::Key::ArrowDown;
		m_KeyMap[NLMISC::KeyNEXT           ]=CEGUI::Key::PageDown;
		m_KeyMap[NLMISC::KeyINSERT         ]=CEGUI::Key::Insert;
		m_KeyMap[NLMISC::KeyDELETE         ]=CEGUI::Key::Delete;
		m_KeyMap[NLMISC::KeyLWIN           ]=CEGUI::Key::LeftWindows;
		m_KeyMap[NLMISC::KeyRWIN           ]=CEGUI::Key::RightWindow;
		m_KeyMap[NLMISC::KeyAPPS           ]=CEGUI::Key::AppMenu;
	}
	float m_MouseX;
	float m_MouseY;
	// screen mode, used for calculating mouse coordinates
	float m_Width;
	float m_Height;
	float m_Depth;
	bool m_Active;
	bool m_MouseActive;
	// key mapping
	std::map<NLMISC::TKey, CEGUI::Key::Scan> m_KeyMap;
	NLMISC::CEventListenerAsync	m_AsyncListener;
	NL3D::UDriver *m_Driver;
};


#endif // __NELINPUTDRIVER_H__

