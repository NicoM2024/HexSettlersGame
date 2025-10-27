cmake_minimum_required(VERSION 3.8)
project(HexSettlers)

set(CMAKE_CXX_STANDARD 17)

add_executable(HexSettlers
        main.cpp
        CppFiles/Board.cpp
        CppFiles/Construct.cpp
        CppFiles/Tile.cpp
        CppFiles/Path.cpp
        CppFiles/Plot.cpp
        CppFiles/Road.cpp
        CppFiles/Building.cpp
        CppFiles/OrderedPair.cpp
        CppFiles/EndpointPair.cpp
        CppFiles/DevCard.cpp
        CppFiles/Player.cpp
        CppFiles/InputFunctions.cpp
        CppFiles/GameBank.cpp
        CppFiles/Craftable.cpp)

add_executable(Tests
        Testing.cpp
        CppFiles/Board.cpp
        CppFiles/Construct.cpp
        CppFiles/Tile.cpp
        CppFiles/Path.cpp
        CppFiles/Plot.cpp
        CppFiles/Road.cpp
        CppFiles/Building.cpp
        CppFiles/OrderedPair.cpp
        CppFiles/EndpointPair.cpp
        CppFiles/DevCard.cpp
        CppFiles/Player.cpp
        CppFiles/InputFunctions.cpp
        CppFiles/GameBank.cpp
        CppFiles/Craftable.cpp)
