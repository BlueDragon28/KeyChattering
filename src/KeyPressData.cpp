/*
* MIT Licence
*
* KeyChattering
*
* Copyright © 2021 Erwan Saclier de la Bâtie (Erwan28250)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
* to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "KeyPressData.h"
#include <iostream>
#include <chrono>

#include "Windows.h"

std::unique_ptr<KeyPressData> KeyPressData::_instance = nullptr;

KeyPressData::KeyPressData() :
    m_timeOfChatter(50000.),
#ifdef NDEBUG
    m_isDebugEnabled(false),
#else
    m_isDebugEnabled(true),
#endif
    m_timeSinceProgramStarted(std::chrono::system_clock::now())
{}

KeyPressData::~KeyPressData()
{
    waitForThreadToFinish();
}

void KeyPressData::waitForThreadToFinish()
{
    for (int i = 0; i < m_threadReleaseKeys.size(); i++)
    {
        if (m_threadReleaseKeys.at(i).joinable())
            m_threadReleaseKeys[i].join();
    }
}

KeyPressData* KeyPressData::createInstance()
{
    // If exist, return the value of the _instance variable.
    // If not, create a new instance and return it.
    if (!_instance)
        _instance = std::unique_ptr<KeyPressData>(new KeyPressData());
    
    return _instance.get();
}

KeyPressData* KeyPressData::instance()
{
    return createInstance();
}

bool KeyPressData::isKeyPressChatter(unsigned long key)
{
    /*
    * Find if the key has already been pressed. If yes,
    * check if the time passed since the last press is lower
    * than the value of the variable m_timeOfChatter. If yes, 
    * it's mean the key is a chatter and need to be rejected.
    */
    auto currentTime = std::chrono::system_clock::now();

    int keyPos = findKeyPressPos(key);
    if (keyPos >= 0)
    {
        std::chrono::duration<double, std::milli> timeSinceLastPress = 
            currentTime - getKeyPressInfo(keyPos).timeWhenPressed;

        std::chrono::duration<double, std::milli> timeSinceStartingOfTheProgram =
            currentTime - m_timeSinceProgramStarted;
        
        setKeyPressTimeSinceStartingOfTheProgram(keyPos, timeSinceStartingOfTheProgram);
        
        if (timeSinceLastPress < m_timeOfChatter)
        {
            if (m_isDebugEnabled)
                std::cout << "Chatter on " << keyName(key) << " key. Time since last press: " << timeSinceLastPress.count() << " ms." << std::endl;
            return true;
        }
        else
        {
            setKeyPressInfoTime(keyPos, currentTime);
            return false;
        }
    }
    else
    {
        KeyPressInfo keyInfo = {};
        keyInfo.keyID = key;
        keyInfo.timeWhenPressed = currentTime;
        appendKeyPressInfo(keyInfo);
        return false;
    }
}

bool KeyPressData::isKeyReleaseChatter(unsigned long key)
{
    /*
    * Find if the key has already been released. If yes,
    * check if the time passed since the last release is lower
    * than the value of the variable m_timeOfChatter. If yes, 
    * it's mean the key is a chatter and need to be rejected.
    */
    auto currentTime = std::chrono::system_clock::now();

    int keyPos = findKeyReleasePos(key);
    int keyPressPos = findKeyPressPos(key);
    if (keyPos >= 0)
    {
        std::chrono::duration<double, std::milli> timeSinceStartingOfTheProgram =
            currentTime - m_timeSinceProgramStarted;
        
        setKeyReleaseTimeSinceStartingOfTheProgram(keyPos, timeSinceStartingOfTheProgram);

        // If the release of the key happen in a time since le press of the key less than m_timeOfChatter,
        // then, delaying the key release.
        if ((timeSinceStartingOfTheProgram - getKeyPressInfo(keyPressPos).timeWhenPressedSinceTheStartingOfTheProgram) < m_timeOfChatter)
        {
            m_threadReleaseKeys.push_back(std::thread(&KeyPressData::waitBeforeReleasingKey, this, key, timeSinceStartingOfTheProgram));
            std::cout << "reject release!" << std::endl;
            return true;
        }
        else
        {
            setKeyReleaseInfoTime(keyPos, currentTime);
            setKeyReleaseIsKeyPressedSinceRelease(keyPos, false);
            return false;
        }
    }
    else
    {
        KeyReleaseInfo keyInfo = {};
        keyInfo.keyID = key;
        keyInfo.timeWhenPressed = currentTime;
        keyInfo.isKeyPressedSinceRelease = false;
        appendKeyReleaseInfo(keyInfo);
        return false;
    }
}

