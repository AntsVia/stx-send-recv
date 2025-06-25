add_executable(stx-server
    Server.cpp
    Session.cpp
    main.cpp
)

target_include_directories(stx-server PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(Threads REQUIRED)

target_link_libraries(stx-server PRIVATE
    Boost::system
    Boost::filesystem
    Threads::Threads
    MessageHandler
)
