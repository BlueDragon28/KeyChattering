#include "KeyPressData.h"
#include <iostream>
#include <chrono>

std::unique_ptr<KeyPressData> KeyPressData::_instance = nullptr;
const KeyPressData::KeyPressInfo KeyPressData::_staticKeyPressInfo = {};

KeyPressData::KeyPressData() :
    m_timeOfChatter(50000.),
#ifdef NDEBUG
    m_isDebugEnabled(false)
#else
    m_isDebugEnabled(true)
#endif
{}

KeyPressData::~KeyPressData()
{}

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
        std::chrono::duration<double, std::micro> timeSinceLastPress = 
            currentTime - getKeyPressInfo(keyPos).timeWhenPressed;
        
        if (timeSinceLastPress < m_timeOfChatter)
        {
            if (m_isDebugEnabled)
                std::cout << "Chatter on key " << key << ", Time since last press: " << timeSinceLastPress.count() << std::endl;
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
    if (keyPos >= 0)
    {
        std::chrono::duration<double, std::micro> timeSinceLastPress =
            currentTime - getKeyReleaseInfo(keyPos).timeWhenPressed;
        
        if (timeSinceLastPress < m_timeOfChatter)
            return true;
        else
        {
            m_keyReleaseInfo[keyPos].timeWhenPressed = currentTime;
            setKeyReleaseInfoTime(keyPos, currentTime);
            return false;
        }
    }
    else
    {
        KeyPressInfo keyInfo = {};
        keyInfo.keyID = key;
        keyInfo.timeWhenPressed = currentTime;
        appendKeyReleaseInfo(keyInfo);
        return false;
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

const KeyPressData::KeyPressInfo& KeyPressData::getKeyPressInfo(int pos) const
{
    if (pos < 0 || pos >= m_keyPressInfo.size()) 
        return _staticKeyPressInfo;
    std::lock_guard<std::mutex>guard(m_keyPressMutex);
    return m_keyPressInfo.at(pos);
}

const KeyPressData::KeyPressInfo& KeyPressData::getKeyReleaseInfo(int pos) const
{
    if (pos < 0 || pos >= m_keyReleaseInfo.size())
        return _staticKeyPressInfo;
    std::lock_guard<std::mutex>guard(m_keyReleaseMutex);
    return m_keyReleaseInfo.at(pos);
}

void KeyPressData::appendKeyPressInfo(const KeyPressInfo& keyInfo)
{
    std::lock_guard<std::mutex>guard(m_keyPressMutex);
    m_keyPressInfo.push_back(keyInfo);
}

void KeyPressData::appendKeyReleaseInfo(const KeyPressInfo& keyInfo)
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