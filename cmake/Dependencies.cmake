include_guard(GLOBAL)

include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)

#### Krog ####
CPMAddPackage("gh:taubedonner/krog#origin/next")

#### asio2 ####
find_package(Threads REQUIRED)

CPMAddPackage(
        NAME asio
        VERSION 1.34.0
        URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-34-0.zip
)

# ASIO doesn't use CMake, we have to configure it manually.
# Extra notes for using on Windows:
# 1) If _WIN32_WINNT is not set, ASIO assumes _WIN32_WINNT=0x0501, i.e. Windows XP target,
# which is definitely not the platform which most users target.
# 2) WIN32_LEAN_AND_MEAN is defined to make Winsock2 work.
if(asio_ADDED)
    add_library(asio INTERFACE)

    target_include_directories(asio
            INTERFACE ${asio_SOURCE_DIR}/asio/include
    )

    target_compile_definitions(asio
            INTERFACE
            ASIO_STANDALONE
            ASIO_NO_DEPRECATED
    )

    target_link_libraries(asio
            INTERFACE
            Threads::Threads
    )

    if(WIN32)
        # macro see @ https://stackoverflow.com/a/40217291/1746503
        macro(get_win32_winnt version)
            if (CMAKE_SYSTEM_VERSION)
                set(ver ${CMAKE_SYSTEM_VERSION})
                string(REGEX MATCH "^([0-9]+).([0-9])" ver ${ver})
                string(REGEX MATCH "^([0-9]+)" verMajor ${ver})
                if ("${verMajor}" MATCHES "10")
                    set(verMajor "A")
                    string(REGEX REPLACE "^([0-9]+)" ${verMajor} ver ${ver})
                endif ("${verMajor}" MATCHES "10")
                string(REPLACE "." "" ver ${ver})
                string(REGEX REPLACE "([0-9A-Z])" "0\\1" ver ${ver})
                set(${version} "0x${ver}")
            endif()
        endmacro()

        if(NOT DEFINED _WIN32_WINNT)
            get_win32_winnt(ver)
            set(_WIN32_WINNT ${ver})
        endif()

        message(STATUS "Set _WIN32_WINNET=${_WIN32_WINNT}")

        target_compile_definitions(asio
                INTERFACE
                _WIN32_WINNT=${_WIN32_WINNT}
                WIN32_LEAN_AND_MEAN
        )
    endif()
endif()

#### readerwriterqueue ####
#CPMAddPackage("gh:cameron314/readerwriterqueue@1.0.6")

#### ImAnim ####
#CPMAddPackage(
#        NAME ImAnim
#        GITHUB_REPOSITORY cipperly/ImAnim
#        GIT_TAG origin/main
#        DOWNLOAD_ONLY True
#)
#add_library(ImAnim_static STATIC)
#target_include_directories(ImAnim_static PUBLIC ${ImAnim_SOURCE_DIR}/src)
#target_sources(ImAnim_static PRIVATE
#        ${ImAnim_SOURCE_DIR}/src/AbstractAnimation.cpp
#        ${ImAnim_SOURCE_DIR}/src/AnimationGroup.cpp
#        ${ImAnim_SOURCE_DIR}/src/Easing.cpp
#        ${ImAnim_SOURCE_DIR}/src/EasingCurve.cpp
#        ${ImAnim_SOURCE_DIR}/src/ImVec2Anim.cpp
#        ${ImAnim_SOURCE_DIR}/src/ImVec4Anim.cpp
#        ${ImAnim_SOURCE_DIR}/src/ParallelAnimationGroup.cpp
#        ${ImAnim_SOURCE_DIR}/src/PauseAnimation.cpp
#        ${ImAnim_SOURCE_DIR}/src/SequentialAnimationGroup.cpp
#        ${ImAnim_SOURCE_DIR}/src/Utils.cpp
#)
#target_link_libraries(ImAnim_static PRIVATE Krog::Krog)
#add_library(ImAnim::ImAnim ALIAS ImAnim_static)
