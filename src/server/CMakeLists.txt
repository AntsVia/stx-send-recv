add_executable(stx-server
    Server.cpp
    main.cpp
)

find_package(Boost REQUIRED COMPONENTS system filesystem)

target_include_directories(stx-server PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(stx-server PRIVATE Boost::system Boost::filesystem)
