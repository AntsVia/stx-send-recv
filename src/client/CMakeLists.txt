add_executable(stx-client
    Client.cpp
    main.cpp
)

find_package(Boost REQUIRED COMPONENTS system filesystem)

target_include_directories(stx-client PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(stx-client PRIVATE
    Boost::system
    Boost::filesystem
    MessageHandler
)
