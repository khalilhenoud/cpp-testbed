/**
 * @file input.h
 * @brief Input handling.
 */
#ifndef _INPUT_H_
#define _INPUT_H_

/**
 * @namespace utils
 * @brief Holds utility functions and classes.
 */
namespace utils {

    class KeyState
    {
    public:
        unsigned char m_IsPressed;
        unsigned char m_IsTriggered;
        unsigned char m_WasPressed;
        unsigned char m_Reserved;
    };

    class Keyboard
    {
    public:
        static int IsPressed(int vKey);
        static int IsTriggered(int vKey);
        static void Update();
    };

    class Mouse
    {
    public:
        static int IsPressedLButton();
        static int IsTriggeredLButton();

        static int IsPressedMButton();
        static int IsTriggeredMButton();

        static int IsPressedRButton();
        static int IsTriggeredRButton();

        static void GetPosition(int &x, int &y);
        static void SetPosition(int x, int y);
        static void Update();
    };
}

#endif
