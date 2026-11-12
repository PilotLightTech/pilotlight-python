# gen_build.py

# Index of this file:
# [SECTION] imports
# [SECTION] project
# [SECTION] pilotlight module
# [SECTION] sandbox executable
# [SECTION] generate_scripts

#-----------------------------------------------------------------------------
# [SECTION] imports
#-----------------------------------------------------------------------------

import os
import sys
import platform as plat
import glob
import shutil

import pl_build.core as pl
import pl_build.backend_win32 as win32
import pl_build.backend_linux as linux
import pl_build.backend_macos as apple

#-----------------------------------------------------------------------------
# [SECTION] project
#-----------------------------------------------------------------------------

# where to output build scripts
working_directory = os.path.dirname(os.path.abspath(__file__)) + "/../src"

with pl.project("pilotlight_python"):

    # project wide settings
    pl.set_output_directory("../out")
    pl.add_link_directories("../out")
    pl.add_definitions("_USE_MATH_DEFINES", "PL_LANGUAGE_PYTHON")

    pl.add_include_directories(
        "../src",
        "../extensions",
        "../../pilotlight/libs",
        "../../pilotlight/extensions",
        "../../pilotlight/src",
        "../../pilotlight/shaders",
        "../dependencies/cpython/",
        "../dependencies/cpython/Include/",
        "../dependencies/cpython/build/debug/"
    )

    # platform defines (used by pilot light extensions)
    pl.add_profile(platform_filter=["Windows"], definitions=["PL_PLATFORM_WINDOWS"])
    pl.add_profile(platform_filter=["Linux"], definitions=["PL_PLATFORM_LINUX"])
    pl.add_profile(platform_filter=["Darwin"], definitions=["PL_PLATFORM_APPLE"])

    # win32 or msvc only
    pl.add_profile(compiler_filter=["msvc"],
                    target_type_filter=[pl.TargetType.DYNAMIC_LIBRARY],
                    linker_flags=["-noimplib", "-noexp"])
    pl.add_profile(compiler_filter=["msvc"],
                    linker_flags=["-incremental:no"],
                    compiler_flags=["-Zc:preprocessor", "-nologo", "-W4", "-WX", "-wd4201",
                                "-wd4100", "-wd4996", "-wd4505", "-wd4189", "-wd5105", "-wd4115", "-permissive"])

    # linux or gcc only
    pl.add_profile(compiler_filter=["gcc"],
                    configuration_filter=["debug"],
                    compiler_flags=["--debug", "-g"])

    # apple or clang only
    pl.add_profile(compiler_filter=["clang"],
                    link_directories=["/usr/local/lib"],
                    configuration_filter=["debug"],
                    compiler_flags=["--debug", "-g"])

    # gcc & clang
    pl.add_profile(compiler_filter=["gcc", "clang"],
                    target_type_filter=[pl.TargetType.DYNAMIC_LIBRARY, pl.TargetType.STATIC_LIBRARY],
                    linker_flags=["-ldl", "-lm"],
                    compiler_flags=["-fPIC"])

    # vulkan only
    pl.add_profile(compiler_filter=["gcc", "msvc"],
                    definitions=["PL_VULKAN_BACKEND"])

    # metal only
    pl.add_profile(compiler_filter=["clang"],
                    definitions=["PL_METAL_BACKEND"],
                    compiler_flags=["-arch arm64"])

    # configs
    pl.add_profile(configuration_filter=["debug"], definitions=["_DEBUG", "PL_CONFIG_DEBUG"])
    pl.add_profile(configuration_filter=["deploy"], definitions=["NDEBUG", "PL_CONFIG_RELEASE",
                                                                 "PL_GLOBAL_LOG_LEVEL=PL_LOG_LEVEL_WARN"]) # quieter for users

    #-----------------------------------------------------------------------------
    # [SECTION] pilotlight module
    #-----------------------------------------------------------------------------

    def add_unix_shader_libraries(compiler):
        # explicit archives avoid a runtime dependency on SDK shared libraries
        # (matches pilot light's gen_dev.py)
        libraries = ["shaderc_combined", "spirv-cross-c", "spirv-cross-core", "spirv-cross-cpp",
                     "spirv-cross-glsl", "spirv-cross-hlsl", "spirv-cross-msl", "spirv-cross-reflect", "spirv-cross-util"]
        if compiler == "gcc":
            pl.add_linker_flags('${VULKAN_SDK:+-L$VULKAN_SDK/lib}')
            pl.add_linker_flags(*["-l:lib" + library + ".a" for library in libraries])
        else:
            pl.add_linker_flags(*["${VULKAN_SDK:-/usr/local}/lib/lib" + library + ".a" for library in libraries])

    # Everything is linked into a single python extension module:
    #   * pilotlight.c         -> python bindings + pilot light unity extension
    #   * pl_main.c            -> core apis (pl.c) + python driven main loop
    #   * pl_platform_unit.c   -> platform extension (windows, input, files, threads)
    #   * pl_graphics_unit.c   -> graphics extension
    #   * pl_shader_unit.c     -> shader extension

    with pl.target("pilotlight", pl.TargetType.DYNAMIC_LIBRARY, reloadable=False):

        pl.add_source_files("pilotlight.c", "pl_main.c", "pl_platform_unit.c", "pl_graphics_unit.c", "pl_shader_unit.c")
        pl.set_output_binary("pilotlight")
        pl.set_output_directory("../pilotlight")

        with pl.configuration("debug"):

            # win32
            with pl.platform("Windows"):
                with pl.compiler("msvc"):
                    pl.set_output_binary("pilotlight_d")
                    pl.add_include_directories("../dependencies/cpython/PC/", "%VULKAN_SDK%\\Include")
                    pl.set_output_binary_extension(".pyd")
                    pl.add_static_link_libraries("vulkan-1")
                    # delay load the vulkan loader so the module imports without it
                    # (i.e. asset tools on machines without a vulkan driver); it is
                    # only loaded when graphics are initialized (see plStarterI.initialize)
                    pl.add_static_link_libraries("delayimp")
                    pl.add_linker_flags("-DELAYLOAD:vulkan-1.dll")
                    pl.add_static_link_libraries("user32", "Shell32", "Ole32", "gdi32", "ucrtd")
                    pl.add_static_link_libraries("shaderc_combined", "spirv-cross-c", "spirv-cross-core", "spirv-cross-cpp",
                        "spirv-cross-glsl", "spirv-cross-hlsl", "spirv-cross-msl", "spirv-cross-reflect", "spirv-cross-util")
                    pl.add_link_directories("../dependencies/cpython/PCbuild/amd64/", '%VULKAN_SDK%\\Lib')
                    pl.add_linker_flags("-nodefaultlib:LIBCMT", "-nodefaultlib:MSVCRT")
                    pl.add_compiler_flags("-std:c11", "-Od", "-MDd", "-Zi")
                    pl.set_post_target_build_step(
                        '@copy "..\\dependencies\\cpython\\PCbuild\\amd64\\python314_d.dll" "..\\out\\" >nul\n'
                        '@copy "..\\dependencies\\cpython\\PCbuild\\amd64\\python314.dll" "..\\out\\" >nul\n'
                        '@copy "..\\dependencies\\cpython\\PCbuild\\amd64\\python3.dll" "..\\out\\" >nul\n'
                        '@copy "..\\dependencies\\cpython\\PCbuild\\amd64\\python3_d.dll" "..\\out\\" >nul\n'
                        )

            # linux
            with pl.platform("Linux"):
                with pl.compiler("gcc"):
                    pl.add_compiler_flags("-std=gnu11")
                    pl.add_include_directories('$VULKAN_SDK/include', '/usr/include/vulkan')
                    pl.add_dynamic_link_libraries("xcb", "X11", "X11-xcb", "xkbcommon", "xcb-cursor", "xcb-xfixes",
                                                  "xcb-keysyms", "pthread", "vulkan")
                    add_unix_shader_libraries("gcc")
                    pl.add_linker_flags("-lstdc++", "-lm", "-lpthread") # after the archives that need them
                    pl.add_link_directories("../dependencies/cpython/build/debug/", '$VULKAN_SDK/lib')

            # mac os
            with pl.platform("Darwin"):
                with pl.compiler("clang"):
                    pl.add_compiler_flags("-std=c99", "-fmodules", "-ObjC")
                    pl.add_linker_flags("-undefined dynamic_lookup", "-lstdc++")
                    pl.set_output_binary_extension(".so")
                    pl.add_link_frameworks("Metal", "MetalKit", "Cocoa", "IOKit", "CoreVideo", "QuartzCore", "Carbon")
                    add_unix_shader_libraries("clang")
                    pl.add_include_directories('$VULKAN_SDK/include')
                    pl.add_link_directories("../dependencies/cpython/build/debug/", '$VULKAN_SDK/lib')
                    pl.add_compiler_flags("-Wno-deprecated-declarations")
                    pl.add_dynamic_link_libraries("python3.14d")

        with pl.configuration("deploy"):

            # win32
            with pl.platform("Windows"):
                with pl.compiler("msvc"):
                    pl.add_include_directories("%PL_PYTHON_INCLUDES%", "%VULKAN_SDK%\\Include")
                    pl.add_link_directories("%PL_PYTHON_LIBS%", '%VULKAN_SDK%\\Lib')
                    pl.set_output_binary_extension(".pyd")
                    pl.add_static_link_libraries("vulkan-1")
                    # delay load the vulkan loader so the module imports without it
                    # (i.e. asset tools on machines without a vulkan driver); it is
                    # only loaded when graphics are initialized (see plStarterI.initialize)
                    pl.add_static_link_libraries("delayimp")
                    pl.add_linker_flags("-DELAYLOAD:vulkan-1.dll")
                    pl.add_static_link_libraries("shaderc_combined", "spirv-cross-c", "spirv-cross-core", "spirv-cross-cpp",
                        "spirv-cross-glsl", "spirv-cross-hlsl", "spirv-cross-msl", "spirv-cross-reflect", "spirv-cross-util")
                    pl.add_static_link_libraries("user32", "Shell32", "Ole32", "gdi32", "ucrt")
                    pl.add_linker_flags("-nodefaultlib:LIBCMT")
                    pl.add_compiler_flags("-std:c11", "-O2", "-MD", "-Zi")

            # linux
            with pl.platform("Linux"):
                with pl.compiler("gcc"):
                    pl.add_compiler_flags("-std=gnu11", "-O2")
                    pl.add_include_directories("$PL_PYTHON_INCLUDES", '$VULKAN_SDK/include', '/usr/include/vulkan')
                    pl.add_dynamic_link_libraries("xcb", "X11", "X11-xcb", "xkbcommon", "xcb-cursor", "xcb-xfixes",
                                                  "xcb-keysyms", "pthread", "vulkan")
                    add_unix_shader_libraries("gcc")
                    pl.add_linker_flags("-lstdc++", "-lm", "-lpthread") # after the archives that need them
                    pl.add_link_directories('$VULKAN_SDK/lib')

            # mac os
            with pl.platform("Darwin"):
                with pl.compiler("clang"):
                    pl.add_compiler_flags("-std=c99", "-fmodules", "-ObjC", "-O2")
                    pl.add_include_directories("$PL_PYTHON_INCLUDES", '$VULKAN_SDK/include')
                    pl.add_link_directories('$VULKAN_SDK/lib')
                    pl.set_output_binary_extension(".so")
                    pl.add_linker_flags("-undefined dynamic_lookup", "-lstdc++")
                    pl.add_link_frameworks("Metal", "MetalKit", "Cocoa", "IOKit", "CoreVideo", "QuartzCore", "Carbon")
                    add_unix_shader_libraries("clang")
                    pl.add_compiler_flags("-Wno-deprecated-declarations")

    #-----------------------------------------------------------------------------
    # [SECTION] sandbox executable
    #-----------------------------------------------------------------------------

    with pl.target("pilotlight_python", pl.TargetType.EXECUTABLE, reloadable=False):

        pl.add_source_files("../sandbox/main.c")
        pl.set_output_binary("pilotlight_python")

        with pl.configuration("debug"):

            # win32
            with pl.platform("Windows"):
                with pl.compiler("msvc"):
                    pl.add_include_directories("../dependencies/cpython/PC/")
                    pl.add_static_link_libraries("user32", "Shell32", "Ole32", "gdi32", "ucrtd")
                    pl.add_link_directories("../dependencies/cpython/PCbuild/amd64/")
                    pl.add_linker_flags("-nodefaultlib:MSVCRT", "-nodefaultlib:LIBCMT")
                    pl.add_compiler_flags("-std:c11", "-Od", "-MDd", "-Zi")

            # linux
            with pl.platform("Linux"):
                with pl.compiler("gcc"):
                    pl.add_link_directories("../dependencies/cpython/build/debug/")
                    pl.add_dynamic_link_libraries("python3.14d")

            # mac os
            with pl.platform("Darwin"):
                with pl.compiler("clang"):
                    pl.add_link_directories("../dependencies/cpython/build/debug/")
                    pl.add_dynamic_link_libraries("python3.14d")

