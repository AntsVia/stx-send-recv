#pragma once

#include "MessageHandlerI.hpp"
#include <string>
#include <iostream>

template<typename Context>
struct InitSessionState : public SessionState<Context> {
   void Start(Context& ctx) override;
   void OnRead(Context& ctx, const std::string& data) override;
   void OnWrite(Context& ctx) override;
};

template<typename Context>
struct AuthSessionState : public SessionState<Context> {
   void Start(Context& ctx) override;
   void OnRead(Context& ctx, const std::string& data) override;
   void OnWrite(Context& ctx) override;
};

template<typename Context>
struct KeyExchangeSessionState : public SessionState<Context> {
   void Start(Context& ctx) override;
   void OnRead(Context& ctx, const std::string& data) override;
   void OnWrite(Context& ctx) override;
};

template<typename Context>
struct DataSendSessionState : public SessionState<Context> {
   void Start(Context& ctx) override;
   void OnRead(Context& ctx, const std::string& data) override;
   void OnWrite(Context& ctx) override;
};

template<typename Context>
struct ReconnectSessionState : public SessionState<Context> {
   void Start(Context& ctx) override;
   void OnRead(Context& ctx, const std::string& data) override;
   void OnWrite(Context& ctx) override;
};

template<typename Context>
struct FinishSessionState : public SessionState<Context> {
   void Start(Context& ctx) override;
   void OnRead(Context& ctx, const std::string& data) override;
   void OnWrite(Context& ctx) override;
};

template <typename Context>
void InitSessionState<Context>::Start(Context& ctx)  {
   ctx.DoWrite("HELLO");
}

template <typename Context>
void InitSessionState<Context>::OnRead(Context& ctx, const std::string& data)  {
   if (data.find("AUTH_REQUEST") != std::string::npos) {
      ctx.SetState(tools::make_unique<AuthSessionState<Context>>());
   } else {
      ctx.SetState(tools::make_unique<FinishSessionState<Context>>());
   }
}

template <typename Context>
void InitSessionState<Context>::OnWrite(Context& ctx){
   ctx.DoRead();
}

template<typename Context>
void AuthSessionState<Context>::Start(Context& ctx)  {
   ctx.DoWrite("AUTH");
}

template<typename Context>
void AuthSessionState<Context>::OnRead(Context& ctx, const std::string& data)  { 
   if (data.find("AUTH_ACK") != std::string::npos) {
      ctx.SetState(tools::make_unique<KeyExchangeSessionState<Context>>());
   } else {
      ctx.SetState(tools::make_unique<FinishSessionState<Context>>());
   }
}

template <typename Context>
void AuthSessionState<Context>::OnWrite(Context& ctx){
   ctx.DoRead();
}

template<typename Context>
void KeyExchangeSessionState<Context>::Start(Context& ctx)  {
   ctx.DoWrite("KEY_EXCHANGE");
}

template<typename Context>
void KeyExchangeSessionState<Context>::OnRead(Context& ctx, const std::string& data)  { 
   if (data.find("KEY_EXCHANGE") != std::string::npos) {
      ctx.SetState(tools::make_unique<DataSendSessionState<Context>>());
   } else {
      ctx.SetState(tools::make_unique<FinishSessionState<Context>>());
   }
}

template <typename Context>
void KeyExchangeSessionState<Context>::OnWrite(Context& ctx){
   ctx.DoRead();
}

template<typename Context>
void DataSendSessionState<Context>::Start(Context& ctx)  {
   ctx.DoWrite("DATA");
}

template<typename Context>
void DataSendSessionState<Context>::OnRead(Context& ctx, const std::string& data)  { 
   if (data.find("DATA") != std::string::npos) {
      ctx.SetState(tools::make_unique<FinishSessionState<Context>>());
      // ctx.SetState(tools::make_unique<DataSendSessionState<Context>>());
   } else if (data.find("FAILED") != std::string::npos) {
      ctx.SetState(tools::make_unique<ReconnectSessionState<Context>>());
   } else {
      ctx.SetState(tools::make_unique<FinishSessionState<Context>>());
   }
}

template <typename Context>
void DataSendSessionState<Context>::OnWrite(Context& ctx){
   ctx.DoRead();
}

template<typename Context>
void ReconnectSessionState<Context>::Start(Context& ctx)  {
   ctx.DoWrite("RECONNECT");
}

template<typename Context>
void ReconnectSessionState<Context>::OnRead(Context& ctx, const std::string& data)  { 
   if (data.find("AUTH") != std::string::npos) {
      ctx.SetState(tools::make_unique<AuthSessionState<Context>>());
   } else {
      ctx.SetState(tools::make_unique<FinishSessionState<Context>>());
   }
}

template <typename Context>
void ReconnectSessionState<Context>::OnWrite(Context& ctx){
   ctx.DoRead();
}

template<typename Context>
void FinishSessionState<Context>::Start(Context& ctx)  {
   ctx.DoWrite("FINISH");
}

template<typename Context>
void FinishSessionState<Context>::OnRead(Context& ctx, const std::string& /*data*/)  {
   ctx.Close();
}

template <typename Context>
void FinishSessionState<Context>::OnWrite(Context& ctx){
   ctx.Close();
}
