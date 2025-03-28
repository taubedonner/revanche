file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.h" "*.cc")

add_executable(revanche)

target_include_directories(revanche PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(revanche PRIVATE ${SOURCES})

target_link_libraries(revanche PRIVATE Krog::Krog frozen::frozen nlohmann_json::nlohmann_json readerwriterqueue asio)

target_compile_definitions(revanche PUBLIC NOMINMAX)

target_precompile_headers(revanche PRIVATE pch.hh)
