cmake_minimum_required(VERSION 3.16)
project(finance_tracker_gui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

add_executable(finance_tracker_gui main.cpp)
target_link_libraries(finance_tracker_gui PRIVATE Qt6::Core Qt6::Widgets)
