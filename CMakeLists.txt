if (MSVC)
    # /utf-8 让编译器将源代码和控制台输出都设置为 UTF-8
    add_compile_options(/utf-8)
endif()


#标题
cmake_minimum_required(VERSION 3.10.0)#最低版本
project(SpaceShooting VERSION 0.1.0 LANGUAGES C CXX)#项目名称及版本

#设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#设置编译目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

set(TARGET ${project_NAME}_${CMAKE_SYSTEM_NAME})

#预处理
set(SDL2_DIR "D:/codelearng/stool package/sdl2/cmake")#SDL2库路径
find_package(SDL2 REQUIRED)#查找SDL2库
    #扩展库
    find_package(SDL2_image  REQUIRED) #查找SDL2_image库
    find_package(SDL2_mixer REQUIRED)#查找SDL2_mixer库
    find_package(SDL2_ttf REQUIRED)#查找SDL2_ttf库

#编译
add_executable(${TARGET}
                     "src/main.cpp"
                     "src/game.cpp"
                     "src/SceneMain.cpp"
                     "src/SceneTitle.cpp"
                     "src/SceneEnd.cpp")#引入可执行文件

# 设置编译选项
if (MSVC)
    # MSVC 设置
    target_compile_options(${TARGET} PRIVATE /utf-8)#强制utf-8    
    string(REPLACE "/W3" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")#移除默认警告等级
    string(APPEND CMAKE_CXX_FLAGS " /W4")#设置警告等级
else()
    # ... GCC/Clang 设置
endif()

#链接
target_link_libraries(${TARGET}
                         ${SDL2_LIBRARIES}#链接SDL2库
                         SDL2_image::SDL2_image#链接SDL2_image库
                         SDL2_mixer::SDL2_mixer#链接SDL2_mixer库
                         SDL2_ttf::SDL2_ttf#链接SDL2_ttf库
                         )#链接具体库                                              


#头文件
target_include_directories(${TARGET} PRIVATE 
    "D:/codelearng/stool package/sdl2/include/SDL2"
    ${SDL2_INCLUDE_DIRS}
)