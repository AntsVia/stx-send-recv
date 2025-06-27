add_library(SocketLib STATIC
    Socket.cpp
)

target_include_directories(SocketLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

find_package(Boost REQUIRED COMPONENTS system)
if(Boost_SYSTEM_LIBRARY)
target_link_libraries(SocketLib PUBLIC
    Boost::system
) 
endif()

