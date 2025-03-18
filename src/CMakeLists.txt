set (HEADERS
        revancheapp.h)

set (SOURCES
        revancheapp.cc)

add_executable(revanche WIN32)

target_include_directories(revanche PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(revanche PRIVATE ${HEADERS} ${SOURCES})

target_link_libraries(revanche PRIVATE Krog::Krog asio)
