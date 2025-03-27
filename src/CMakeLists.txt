set (HEADERS
        revancheapp.h
        vanch/message.h
        vanch/messageregistry.h
        vanch/udpclient.h
)

set (SOURCES
        revancheapp.cc
        vanch/message.cc
        vanch/messageregistry.cc
        vanch/udpclient.cc
)

add_executable(revanche WIN32)

target_include_directories(revanche PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(revanche PRIVATE ${HEADERS} ${SOURCES})

target_link_libraries(revanche PRIVATE Krog::Krog nlohmann_json::nlohmann_json readerwriterqueue asio)

target_precompile_headers(revanche PRIVATE pch.hh)
