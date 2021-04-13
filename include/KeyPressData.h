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

#ifndef KEYCHATTERING_KEYPRESSDATA_H_
#define KEYCHATTERING_KEYPRESSDATA_H_

#include <chrono>
#include <vector>
#include <memory>
#include <mutex>
#include <string>

class KeyPressData
{
    KeyPressData(const KeyPressData&) = delete;

    struct KeyPressInfo
    {
        unsigned long keyID;
        std::chrono::time_point<std::chrono::system_clock> timeWhenPressed;
    };

    KeyPressData();
public:
    ~KeyPressData();

    static KeyPressData* createInstance();
    static KeyPressData* instance();
    static std::string keyName(unsigned long keyNumber);

    bool isKeyPressChatter(unsigned long key);
    bool isKeyReleaseChatter(unsigned long key);

    void setChatterTime(int msec);
    void enableDebug(bool enable);

private:
    int findKeyPressPos(unsigned long key) const;
    int findKeyReleasePos(unsigned long key) const;
    const KeyPressInfo& getKeyPressInfo(int pos) const;
    const KeyPressInfo& getKeyReleaseInfo(int pos) const;
    void appendKeyPressInfo(const KeyPressInfo& keyInfo);
    void appendKeyReleaseInfo(const KeyPressInfo& keyInfo);
    void setKeyPressInfoTime(int pos, std::chrono::time_point<std::chrono::system_clock> time);
    void setKeyReleaseInfoTime(int pos, std::chrono::time_point<std::chrono::system_clock> time);
    /*int keyPressInfoSize() const;
    int keyReleaseInfoSize() const;*/

    static std::unique_ptr<KeyPressData> _instance;
    static const KeyPressInfo _staticKeyPressInfo;

    std::vector<KeyPressInfo> m_keyPressInfo;
    std::vector<KeyPressInfo> m_keyReleaseInfo;
    mutable std::mutex m_keyPressMutex;
    mutable std::mutex m_keyReleaseMutex;
    std::chrono::duration<double, std::micro> m_timeOfChatter;

    bool m_isDebugEnabled;
};

#endif // KEYCHATTERING_KEYPRESSDATA_H_