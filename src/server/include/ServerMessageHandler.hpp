#pragma once

#include "MessageHandlerI.hpp"
#include <string>
#include <iostream>

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
struct AuthAckSessionState : public SessionState<Context> {
   void Start(Context& ctx) override;
   void OnRead(Context& ctx, const std::string& data) override;
};

template<typename Context>
struct KeyExchangeSessionState : public SessionState<Context> {
   void Start(Context& ctx) override;
   void OnRead(Context& ctx, const std::string& data) override;
};

template<typename Context>
struct DataReceiveSessionState : public SessionState<Context> {
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
   ctx.DoRead();
}

template <typename Context>
void InitSessionState<Context>::OnRead(Context& ctx, const std::string& data)  {
   std::cout << __LINE__ << " OnRead: " << data << "";
   if (data.find("HELLO") != std::string::npos) {
      ctx.SetState(tools::make_unique<AuthSessionState<Context>>());
   } else {
      ctx.SetState(tools::make_unique<FinishSessionState<Context>>());
   }
}

template<typename Context>
void AuthSessionState<Context>::Start(Context& ctx)  {
   ctx.DoWrite("AUTH_REQUEST");
}

template<typename Context>
void AuthSessionState<Context>::OnRead(Context& ctx, const std::string& data)  { 
   std::cout << __LINE__ << " OnRead: " << data << "";
   if (data.find("AUTH") != std::string::npos) {
      ctx.SetState(tools::make_unique<AuthAckSessionState<Context>>());
   } else {
      ctx.SetState(tools::make_unique<FinishSessionState<Context>>());
   }
}

template<typename Context>
void AuthAckSessionState<Context>::Start(Context& ctx)  {
   ctx.DoWrite("AUTH_ACK");
}

template<typename Context>
void AuthAckSessionState<Context>::OnRead(Context& ctx, const std::string& data)  { 
   std::cout << __LINE__ << " OnRead: " << data << "";
   if (data.find("KEY_EXCHANGE") != std::string::npos) {
      ctx.SetState(tools::make_unique<KeyExchangeSessionState<Context>>());
   } else {
      ctx.SetState(tools::make_unique<FinishSessionState<Context>>());
   }
}

template<typename Context>
void KeyExchangeSessionState<Context>::Start(Context& ctx)  {
   ctx.DoWrite("KEY_EXCHANGE");
}

template<typename Context>
void KeyExchangeSessionState<Context>::OnRead(Context& ctx, const std::string& data)  {
   std::cout << __LINE__ << " OnRead: " << data << "";
   if (data.find("DATA") != std::string::npos) {
      ctx.SetState(tools::make_unique<DataReceiveSessionState<Context>>());
   } else {
      ctx.SetState(tools::make_unique<FinishSessionState<Context>>());
   }
}

template<typename Context>
void DataReceiveSessionState<Context>::Start(Context& ctx)  {
   ctx.DoWrite("DATA");
}

template<typename Context>
void DataReceiveSessionState<Context>::OnRead(Context& ctx, const std::string& data)  {
   std::cout << __LINE__ << " OnRead: " << data << "";
   if (data.find("DATA") != std::string::npos) {
      ctx.SetState(tools::make_unique<DataReceiveSessionState<Context>>());
   // } else if (/*failed process data */) {
   //    ctx.SetState(tools::make_unique<ReconnectSessionState<Context>>());
   } else {
      ctx.SetState(tools::make_unique<FinishSessionState<Context>>());
   }
}

template<typename Context>
void FinishSessionState<Context>::Start(Context& ctx)  {
   ctx.DoRead();
}

template<typename Context>
void FinishSessionState<Context>::OnRead(Context& ctx, const std::string& data)  { 
   std::cout << __LINE__ << " OnRead: " << data << "";
   ctx.Close();
}
