#include "gtest/gtest.h"
#include <cstddef>
#include <functional>
#include <memory>
#include <stdint.h>
#include <string>
#include <iostream>
#include <vector>

using namespace std;
using namespace testing;

template <typename F, typename... Args>
auto safeInvoke(F&& f, Args&&... args) {
    if (f) {
        return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    }
}

class VideoRoom {
public:
    uint32_t mAttendeeCount;

    using identify_func_t = std::function<bool()>;

    identify_func_t mUserIdentifier = nullptr;

    virtual void join(const std::string& name) {
        ++mAttendeeCount;
    };

    void leave(const std::string& name) {
        --mAttendeeCount;
    };


    void registerIdentifier(const identify_func_t& callback) {
        mUserIdentifier = callback;
    }
    bool idendifyUser(const std::string& name) {
        if(mUserIdentifier) {
            return std::invoke(mUserIdentifier);
        }
        return false;
    };

    virtual ~VideoRoom() = default;
};



TEST(InvokeTest, testFunctionInvoke)
{
    VideoRoom* pRoom = nullptr;

    pRoom = new VideoRoom();
    pRoom->registerIdentifier([](){
        cout << "user identified" << endl;
        return true;
    });
    string username = "Alice";
    pRoom->join(username);
    pRoom->idendifyUser(username);
    pRoom->leave(username);
    cout << "attendeeCount=" << pRoom->mAttendeeCount << endl;
    delete pRoom;

}