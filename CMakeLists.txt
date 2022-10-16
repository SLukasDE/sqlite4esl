cmake_minimum_required(VERSION 3.23)

project(sqlite4esl VERSION 1.5.0
            LANGUAGES CXX)
            
#include(FetchContent)
#FetchContent_Declare(
#  catch
#  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
#  GIT_TAG        v2.13.6
#)
#FetchContent_MakeAvailable(catch)


# The compiled library code is here
#add_subdirectory(src/main)
# STATIC, SHARED, MODULE, INTERFACE
add_library(sqlite4esl STATIC
      ./src/main/sqlite4esl/Plugin.cpp
      ./src/main/sqlite4esl/database/ResultSetBinding.cpp
      ./src/main/sqlite4esl/database/ResultSetBinding.h
      ./src/main/sqlite4esl/database/ConnectionFactory.h
      ./src/main/sqlite4esl/database/PreparedStatementBinding.h
      ./src/main/sqlite4esl/database/PreparedBulkStatementBinding.cpp
      ./src/main/sqlite4esl/database/Driver.cpp
      ./src/main/sqlite4esl/database/StatementHandle.h
      ./src/main/sqlite4esl/database/PreparedBulkStatementBinding.h
      ./src/main/sqlite4esl/database/StatementHandle.cpp
      ./src/main/sqlite4esl/database/Connection.h
      ./src/main/sqlite4esl/database/Connection.cpp
      ./src/main/sqlite4esl/database/ConnectionFactory.cpp
      ./src/main/sqlite4esl/database/PreparedStatementBinding.cpp
      ./src/main/sqlite4esl/database/Driver.h
      ./src/main/sqlite4esl/Plugin.h
      ./src/main/sqlite4esl/Logger.h
)

target_include_directories(sqlite4esl PUBLIC ./src/main)
target_compile_features(sqlite4esl PUBLIC cxx_std_11)
set_property(TARGET sqlite4esl PROPERTY POSITION_INDEPENDENT_CODE ON)

find_package(esl REQUIRED)
find_package(SQLite3 REQUIRED)

target_link_libraries(sqlite4esl PRIVATE esl::esl)
target_link_libraries(sqlite4esl PRIVATE sqlite3)

install(TARGETS sqlite4esl
        EXPORT sqlite4esl-export
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

install(EXPORT      sqlite4esl-export
        FILE        sqlite4eslTargets.cmake
        NAMESPACE   sqlite4esl::
        DESTINATION lib/cmake
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/sqlite4eslConfig.cmake
        DESTINATION "lib/cmake"
)




# The compiled test code is here
#add_subdirectory(src/test)
