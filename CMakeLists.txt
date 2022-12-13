cmake_minimum_required(VERSION 3.0.0)
project(VinNES VERSION 0.1.0)

set(VINNES_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SDL2_SRC_DIR ${VINNES_SRC_DIR}/dependencies/SDL2)
set(VINNES_MAIN_FILES ${VINNES_SRC_DIR}/Main.cpp)
set(VINNES_SRC_FILES 
    ${VINNES_SRC_DIR}/VinNES.cpp
    ${VINNES_SRC_DIR}/IOController/IOController.cpp
    ${VINNES_SRC_DIR}/CPUInterpreter/CPUInterpreter.cpp
)


add_executable(VinNES ${VINNES_MAIN_FILES} ${VINNES_SRC_FILES})
set_property(TARGET VinNES PROPERTY CXX_STANDARD 20)
target_include_directories(VinNES 
    PUBLIC
    ${VINNES_SRC_DIR}
    ${VINNES_SRC_DIR}/IOControler
    ${VINNES_SRC_DIR}/CPUInterpreter
    ${VINNES_SRC_DIR}/Utils
    ${SDL2_SRC_DIR}/include
)

file(COPY ${SDL2_SRC_DIR}/bin/SDL2.dll DESTINATION ${CMAKE_BINARY_DIR})

target_link_directories(VinNES
    PUBLIC
    ${SDL2_SRC_DIR}/lib
    )
    
set(VINNES_REQUIRED_LINK_LIBS SDL2main SDL2)

target_link_libraries(VinNES ${VINNES_REQUIRED_LINK_LIBS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
