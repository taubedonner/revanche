set (HEADERS
        revancheapp.h
        vanch/command.h
        vanch/commandregistry.h
        vanch/udpclient.h
        vanch/udpscanner.h
)

set (SOURCES
        revancheapp.cc
        vanch/command.cc
        vanch/commandregistry.cc
        vanch/udpclient.cc
        vanch/udpscanner.cc
)

add_executable(revanche WIN32)

target_include_directories(revanche PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(revanche PRIVATE ${HEADERS} ${SOURCES})

target_link_libraries(revanche PRIVATE Krog::Krog nlohmann_json::nlohmann_json readerwriterqueue asio)

target_precompile_headers(revanche PRIVATE pch.hh)
