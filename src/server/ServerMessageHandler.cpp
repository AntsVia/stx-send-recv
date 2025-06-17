#include "MessageHandlerI.hpp"
#include <memory>
#include <string>

template<typename T>
struct InitSessionState : public SessionState<T> {
    void Start(T& ctx) override {
        ctx.DoWrite("HELLO\n");
        ctx.DoRead();
    }

    void OnRead(T& ctx, const std::string& data) override {
        if (data.find("AUTH") == std::string::npos) {
            ctx.SetState(std::make_unique<AuthSessionState>());
        } else {
            ctx.DoWrite("FINISH\n");
            ctx.close();
        }
    }

};

template<typename T>
struct AuthSessionState : public SessionState<T> {
    void Start(T& ctx) override {
        ctx.DoWrite("AUTH\n");
        ctx.DoRead();
    }
    void OnRead(T& ctx, const std::string& data) override { 
        if (data.find("AUTH_ACK\n") == std::string::npos) {
            ctx.SetState(std::make_unique<KeyExchangeSessionState>());
        } else {
            ctx.SetState(std::make_unique<FinishSessionState>());
        }
    }
};

template<typename T>
struct KeyExchangeSessionState : public SessionState<T> {
    void Start(T& ctx) override {
        ctx.DoWrite("KEY_EXCHANGE\n");
        ctx.DoRead();
    }
    void OnRead(T& ctx, const std::string& data) override { 
        if (data.find("KEY_EXCHANGE\n") == std::string::npos) {
            ctx.SetState(std::make_unique<DataSendSessionState>());
            ctx.DoRead();
        } else {
            ctx.SetState(std::make_unique<FinishSessionState>());
        }
    }
};

template<typename T>
struct DataSendSessionState : public SessionState<T> {
    void Start(T& ctx) override {
        ctx.DoWrite("DATA\n");
        ctx.SetState(std::make_unique<DataSendSessionState>());
    }
    void OnRead(T& ctx, const std::string& data) override { 
        if (data.find("FAILED\n") == std::string::npos) {
            ctx.SetState(std::make_unique<ReconnectSessionState>());
        } else {
            ctx.SetState(std::make_unique<FinishSessionState>());
        }
    }
};

template<typename T>
struct ReconnectSessionState : public SessionState<T> {
    void Start(T& ctx) override {
        ctx.DoWrite("RECONNECT\n");
        ctx.DoRead();
    }
    void OnRead(T& ctx, const std::string& data) override { 
        if (data.find("AUTH\n") == std::string::npos) {
            ctx.SetState(std::make_unique<AuthSessionState>());
        } else {
            ctx.SetState(std::make_unique<FinishSessionState>());
        }
    }
};

template<typename T>
struct FinishSessionState : public SessionState<T> {
    void Start(T& ctx) override {
        ctx.DoWrite("FINISH\n");
        ctx.DoRead();
    }
    void OnRead(T& ctx, const std::string& /*data*/) override { 
        ctx.Close();
    }
};
