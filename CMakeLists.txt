cmake_minimum_required(VERSION 3.10)
project(myTrayApp)
# 查找pkg-config
find_package(PkgConfig REQUIRED)

# 配置GTK3和AppIndicator依赖
pkg_search_module(GTK3 REQUIRED gtk+-3.0)
pkg_search_module(APPINDICATOR REQUIRED ayatana-appindicator3-0.1)

# 包含tray库头文件目录
include_directories(./tray/include)

# 配置tray库源码路径（假设源文件在tray/src目录）
file(GLOB TRAY_SOURCES "./tray/src/*.c" "./tray/src/*/*.c")

# 编译tray静态库
add_library(tray STATIC ${TRAY_SOURCES})

# 链接tray库所需的系统库
target_link_libraries(tray ${GTK3_LIBRARIES} ${APPINDICATOR_LIBRARIES})


# 设置tray库的编译选项（包含路径）
target_include_directories(tray PUBLIC ${GTK3_INCLUDE_DIRS} ${APPINDICATOR_INCLUDE_DIRS} )

# 编译主程序
add_executable(${PROJECT_NAME} main.c)

# 链接主程序依赖
target_link_libraries( ${PROJECT_NAME} tray ${GTK3_LIBRARIES} ${APPINDICATOR_LIBRARIES} )


# 设置主程序的包含路径
target_include_directories(${PROJECT_NAME} PUBLIC ${GTK3_INCLUDE_DIRS} ${APPINDICATOR_INCLUDE_DIRS} )


# 添加编译选项（通过pkg-config获取的cflags）
target_compile_options(${PROJECT_NAME} PUBLIC ${GTK3_CFLAGS_OTHER} ${APPINDICATOR_CFLAGS_OTHER} )