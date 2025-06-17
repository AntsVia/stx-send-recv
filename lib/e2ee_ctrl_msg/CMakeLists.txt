add_library(MessageHandler INTERFACE)

target_sources(MessageHandler INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/interface/MessageHandlerI.hpp
)

target_include_directories(MessageHandler INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/interface
)
