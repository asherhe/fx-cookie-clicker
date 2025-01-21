# Configure with [fxsdk build-fx] or [fxsdk build-cg], which provide the
# toolchain file and module path of the fxSDK

cmake_minimum_required(VERSION 3.15)
project(Cookie VERSION 0.0.2)

include(GenerateG1A)
include(Fxconv)
find_package(Gint 2.9 REQUIRED)

fxconv_declare_converters(assets-fx/converters.py)

set(SOURCES
  src/main.cpp
  src/game.h
  src/game.cpp
  src/building.h
  src/building.cpp
  src/util.h
  src/util.cpp
  src/list.h
  src/list.cpp
  src/rarray.h
  src/rarray.cpp
  src/files.h
  src/files.cpp
)

set(ASSETS
  assets-fx/big_cookie.png
  assets-fx/cookie.png
  assets-fx/ui_base.png
  assets-fx/title.png
  assets-fx/info_bar.png
  assets-fx/tabs.png
  assets-fx/font3x5.png
  assets-fx/building_data.csv
  assets-fx/upg_data.csv
  assets-fx/descs.txt
)

fxconv_declare_assets(${ASSETS} WITH_METADATA)

add_executable(cookie ${SOURCES} ${ASSETS})
target_compile_options(cookie PRIVATE -Wall -Wextra -Os -flto -DCOOKIE_VERSION="${PROJECT_VERSION}")
target_link_libraries(cookie Gint::Gint)

generate_g1a(
  TARGET cookie
  OUTPUT "Cookie.g1a"
  NAME "Cookie"
  ICON assets-fx/icon.png
  VERSION ${PROJECT_VERSION}
)