void KeyPressData::waitBeforeReleasingKey(unsigned long key, const std::chrono::duration<double, std::milli> timeWhenKeyRelease)
{
    /*
    * This function is used to delaying the release of a key.
    * The reason is because of the chatter itself, when a key is chattering,
    * the key is release but never repressed, so the user is pressing it but in the OS
    * side, it's like the key is not pressed at all.
    * This function delaying for a certain amont of time to check after the amont of time
    * if the key need to be released.
    */

    // Waiting.
    std::this_thread::sleep_for(m_timeOfChatter*2);

    int keyPressPos = findKeyPressPos(key);
    int keyReleasePos = findKeyReleasePos(key);

    // If the time since the last release is higher than the actual release, it's mean that the key is already
    // release, so, no need to release it again.
    if (timeWhenKeyRelease < getKeyReleaseInfo(keyReleasePos).timeWhenPressedSinceTheStartingOfTheProgram)
    {
        std::cout << "A release already occured on the key " << keyName(key) << "." << std::endl;
        return;
    }

    // If the time since the last press is higher than the last press, it's mean that the user has released the key,
    // so we releasing the key.
    if (timeWhenKeyRelease > getKeyPressInfo(keyPressPos).timeWhenPressedSinceTheStartingOfTheProgram)
    {
        INPUT input[1];
        ZeroMemory(input, sizeof(input));
        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wVk = key;
        input[0].ki.dwFlags = KEYEVENTF_KEYUP;

        unsigned int result = SendInput(1, input, sizeof(INPUT));
        if (m_isDebugEnabled)
        {
            if (result != 1)
                std::cout << "Failed to release the " << keyName(key) << " key." << std::endl;
            else
                std::cout << "Key " << keyName(key) << "released!" << std::endl;
        }
    }
    // if not, the release is a chatter and need to be discarded.
    else
    {
        if (m_isDebugEnabled)
            std::cout << "discard " << keyName(key) << " releasing!" << std::endl;
    }
}

int KeyPressData::findKeyPressPos(unsigned long key) const
{
    // Find if they is already a key in the vector m_keyPressInfo.
    // If yes, return the position. If not, return -1.
    std::lock_guard<std::mutex>guard(m_keyPressMutex);
    for (int i = 0; i < m_keyPressInfo.size(); i++)
    {
        if (m_keyPressInfo.at(i).keyID == key)
            return i;
    }

    return -1;
}

int KeyPressData::findKeyReleasePos(unsigned long key) const
{
    // Find if they is already a key in the vector m_keyReleaseInfo.
    // If yes, return the position. If not, return -1.
    std::lock_guard<std::mutex>guard(m_keyReleaseMutex);
    for (int i = 0; i < m_keyReleaseInfo.size(); i++)
    {
        if (m_keyReleaseInfo.at(i).keyID == key)
            return i;
    }

    return -1;
}

const KeyPressData::KeyPressInfo KeyPressData::getKeyPressInfo(int pos) const
{
    if (pos < 0 || pos >= m_keyPressInfo.size()) 
        return {};
    std::lock_guard<std::mutex>guard(m_keyPressMutex);
    return m_keyPressInfo.at(pos);
}

const KeyPressData::KeyReleaseInfo KeyPressData::getKeyReleaseInfo(int pos) const
{
    if (pos < 0 || pos >= m_keyReleaseInfo.size())
        return {};
    std::lock_guard<std::mutex>guard(m_keyReleaseMutex);
    return m_keyReleaseInfo.at(pos);
}

void KeyPressData::appendKeyPressInfo(const KeyPressInfo& keyInfo)
{
    std::lock_guard<std::mutex>guard(m_keyPressMutex);
    m_keyPressInfo.push_back(keyInfo);
}

void KeyPressData::appendKeyReleaseInfo(const KeyReleaseInfo& keyInfo)
{
    std::lock_guard<std::mutex>guard(m_keyReleaseMutex);
    m_keyReleaseInfo.push_back(keyInfo);
}

void KeyPressData::setKeyPressInfoTime(int pos, std::chrono::time_point<std::chrono::system_clock> time)
{
    if (pos < 0 || pos >= m_keyPressInfo.size())
        return;
    std::lock_guard<std::mutex>guard(m_keyPressMutex);
    m_keyPressInfo[pos].timeWhenPressed = time;
}

void KeyPressData::setKeyReleaseInfoTime(int pos, std::chrono::time_point<std::chrono::system_clock> time)
{
    if (pos < 0 || pos >= m_keyReleaseInfo.size())
        return;
    std::lock_guard<std::mutex>guard(m_keyReleaseMutex);
    m_keyReleaseInfo[pos].timeWhenPressed = time;
}

