#pragma once

#include "MessageHandlerI.hpp"
#include <string>

template<typename Context>
struct InitSessionState : public SessionState<Context> {
   void Start(Context& ctx) override;
   void OnRead(Context& ctx, const std::string& data) override;
};
template<typename Context>
struct AuthSessionState : public SessionState<Context> {
   void Start(Context& ctx) override;
   void OnRead(Context& ctx, const std::string& data) override;
};
template<typename Context>
struct KeyExchangeSessionState : public SessionState<Context> {
   void Start(Context& ctx) override;
   void OnRead(Context& ctx, const std::string& data) override;
};
template<typename Context>
struct DataSendSessionState : public SessionState<Context> {
   void Start(Context& ctx) override;
   void OnRead(Context& ctx, const std::string& data) override;
};
template<typename Context>
struct ReconnectSessionState : public SessionState<Context> {
   void Start(Context& ctx) override;
   void OnRead(Context& ctx, const std::string& data) override;
};
template<typename Context>
struct FinishSessionState : public SessionState<Context> {
   void Start(Context& ctx) override;
   void OnRead(Context& ctx, const std::string& data) override;
};

template <typename Context>
void InitSessionState<Context>::Start(Context& ctx)  {
       ctx.DoWrite("HELLO\n");
    ctx.DoRead();
}

template <typename Context>
void InitSessionState<Context>::OnRead(Context& ctx, const std::string& data)  {
       if (data.find("AUTH") == std::string::npos) {
        ctx.SetState(tools::make_unique<AuthSessionState<Context>>());
    } else {
        ctx.SetState(tools::make_unique<FinishSessionState<Context>>());
    }
}

template<typename Context>
void AuthSessionState<Context>::Start(Context& ctx)  {
       ctx.DoWrite("AUTH\n");
    ctx.DoRead();
}

template<typename Context>
void AuthSessionState<Context>::OnRead(Context& ctx, const std::string& data)  { 
       if (data.find("AUTH_ACK\n") == std::string::npos) {
        ctx.SetState(tools::make_unique<KeyExchangeSessionState<Context>>());
    } else {
        ctx.SetState(tools::make_unique<FinishSessionState<Context>>());
    }
}

template<typename Context>
void KeyExchangeSessionState<Context>::Start(Context& ctx)  {
       ctx.DoWrite("KEY_EXCHANGE\n");
    ctx.DoRead();
}

template<typename Context>
void KeyExchangeSessionState<Context>::OnRead(Context& ctx, const std::string& data)  { 
       if (data.find("KEY_EXCHANGE\n") == std::string::npos) {
        ctx.SetState(tools::make_unique<DataSendSessionState<Context>>());
        ctx.DoRead();
    } else {
        ctx.SetState(tools::make_unique<FinishSessionState<Context>>());
    }
}

template<typename Context>
void DataSendSessionState<Context>::Start(Context& ctx)  {
       ctx.DoWrite("DATA\n");
    ctx.SetState(tools::make_unique<DataSendSessionState<Context>>());
}

template<typename Context>
void DataSendSessionState<Context>::OnRead(Context& ctx, const std::string& data)  { 
       if (data.find("FAILED\n") == std::string::npos) {
        ctx.SetState(tools::make_unique<ReconnectSessionState<Context>>());
    } else {
        ctx.SetState(tools::make_unique<FinishSessionState<Context>>());
    }
}

template<typename Context>
void ReconnectSessionState<Context>::Start(Context& ctx)  {
       ctx.DoWrite("RECONNECT\n");
    ctx.DoRead();
}

template<typename Context>
void ReconnectSessionState<Context>::OnRead(Context& ctx, const std::string& data)  { 
       if (data.find("AUTH\n") == std::string::npos) {
        ctx.SetState(tools::make_unique<AuthSessionState<Context>>());
    } else {
        ctx.SetState(tools::make_unique<FinishSessionState<Context>>());
    }
}

template<typename Context>
void FinishSessionState<Context>::Start(Context& ctx)  {
       ctx.DoWrite("FINISH\n");
    ctx.DoRead();
}

template<typename Context>
void FinishSessionState<Context>::OnRead(Context& ctx, const std::string& /*data*/)  { 
       ctx.Close();
}