#-----------------------------------------------------------------------------
# [SECTION] generate scripts
#-----------------------------------------------------------------------------

if plat.system() == "Windows":
    win32.generate_build(working_directory + '/' + "build.bat")
elif plat.system() == "Darwin":
    apple.generate_build(working_directory + '/' + "build.sh")
elif plat.system() == "Linux":
    linux.generate_build(working_directory + '/' + "build.sh")

# pl_build's linux & macos backends always name shared libraries "lib<name>.so",
# but python can only import the extension module as "pilotlight.so"
if plat.system() in ["Darwin", "Linux"]:
    build_script = working_directory + '/' + "build.sh"
    with open(build_script, "r") as file:
        build_script_text = file.read()
    build_script_text = build_script_text.replace('/pilotlight/libpilotlight.so"', '/pilotlight/pilotlight.so"')
    with open(build_script, "w", newline="\n") as file:
        file.write(build_script_text)

# copy the shaders & resources the bindings use from pilot light into the python
# package (these ship in the wheel & are copied into new projects, so keep them
# to what is needed)
package_files = {
    "shaders": [
        # draw extension (2D/3D drawing & UI)
        "pl_draw_2d.vert",
        "pl_draw_2d.frag",
        "pl_draw_2d_sdf.frag",
        "pl_draw_3d.vert",
        "pl_draw_3d.frag",
        "pl_draw_3d_line.vert",
    ],
    "resources": [
        "Cousine-Regular.ttf", # SIL OFL 1.1 (see THIRD_PARTY_NOTICES.md)
    ],
}

for folder in ["shaders", "assets", "resources"]: # (assets are no longer shipped)
    target_folder = working_directory + "/../pilotlight/" + folder
    if os.path.isdir(target_folder):
        shutil.rmtree(target_folder)

for folder, files in package_files.items():
    target_folder = working_directory + "/../pilotlight/" + folder
    os.makedirs(target_folder)
    for file in files:
        shutil.copy2(working_directory + "/../../pilotlight/" + folder + "/" + file, target_folder)