void KeyPressData::setChatterTime(int msec)
{
    // Setting the time of chatter.
    if (msec <= 0)
        return;
    m_timeOfChatter = std::chrono::duration<double, std::micro>(double(msec) * 1000);
}

void KeyPressData::enableDebug(bool value)
{
    m_isDebugEnabled = value;
}

void KeyPressData::setKeyReleaseIsKeyPressedSinceRelease(int pos, bool value)
{
    if (pos < 0 || pos >= m_keyReleaseInfo.size())
        return;
    std::lock_guard<std::mutex>guard(m_keyReleaseMutex);
    m_keyReleaseInfo[pos].isKeyPressedSinceRelease = value;
}

void KeyPressData::setKeyPressTimeSinceStartingOfTheProgram(int pos, const std::chrono::duration<double, std::milli>& time)
{
    if (pos < 0 || pos >= m_keyPressInfo.size())
        return;
    std::lock_guard<std::mutex>guard(m_keyPressMutex);
    m_keyPressInfo[pos].timeWhenPressedSinceTheStartingOfTheProgram = time;
}

void KeyPressData::setKeyReleaseTimeSinceStartingOfTheProgram(int pos, const std::chrono::duration<double, std::milli>& time)
{
    if (pos < 0 || pos >= m_keyReleaseInfo.size())
        return;
    std::lock_guard<std::mutex>guard(m_keyReleaseMutex);
    m_keyReleaseInfo[pos].timeWhenPressedSinceTheStartingOfTheProgram = time;
}

