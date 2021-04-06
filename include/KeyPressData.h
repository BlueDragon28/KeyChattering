#ifndef KEYCHATTERING_KEYPRESSDATA_H_
#define KEYCHATTERING_KEYPRESSDATA_H_

#include <chrono>
#include <vector>
#include <memory>
#include <mutex>

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

    bool isKeyPressChatter(unsigned long key);
    bool isKeyReleaseChatter(unsigned long key);

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

    std::vector<KeyPressInfo> m_keyPressInfo;
    std::vector<KeyPressInfo> m_keyReleaseInfo;
    mutable std::mutex m_keyPressMutex;
    mutable std::mutex m_keyReleaseMutex;
    std::chrono::duration<double, std::micro> m_timeOfChatter;
};

#endif // KEYCHATTERING_KEYPRESSDATA_H_