cmake_minimum_required(VERSION 3.20)

project(Trivo
    VERSION 1.0.0
    DESCRIPTION "Open-source 3D File Viewer"
    HOMEPAGE_URL "https://github.com/your-org/trivo"
    LANGUAGES CXX
)

# ── C++ Standard ──────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Qt auto-tools ─────────────────────────────────────────────────────────────
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ── Output dirs ───────────────────────────────────────────────────────────────
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ── Qt6 ───────────────────────────────────────────────────────────────────────
find_package(Qt6 REQUIRED COMPONENTS
    Core Widgets OpenGL OpenGLWidgets Gui Concurrent
)
qt_standard_project_setup()

# ── Assimp ────────────────────────────────────────────────────────────────────
# Priority 1: vcpkg toolchain (Windows CI)
# Priority 2: system CMake config (Homebrew / apt)
# Priority 3: PkgConfig fallback (Linux)
# Priority 4: manual find (last resort)

find_package(assimp CONFIG QUIET)

if(NOT assimp_FOUND)
    message(STATUS "assimp CONFIG not found, trying find_package...")
    find_package(assimp QUIET)
endif()

if(NOT assimp_FOUND)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(ASSIMP_PC assimp)
        if(ASSIMP_PC_FOUND)
            add_library(assimp::assimp INTERFACE IMPORTED)
            target_include_directories(assimp::assimp INTERFACE ${ASSIMP_PC_INCLUDE_DIRS})
            target_link_libraries(assimp::assimp INTERFACE ${ASSIMP_PC_LIBRARIES})
            target_link_directories(assimp::assimp INTERFACE ${ASSIMP_PC_LIBRARY_DIRS})
            set(assimp_FOUND TRUE)
            message(STATUS "Found assimp via PkgConfig: ${ASSIMP_PC_VERSION}")
        endif()
    endif()
endif()

if(NOT assimp_FOUND)
    # 마지막 수단: 수동 탐색
    find_library(ASSIMP_LIB
        NAMES assimp assimp-vc143-mt assimp-vc142-mt assimp-vc141-mt
        REQUIRED
    )
    find_path(ASSIMP_INCLUDE_DIR assimp/scene.h REQUIRED)
    add_library(assimp::assimp INTERFACE IMPORTED)
    target_include_directories(assimp::assimp INTERFACE ${ASSIMP_INCLUDE_DIR})
    target_link_libraries(assimp::assimp INTERFACE ${ASSIMP_LIB})
    message(STATUS "Found assimp manually: ${ASSIMP_LIB}")
endif()

# ── Sources ───────────────────────────────────────────────────────────────────
set(TRIVO_SOURCES
    src/main.cpp

    src/core/MainWindow.cpp       src/core/MainWindow.h
    src/core/Viewport.cpp         src/core/Viewport.h
    src/core/Scene.cpp            src/core/Scene.h
    src/core/Camera.cpp           src/core/Camera.h
    src/core/Renderer.cpp         src/core/Renderer.h
    src/core/ModelLoader.cpp      src/core/ModelLoader.h
    src/core/LightSystem.cpp      src/core/LightSystem.h
    src/core/AnimationPlayer.cpp  src/core/AnimationPlayer.h

    src/ui/Sidebar.cpp            src/ui/Sidebar.h
    src/ui/InfoPanel.cpp          src/ui/InfoPanel.h
    src/ui/ScenePanel.cpp         src/ui/ScenePanel.h
    src/ui/ToolBar.cpp            src/ui/ToolBar.h
    src/ui/AboutDialog.cpp        src/ui/AboutDialog.h
    src/ui/DropOverlay.cpp        src/ui/DropOverlay.h

    src/i18n/I18n.cpp             src/i18n/I18n.h

    src/utils/FileUtils.cpp       src/utils/FileUtils.h
    src/utils/IconProvider.cpp    src/utils/IconProvider.h

    resources/resources.qrc
)

# ── Platform executables ──────────────────────────────────────────────────────
if(WIN32)
    list(APPEND TRIVO_SOURCES resources/trivo.rc)
    add_executable(Trivo WIN32 ${TRIVO_SOURCES})

elseif(APPLE)
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.trivo.app")
    set(MACOSX_BUNDLE_BUNDLE_NAME    "Trivo")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_ICON_FILE      "trivo.icns")
    add_executable(Trivo MACOSX_BUNDLE ${TRIVO_SOURCES})
    set_target_properties(Trivo PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/resources/Info.plist"
    )
else()
    add_executable(Trivo ${TRIVO_SOURCES})
endif()

# ── Include / Link ────────────────────────────────────────────────────────────
target_include_directories(Trivo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(Trivo PRIVATE
    Qt6::Core Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets Qt6::Gui Qt6::Concurrent
    assimp::assimp
)

# ── Compile options ───────────────────────────────────────────────────────────
if(MSVC)
    target_compile_options(Trivo PRIVATE /W3 /MP /utf-8 /wd4251)
    target_compile_definitions(Trivo PRIVATE _UNICODE UNICODE NOMINMAX)
else()
    target_compile_options(Trivo PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif()

# ── Install (for AppImage / macOS bundle) ─────────────────────────────────────
include(GNUInstallDirs)

install(TARGETS Trivo
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(UNIX AND NOT APPLE)
    install(FILES resources/trivo.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
    )
    install(FILES resources/icons/trivo_app.svg
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
        RENAME trivo.svg
    )
endif()

# ── Build summary ─────────────────────────────────────────────────────────────
message(STATUS "")
message(STATUS "┌─ Trivo ${PROJECT_VERSION} ────────────────────────────────")
message(STATUS "│  Platform   : ${CMAKE_SYSTEM_NAME}")
message(STATUS "│  Build type : ${CMAKE_BUILD_TYPE}")
message(STATUS "│  C++ std    : C++${CMAKE_CXX_STANDARD}")
message(STATUS "│  Qt6        : ${Qt6_VERSION}")
message(STATUS "│  Compiler   : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "└────────────────────────────────────────────────────────────")
message(STATUS "")
