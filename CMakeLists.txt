cmake_minimum_required(VERSION 3.14)
project(EdaProyect)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(src)
link_directories(src)

enable_testing()

add_executable(
  RTree
  tests/RTreeTest.cpp
)
target_link_libraries(
  RTree
  gtest_main
)
add_executable(
  Point
  tests/PointTest.cpp
)
target_link_libraries(
  Point
  gtest_main
)
add_executable(
  BBox
  tests/BBoxTest.cpp
)
target_link_libraries(
  BBox
  gtest_main
)
add_executable(
  RTreeNode
  tests/RTreeNodeTest.cpp
)
target_link_libraries(
  RTreeNode
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(RTree)
gtest_discover_tests(RTreeNode)
gtest_discover_tests(Point)
gtest_discover_tests(BBox)