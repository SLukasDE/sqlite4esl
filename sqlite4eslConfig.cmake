include(CMakeFindDependencyMacro)

find_dependency(esa)
find_dependency(esl)
find_dependency(SQLite3)

include("${CMAKE_CURRENT_LIST_DIR}/sqlite4eslTargets.cmake")
