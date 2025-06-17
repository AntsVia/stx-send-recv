#pragma once
#include <memory>
#include <string>

namespace tools {
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
}

template<typename Context>
struct SessionState {
    virtual ~SessionState() = default;
    virtual void Start(Context& ctx) = 0; // Begin state logic (e.g., async_read)
    virtual void OnRead(Context& ctx, const std::string& data) = 0;
};

