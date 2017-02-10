#include <windows.h>
#include "input.h"

using namespace utils;

extern HWND hWnd;

static KeyState m_Keys[256] = {0};
static KeyState m_MouseKeys[3] = {0};

int Keyboard::IsPressed(int vKey)
{
    return m_Keys[vKey].m_IsPressed;
}

int Keyboard::IsTriggered(int vKey)
{
    return m_Keys[vKey].m_IsTriggered;
}

void Keyboard::Update()
{
	int i;
	BYTE ks[256] = {0};
	GetKeyboardState(ks);

	for (i = 0; i < 256; ++i) {
		m_Keys[i].m_IsPressed = ks[i] & (1 << 7)? 1 : 0;

		if (m_Keys[i].m_IsPressed) {
			if (m_Keys[i].m_WasPressed)
				m_Keys[i].m_IsTriggered = 0;
			else {
                m_Keys[i].m_WasPressed = 1;
                m_Keys[i].m_IsTriggered = 1;
			}
		} else {
			m_Keys[i].m_IsTriggered = 0;
			m_Keys[i].m_WasPressed = 0;
		}
	}
}

int Mouse::IsPressedLButton()
{
    return m_MouseKeys[0].m_IsPressed;
}

int Mouse::IsTriggeredLButton()
{
    return m_MouseKeys[0].m_IsTriggered;
}

int Mouse::IsPressedMButton()
{
    return m_MouseKeys[1].m_IsPressed;
}

int Mouse::IsTriggeredMButton()
{
    return m_MouseKeys[1].m_IsTriggered;
}

int Mouse::IsPressedRButton()
{
    return m_MouseKeys[2].m_IsPressed;
}

int Mouse::IsTriggeredRButton()
{
    return m_MouseKeys[2].m_IsTriggered;
}

void Mouse::GetPosition(int &x, int &y)
{
	POINT pn = {0};
	GetCursorPos(&pn);
	x = pn.x;
	y = pn.y;
}

void Mouse::SetPosition(int x, int y)
{
    SetCursorPos(x, y);
}

void Mouse::Update()
{
	int i;

	SHORT lbs = GetAsyncKeyState(VK_LBUTTON);
	SHORT mbs = GetAsyncKeyState(VK_MBUTTON);
	SHORT rbs = GetAsyncKeyState(VK_RBUTTON);

	SHORT ks[3] = {0};
	ks[0] = lbs;
	ks[1] = mbs;
	ks[2] = rbs;

	for (i = 0; i < 3; ++i) {
		m_MouseKeys[i].m_IsPressed = ks[i] & (1 << 15)? 1 : 0;

		if (m_MouseKeys[i].m_IsPressed) {
			if (m_MouseKeys[i].m_WasPressed)
				m_MouseKeys[i].m_IsTriggered = 0;
			else {
                m_MouseKeys[i].m_WasPressed = 1;
                m_MouseKeys[i].m_IsTriggered = 1;
			}
		} else {
			m_MouseKeys[i].m_IsTriggered = 0;
			m_MouseKeys[i].m_WasPressed = 0;
		}
	}
}