std::string KeyPressData::keyName(unsigned long keyNumber)
{
    // Base on the windows documentation : https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
    // Return the name of a key.
    switch(keyNumber)
    {
    case VK_BACK:
        return "backspace";
    case VK_TAB:
        return "tab";
    case VK_CLEAR:
        return "clear";
    case VK_RETURN:
        return "enter";
    case VK_SHIFT:
        return "shift";
    case VK_CONTROL:
        return "control";
    case VK_MENU:
        return "alt";
    case VK_PAUSE:
        return "pause";
    case VK_CAPITAL:
        return "caps lock";
    case VK_KANA:
        return "ime kana";
    case VK_ESCAPE:
        return "escape";
    case VK_SPACE:
        return "spacebar";
    case VK_PRIOR:
        return "page up";
    case VK_NEXT:
        return "page down";
    case VK_END:
        return "end";
    case VK_HOME:
        return "home";
    case VK_LEFT:
        return "left arrow";
    case VK_UP:
        return "up arrow";
    case VK_RIGHT:
        return "right arrow";
    case VK_DOWN:
        return "down arrow";
    case VK_SELECT:
        return "select";
    case VK_PRINT:
        return "print";
    case VK_EXECUTE:
        return "execute";
    case VK_SNAPSHOT:
        return "print screen";
    case VK_INSERT:
        return "insert";
    case VK_DELETE:
        return "delete";
    case VK_HELP:
        return "HELP";
    case 0x30:
        return "0";
    case 0x31:
        return "1";
    case 0x32:
        return "2";
    case 0x33:
        return "3";
    case 0x34:
        return "4";
    case 0x35:
        return "5";
    case 0x36:
        return "6";
    case 0x37:
        return "7";
    case 0x38:
        return "8";
    case 0x39:
        return "9";
    case 0x41:
        return "A";
    case 0x42:
        return "B";
    case 0x43:
        return "C";
    case 0x44:
        return "D";
    case 0x45:
        return "E";
    case 0x46:
        return "F";
    case 0x47:
        return "G";
    case 0x48:
        return "H";
    case 0x49:
        return "I";
    case 0x4A:
        return "J";
    case 0x4B:
        return "K";
    case 0x4C:
        return "L";
    case 0x4D:
        return "M";
    case 0x4E:
        return "N";
    case 0x4F:
        return "O";
    case 0x50:
        return "P";
    case 0x51:
        return "Q";
    case 0x52:
        return "R";
    case 0x53:
        return "S";
    case 0x54:
        return "T";
    case 0x55:
        return "U";
    case 0x56:
        return "V";
    case 0x57:
        return "W";
    case 0x58:
        return "X";
    case 0x59:
        return "Y";
    case 0x5A:
        return "Z";
    case VK_LWIN:
        return "left windows";
    case VK_RWIN:
        return "right windows";
    case VK_APPS:
        return "application";
    case VK_SLEEP:
        return "computer sleep";
    case VK_NUMPAD0:
        return "numpad 0";
    case VK_NUMPAD1:
        return "numpad 1";
    case VK_NUMPAD2:
        return "numpad 2";
    case VK_NUMPAD3:
        return "numpad 3";
    case VK_NUMPAD4:
        return "numpad 4";
    case VK_NUMPAD5:
        return "numpad 5";
    case VK_NUMPAD6:
        return "numpad 6";
    case VK_NUMPAD7:
        return "numpad 7";
    case VK_NUMPAD8:
        return "numpad 8";
    case VK_NUMPAD9:
        return "numpad 9";
    case VK_MULTIPLY:
        return "muliply";
    case VK_ADD:
        return "add";
    case VK_SEPARATOR:
        return "separator";
    case VK_SUBTRACT:
        return "subtract";
    case VK_DECIMAL:
        return "decimal";
    case VK_DIVIDE:
        return "devide";
    case VK_F1:
        return "F1";
    case VK_F2:
        return "F2";
    case VK_F3:
        return "F3";
    case VK_F4:
        return "F4";
    case VK_F5:
        return "F5";
    case VK_F6:
        return "F6";
    case VK_F7:
        return "F7";
    case VK_F8:
        return "F8";
    case VK_F9:
        return "F9";
    case VK_F10:
        return "F10";
    case VK_F11:
        return "F11";
    case VK_F12:
        return "F12";
    case VK_F13:
        return "F13";
    case VK_F14:
        return "F14";
    case VK_F15:
        return "F15";
    case VK_F16:
        return "F16";
    case VK_F17:
        return "F17";
    case VK_F18:
        return "F18";
    case VK_F19:
        return "F19";
    case VK_F20:
        return "F20";
    case VK_F21:
        return "F21";
    case VK_F22:
        return "F22";
    case VK_F23:
        return "F23";
    case VK_F24:
        return "F24";
    case VK_NUMLOCK:
        return "num lock";
    case VK_SCROLL:
        return "scroll lock";
    case VK_LSHIFT:
        return "left shift";
    case VK_RSHIFT:
        return "right shift";
    case VK_LCONTROL:
        return "left control";
    case VK_RCONTROL:
        return "right control";
    case VK_LMENU:
        return "left menu";
    case VK_RMENU:
        return "right menu";
    case VK_BROWSER_BACK:
        return "browser back";
    case VK_BROWSER_FORWARD:
        return "browser forward";
    case VK_BROWSER_REFRESH:
        return "browser refresh";
    case VK_BROWSER_STOP:
        return "browser stop";
    case VK_BROWSER_SEARCH:
        return "browser search";
    case VK_BROWSER_FAVORITES:
        return "browser favorites";
    case VK_BROWSER_HOME:
        return "browser home";
    case VK_VOLUME_MUTE:    
        return "volume mute";
    case VK_VOLUME_UP:  
        return "volume up";
    case VK_VOLUME_DOWN:
        return "volume down";
    case VK_MEDIA_NEXT_TRACK:
        return "media next track";
    case VK_MEDIA_PREV_TRACK:
        return "media previous track";
    case VK_MEDIA_STOP:
        return "media stop";
    case VK_MEDIA_PLAY_PAUSE:
        return "media play/pause";
    case VK_LAUNCH_MAIL:
        return "launch mail";
    case VK_LAUNCH_APP1:
        return "launch app1";
    case VK_LAUNCH_APP2:
        return "launch app2";
    case VK_OEM_1:
        return ";: us layout";
    case VK_OEM_PLUS:
        return "+";
    case VK_OEM_COMMA:
        return ",";
    case VK_OEM_MINUS:
        return "-";
    case VK_OEM_PERIOD:
        return ".";
    case VK_OEM_2:
        return "/? us layout";
    case VK_OEM_3:
        return "~ us layout";
    case VK_OEM_4:
        return "[{ us layout";
    case VK_OEM_5:
        return "\\| us layout";
    case VK_OEM_6:
        return "]} us layout";
    case VK_OEM_7:
        return "single-quote/double quote us layout";
    case VK_PROCESSKEY:
        return "ime process";
    case VK_ATTN:
        return "attention interrupt";
    case VK_CRSEL:
        return "crsel";
    case VK_EXSEL:
        return "exsel";
    case VK_EREOF:
        return "erase eof";
    case VK_PLAY:
        return "play";
    case VK_ZOOM:
        return "zoom";
    case VK_PA1:
        return "pa1";
    case VK_OEM_CLEAR:
        return "clear";
    case VK_JUNJA:
        return "ime junja mode";
    case VK_FINAL:
        return "ime final mode";
    case VK_HANJA:
        return "ime hanja mode";
    case 0x1A:
        return "ime off";
    case VK_CONVERT:
        return "ime convert";
    case VK_NONCONVERT:
        return "ime nonconvert";
    case VK_ACCEPT:
        return "ime accept";
    case VK_MODECHANGE:
        return "ime mode change request";
    }
    return "unknow";
}