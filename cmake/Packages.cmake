include(FetchContent)
#include(FindPkgConfig)

if(ESL_DEPENDENCIES_USE_CONAN)
    message(STATUS "Using Conan")
    include(${CMAKE_BINARY_DIR}/conan/conan_toolchain.cmake)
endif()

if(ESL_DEPENDENCIES_USE_VCPKG)
    message(STATUS "Using VCPKG")
    if(WIN32)
        set(USER_HOME_DIRECTORY $ENV{USERPROFILE})
    else()
        set(USER_HOME_DIRECTORY $ENV{HOME})
    endif()
    message(STATUS "User Home Directory: ${USER_HOME_DIRECTORY}")
    include(${USER_HOME_DIRECTORY}/opt/vcpkg/scripts/buildsystems/vcpkg.cmake)
endif()

if(ESL_DEPENDENCIES_USE_PKGCONFIG)
    find_package(PkgConfig QUIET)
endif()

function(find_custom_package NAME URL TAG)
    # Default, try 'find_package'. VCPKG or Conan may be used, if enabled
    if(NOT ${NAME}_FOUND)
        message(STATUS "Try to find ${NAME} by find_package")
        find_package(${NAME} QUIET)
        if(${NAME}_FOUND)
            message(STATUS "${NAME} has been found by using find_package")
        endif()
    endif()

    if(NOT ${NAME}_FOUND)
        message(STATUS "Try to find ${NAME} by FetchContent")
        set(COMPILE_UNITTESTS OFF)
        FetchContent_Declare(
            ${NAME}
            GIT_REPOSITORY ${URL}
            GIT_TAG ${TAG}
            GIT_SHALLOW TRUE
            OVERRIDE_FIND_PACKAGE # 'find_package(...)' will call 'FetchContent_MakeAvailable(...)'
        )
        find_package(${NAME} QUIET)
        if(${NAME}_FOUND)
            message(STATUS "${NAME} has been found by using FetchContent")
        endif()
    endif()

    if(NOT ${NAME}_FOUND)
        message(FATAL_ERROR "${NAME} NOT found")
    endif()
endfunction()

function(find_package_esa)
    find_custom_package(esa https://github.com/SLukasDE/esa master)
endfunction()

function(find_package_esl)
    find_custom_package(esl https://github.com/SLukasDE/esl master)
endfunction()


function(find_package_SQLite3) # SQLite::SQLite3
    # Default, try 'find_package'. VCPKG or Conan may be used, if enabled
    if(NOT SQLite3_FOUND)
        message(STATUS "Try to find SQLite3 by find_package")
        find_package(SQLite3 QUIET)
        if(SQLite3_FOUND)
            message(STATUS "SQLite3 has been found by using find_package")
        endif()
    endif()
	
    
    if(NOT SQLite3_FOUND)
        message(STATUS "Try to find SQLite3 by FetchContent")
        FetchContent_Declare(
            SQLite3
            SOURCE_DIR "${CMAKE_SOURCE_DIR}/cmake/sqlite3-cmake"
            OVERRIDE_FIND_PACKAGE # 'find_package(...)' will call 'FetchContent_MakeAvailable(...)'
        )
        find_package(SQLite3 QUIET)
        if(SQLite3_FOUND)
            message(STATUS "SQLite3 has been found by using FetchContent")
        endif()
    endif()

    if(NOT SQLite3_FOUND)
        message(FATAL_ERROR "SQLite3 NOT found")
    endif()
endfunction()
