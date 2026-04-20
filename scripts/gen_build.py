# gen_examples.py

# Index of this file:
# [SECTION] imports
# [SECTION] project
# [SECTION] examples
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
    pl.add_definitions("_USE_MATH_DEFINES")

    pl.add_include_directories(
        "../src",
        "../extensions",
        "../../pilotlight/libs",
        "../../pilotlight/extensions",
        "../../pilotlight/src",
        "../../pilotlight/shaders",
        "../../pilotlight/dependencies/stb",
        "../../pilotlight/dependencies/glfw/include/",
        "../../pilotlight/dependencies/imgui/",
        "../../pilotlight/dependencies/cgltf",
        "../dependencies/cpython/",
        "../dependencies/cpython/Include/",
        "../dependencies/cpython/build/debug/"
    )

    # win32 or msvc only
    pl.add_profile(compiler_filter=["msvc"],
                    target_type_filter=[pl.TargetType.DYNAMIC_LIBRARY],
                    linker_flags=["-noimplib", "-noexp"])
    pl.add_profile(compiler_filter=["msvc"],
                    linker_flags=["-incremental:no"],
                    compiler_flags=["-Zc:preprocessor", "-nologo", "-W4", "-WX", "-wd4201",
                                "-wd4100", "-wd4996", "-wd4505", "-wd4189", "-wd5105", "-wd4115", "-permissive-"])

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
    
    #-----------------------------------------------------------------------------
    # [SECTION] glfw
    #-----------------------------------------------------------------------------

    with pl.target("glfw", pl.TargetType.STATIC_LIBRARY, False, False):

        pl.add_source_files("../../pilotlight/dependencies/glfw/src/glfw_unity.c")
        pl.add_source_files("../../pilotlight/dependencies/glfw/src/null_window.c")

        with pl.configuration("debug"):

            pl.set_output_binary("glfwd")

            # win32
            with pl.platform("Windows"):
                with pl.compiler("msvc"):
                    pl.add_include_directories("%VULKAN_SDK%\\Include")
                    pl.add_definitions("UNICODE", "_UNICODE", "_CRT_SECURE_NO_WARNINGS", "_GLFW_VULKAN_STATIC", "_GLFW_WIN32", "_DEBUG")
                    pl.add_compiler_flags("-std:c11", "-Od", "-MDd", "-Zi", "-permissive", "-wd4244")
            
            # linux
            with pl.platform("Linux"):
                with pl.compiler("gcc"):
                    pl.add_definitions("_GLFW_VULKAN_STATIC", "_GLFW_X11")
                    pl.add_include_directories('$VULKAN_SDK/include', '/usr/include/vulkan', '/usr/include/vulkan')
                    pl.add_dynamic_link_libraries("xcb", "X11", "X11-xcb", "xkbcommon", "pthread", "xcb-cursor", "vulkan")
                    pl.add_link_directories('$VULKAN_SDK/lib')
                    pl.add_compiler_flags("-std=gnu99")
                    pl.add_source_files("../../pilotlight/dependencies/glfw/src/posix_poll.c")

            # apple
            with pl.platform("Darwin"):
                with pl.compiler("clang"):
                    pl.add_definitions("_GLFW_VULKAN_STATIC", "_GLFW_COCOA")
                    pl.add_compiler_flags("-Wno-deprecated-declarations", "-std=c99", "-fmodules", "-ObjC")
                    pl.add_link_frameworks("Cocoa", "IOKit", "CoreFoundation")

        with pl.configuration("deploy"):

            pl.set_output_binary("glfw")

            # win32
            with pl.platform("Windows"):
                with pl.compiler("msvc"):
                    pl.add_include_directories("%VULKAN_SDK%\\Include")
                    pl.add_definitions("UNICODE", "_UNICODE", "_CRT_SECURE_NO_WARNINGS", "_GLFW_VULKAN_STATIC", "_GLFW_WIN32")
                    pl.add_compiler_flags("-std:c11", "-O2", "-MD", "-Zi", "-permissive", "-wd4244")

            # linux
            with pl.platform("Linux"):
                with pl.compiler("gcc"):
                    pl.add_definitions("_GLFW_VULKAN_STATIC", "_GLFW_X11")
                    pl.add_include_directories('$VULKAN_SDK/include', '/usr/include/vulkan', '/usr/include/vulkan')
                    pl.add_dynamic_link_libraries("xcb", "X11", "X11-xcb", "xkbcommon", "pthread", "xcb-cursor", "vulkan")
                    pl.add_link_directories('$VULKAN_SDK/lib')
                    pl.add_compiler_flags("-std=gnu99")
                    pl.add_source_files("../../pilotlight/dependencies/glfw/src/posix_poll.c")

            # apple
            with pl.platform("Darwin"):
                with pl.compiler("clang"):
                    pl.add_definitions("_GLFW_VULKAN_STATIC", "_GLFW_COCOA")
                    pl.add_compiler_flags("-std=c99", "-fmodules", "-ObjC", "-Wno-deprecated-declarations")
                    pl.add_link_frameworks("Cocoa", "IOKit", "CoreFoundation")

    #-----------------------------------------------------------------------------
    # [SECTION] imgui & implot
    #-----------------------------------------------------------------------------

    with pl.target("imgui", pl.TargetType.STATIC_LIBRARY, False, False):

        # imgui & imgui
        pl.add_source_files("../../pilotlight/dependencies/imgui/imgui_unity.cpp")

        # default config
        with pl.configuration("debug"):

            pl.set_output_binary("dearimguid")
            pl.add_static_link_libraries("glfwd")

            # win32
            with pl.platform("Windows"):
                with pl.compiler("msvc"):
                    pl.add_compiler_flags("-std:c++14", "-Od", "-MDd", "-Zi", "-permissive")

            # linux
            with pl.platform("Linux"):
                with pl.compiler("gcc"):
                    pl.add_compiler_flags("-std=c++14")
                    pl.add_linker_flags("-lstdc++")

            # macos
            with pl.platform("Darwin"):
                with pl.compiler("clang"):
                    pl.add_compiler_flags("-std=c++14", "-Warc-bridge-casts-disallowed-in-nonarc")
                    pl.add_linker_flags("-lstdc++")

        with pl.configuration("deploy"):

            pl.set_output_binary("dearimgui")
            pl.add_static_link_libraries("glfw")

            # win32
            with pl.platform("Windows"):
                with pl.compiler("msvc"):
                    pl.add_compiler_flags("-std:c++14", "-O2", "-MD", "-Zi", "-permissive")

            # linux
            with pl.platform("Linux"):
                with pl.compiler("gcc"):
                    pl.add_compiler_flags("-std=c++14")
                    pl.add_linker_flags("-lstdc++")

            # macos
            with pl.platform("Darwin"):
                with pl.compiler("clang"):
                    pl.add_compiler_flags("-std=c++14", "-Warc-bridge-casts-disallowed-in-nonarc")
                    pl.add_linker_flags("-lstdc++")

    #-----------------------------------------------------------------------------
    # [SECTION] pl_platform_ext
    #-----------------------------------------------------------------------------

    with pl.target("pl_platform_ext", pl.TargetType.STATIC_LIBRARY, False, False):
            
        pl.set_output_binary("pl_platform_ext")

        with pl.configuration("debug"):

            # win32
            with pl.platform("Windows"):

                pl.add_include_directories("../dependencies/cpython/PC/")

                with pl.compiler("msvc"):
                    pl.add_source_files("../../pilotlight/extensions/pl_platform_win32_ext.c")
                    pl.add_static_link_libraries("ucrtd", "user32", "Ole32")
                    pl.add_compiler_flags("-std:c11")

            # linux
            with pl.platform("Linux"):
                with pl.compiler("gcc"):
                    pl.add_source_files("../../pilotlight/extensions/pl_platform_linux_ext.c")
                    pl.add_dynamic_link_libraries("pthread")

            # mac os
            with pl.platform("Darwin"):
                with pl.compiler("clang"):
                    pl.add_source_files("../../pilotlight/extensions/pl_platform_macos_ext.m")
                    pl.add_linker_flags("-undefined dynamic_lookup")

        with pl.configuration("deploy"):
            
            # win32
            with pl.platform("Windows"):
                with pl.compiler("msvc"):
                    pl.add_source_files("../../pilotlight/extensions/pl_platform_win32_ext.c")
                    pl.add_static_link_libraries("ucrt", "user32", "Ole32")
                    pl.add_compiler_flags("-std:c11")

            # linux
            with pl.platform("Linux"):
                with pl.compiler("gcc"):
                    pl.add_source_files("../../pilotlight/extensions/pl_platform_linux_ext.c")
                    pl.add_dynamic_link_libraries("pthread")

            # mac os
            with pl.platform("Darwin"):
                with pl.compiler("clang"):
                    pl.add_source_files("../../pilotlight/extensions/pl_platform_macos_ext.m")
                    pl.add_linker_flags("-undefined dynamic_lookup")
                    pl.add_compiler_flags("-Warc-bridge-casts-disallowed-in-nonarc", "-Wnullability-completeness")

    #-----------------------------------------------------------------------------
    # [SECTION] backend main
    #-----------------------------------------------------------------------------
    
    with pl.target("pilotlight_main", pl.TargetType.STATIC_LIBRARY, False):

        pl.add_source_files("pl_main.cpp")
        
        with pl.configuration("debug"):

            pl.add_static_link_libraries("dearimguid", "glfwd", "pl_platform_ext")
            pl.set_output_binary("pilotlight_main")
            
            # win32
            with pl.platform("Windows"):
                with pl.compiler("msvc"):
                    pl.add_include_directories("%VULKAN_SDK%\\Include")
                    pl.add_static_link_libraries("user32", "Shell32", "Ole32", "gdi32", "ucrtd")
                    pl.add_link_directories("../dependencies/cpython/PCbuild/amd64/", '%VULKAN_SDK%\\Lib')
                    pl.add_linker_flags("-nodefaultlib:LIBCMT", "-nodefaultlib:MSVCRT")
                    pl.add_compiler_flags("-std:c11", "-Od", "-MDd", "-Zi")

            # linux
            with pl.platform("Linux"):
                with pl.compiler("gcc"):
                    pl.add_link_directories("../dependencies/cpython/build/debug/")

            # mac os
            with pl.platform("Darwin"):
                with pl.compiler("clang"):
                    pl.add_compiler_flags("-fmodules", "-ObjC++")
                    pl.add_linker_flags("-undefined dynamic_lookup")
                    pl.add_link_frameworks("Metal", "MetalKit", "Cocoa", "IOKit", "CoreVideo", "QuartzCore")
                    pl.add_link_directories("../dependencies/cpython/build/debug/")
                    pl.add_compiler_flags("-Wno-deprecated-declarations")
                    pl.add_dynamic_link_libraries("python3.14d")


        with pl.configuration("deploy"):

            pl.add_static_link_libraries("dearimgui", "glfw", "pl_platform_ext")
            pl.set_output_binary("pilotlight_main")

            # win32
            with pl.platform("Windows"):
                with pl.compiler("msvc"):

                    pl.add_include_directories("%PL_PYTHON_INCLUDES%")
                    pl.add_link_directories("%PL_PYTHON_LIBS%")
                    pl.add_include_directories("%VULKAN_SDK%\\Include")
                    pl.add_static_link_libraries("vulkan-1")
                    pl.add_static_link_libraries("user32", "Shell32", "Ole32", "gdi32", "ucrt")
                    pl.add_link_directories('%VULKAN_SDK%\\Lib')
                    pl.add_linker_flags("-nodefaultlib:LIBCMT")
                    pl.add_compiler_flags("-std:c11", "-Od", "-MD", "-Zi")     

            with pl.platform("Linux"):
                with pl.compiler("gcc"):
                    pl.add_include_directories("$PL_PYTHON_INCLUDES")

            # mac os
            with pl.platform("Darwin"):
                with pl.compiler("clang"):
                    pl.add_include_directories("$PL_PYTHON_INCLUDES")
                    pl.add_link_directories('$VULKAN_SDK/lib')
                    pl.add_compiler_flags("-fmodules", "-ObjC++")
                    pl.add_linker_flags("-undefined dynamic_lookup")
                    pl.add_link_frameworks("Metal", "MetalKit", "Cocoa", "IOKit", "CoreVideo", "QuartzCore")
                    pl.add_compiler_flags("-Wno-deprecated-declarations")   

    #-----------------------------------------------------------------------------
    # [SECTION] imgui module
    #-----------------------------------------------------------------------------
    
    with pl.target("pilotlight_imgui", pl.TargetType.DYNAMIC_LIBRARY, False):

        pl.add_source_files("../extensions/pl_dearimgui_ext_m.cpp")
        
        with pl.configuration("debug"):

            pl.set_output_directory("../pilotlight")
            pl.add_static_link_libraries("dearimguid", "glfwd", "pl_platform_ext")
            
            # win32
            with pl.platform("Windows"):
                with pl.compiler("msvc"):
                    pl.set_output_binary("imgui_d")
                    pl.add_include_directories("../dependencies/cpython/PC/")
                    pl.add_include_directories("%VULKAN_SDK%\\Include")
                    pl.set_output_binary_extension(".pyd")
                    pl.add_static_link_libraries("vulkan-1")
                    pl.add_link_directories("../dependencies/cpython/PCbuild/amd64/", '%VULKAN_SDK%\\Lib')
                    pl.add_linker_flags("-nodefaultlib:LIBCMT", "-nodefaultlib:MSVCRT")
                    pl.add_compiler_flags("-std:c11", "-Od", "-MDd", "-Zi")

            # linux
            with pl.platform("Linux"):
                with pl.compiler("gcc"):
                    pl.set_output_binary("imgui")
                    pl.add_include_directories('$VULKAN_SDK/include', '/usr/include/vulkan')
                    pl.add_compiler_flags("-std=c++14")
                    pl.add_linker_flags("-lstdc++")
                    pl.add_dynamic_link_libraries("vulkan")
                    pl.add_link_directories('$VULKAN_SDK/lib')

            # mac os
            with pl.platform("Darwin"):
                with pl.compiler("clang"):
                    pl.set_output_binary("imgui")
                    pl.set_output_binary_extension(".so")
                    pl.add_compiler_flags("-fmodules", "-ObjC++")
                    pl.add_linker_flags("-undefined dynamic_lookup")
                    pl.add_link_frameworks("Metal", "MetalKit", "Cocoa", "IOKit", "CoreVideo", "QuartzCore")
                    pl.add_link_directories("../dependencies/cpython/build/debug/")
                    pl.add_compiler_flags("-Wno-deprecated-declarations")
                    pl.add_dynamic_link_libraries("python3.14d")

        with pl.configuration("deploy"):

            pl.set_output_binary("imgui")
            pl.set_output_directory("../pilotlight")
            pl.add_static_link_libraries("glfw", "pl_platform_ext", "dearimgui")

            # win32
            with pl.platform("Windows"):
                with pl.compiler("msvc"):
                    pl.add_include_directories("%PL_PYTHON_INCLUDES%", "%VULKAN_SDK%\\Include")
                    pl.set_output_binary_extension(".pyd")
                    pl.add_static_link_libraries("vulkan-1", "user32", "Shell32", "Ole32", "gdi32", "ucrt")
                    pl.add_link_directories('%VULKAN_SDK%\\Lib', "%PL_PYTHON_LIBS%")
                    pl.add_linker_flags("-nodefaultlib:LIBCMT")
                    pl.add_compiler_flags("-std:c11", "-O2", "-MD", "-Zi")

            with pl.platform("Linux"):
                with pl.compiler("gcc"):
                    pl.add_include_directories("$PL_PYTHON_INCLUDES", '$VULKAN_SDK/include', '/usr/include/vulkan')
                    pl.add_compiler_flags("-std=c++14")
                    pl.add_linker_flags("-lstdc++")
                    pl.add_dynamic_link_libraries("vulkan")
                    pl.add_link_directories('$VULKAN_SDK/lib')

            # mac os
            with pl.platform("Darwin"):
                with pl.compiler("clang"):
                    pl.add_include_directories("$PL_PYTHON_INCLUDES")
                    pl.add_link_directories('$VULKAN_SDK/lib')
                    pl.set_output_binary_extension(".so")
                    pl.add_compiler_flags("-fmodules", "-ObjC++")
                    pl.add_linker_flags("-undefined dynamic_lookup")
                    pl.add_link_frameworks("Metal", "MetalKit", "Cocoa", "IOKit", "CoreVideo", "QuartzCore")
                    pl.add_compiler_flags("-Wno-deprecated-declarations")

    #-----------------------------------------------------------------------------
    # [SECTION] pilotlight module
    #-----------------------------------------------------------------------------
    
    with pl.target("pilotlight", pl.TargetType.DYNAMIC_LIBRARY, False):

        pl.add_source_files("pilotlight.c")
        pl.set_output_binary("pilotlight")
        pl.set_output_directory("../pilotlight")
        pl.add_static_link_libraries("pilotlight_main", "pl_platform_ext")
        
        with pl.configuration("debug"):

            pl.add_static_link_libraries("dearimguid", "glfwd")
            
            # win32
            with pl.platform("Windows"):
                with pl.compiler("msvc"):
                    pl.set_output_binary("pilotlight_d")
                    pl.add_include_directories("../dependencies/cpython/PC/", "%VULKAN_SDK%\\Include")
                    pl.set_output_binary_extension(".pyd")
                    pl.add_static_link_libraries("vulkan-1")
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
                    pl.add_include_directories('$VULKAN_SDK/include', '/usr/include/vulkan')
                    pl.add_dynamic_link_libraries("vulkan")
                    pl.add_linker_flags("-lstdc++")
                    pl.add_static_link_libraries("shaderc_combined", "spirv-cross-c", "spirv-cross-core", "spirv-cross-cpp",
                        "spirv-cross-glsl", "spirv-cross-hlsl", "spirv-cross-msl", "spirv-cross-reflect", "spirv-cross-util")
                    pl.add_link_directories("../dependencies/cpython/build/debug/", '$VULKAN_SDK/lib')

            # mac os
            with pl.platform("Darwin"):
                with pl.compiler("clang"):
                    pl.add_compiler_flags("-std=c99", "-fmodules", "-ObjC")
                    pl.add_linker_flags("-undefined dynamic_lookup")
                    pl.set_output_binary_extension(".so")
                    pl.add_link_frameworks("Metal", "MetalKit", "Cocoa", "IOKit", "CoreVideo", "QuartzCore")
                    pl.add_static_link_libraries("shaderc_combined", "spirv-cross-c", "spirv-cross-core", "spirv-cross-cpp",
                        "spirv-cross-glsl", "spirv-cross-hlsl", "spirv-cross-msl", "spirv-cross-reflect", "spirv-cross-util")
                    pl.add_link_directories("../dependencies/cpython/build/debug/")
                    pl.add_compiler_flags("-Wno-deprecated-declarations")
                    pl.add_dynamic_link_libraries("python3.14d")

        with pl.configuration("deploy"):

            pl.set_output_binary("pilotlight")
            pl.add_static_link_libraries("dearimgui", "glfw")

            # win32
            with pl.platform("Windows"):
                with pl.compiler("msvc"):
                    pl.add_include_directories("%PL_PYTHON_INCLUDES%", "%VULKAN_SDK%\\Include")
                    pl.add_link_directories("%PL_PYTHON_LIBS%", '%VULKAN_SDK%\\Lib')
                    pl.set_output_binary_extension(".pyd")
                    pl.add_static_link_libraries("vulkan-1")
                    pl.add_static_link_libraries("shaderc_combined", "spirv-cross-c", "spirv-cross-core", "spirv-cross-cpp",
                        "spirv-cross-glsl", "spirv-cross-hlsl", "spirv-cross-msl", "spirv-cross-reflect", "spirv-cross-util")
                    pl.add_static_link_libraries("user32", "Shell32", "Ole32", "gdi32", "ucrt")
                    pl.add_linker_flags("-nodefaultlib:LIBCMT")
                    pl.add_compiler_flags("-std:c11", "-O2", "-MD", "-Zi")

            with pl.platform("Linux"):
                with pl.compiler("gcc"):
                    pl.add_include_directories("$PL_PYTHON_INCLUDES", '$VULKAN_SDK/include', '/usr/include/vulkan')
                    pl.add_dynamic_link_libraries("vulkan")
                    pl.add_static_link_libraries("shaderc_combined", "spirv-cross-c", "spirv-cross-core", "spirv-cross-cpp",
                        "spirv-cross-glsl", "spirv-cross-hlsl", "spirv-cross-msl", "spirv-cross-reflect", "spirv-cross-util")
                    pl.add_link_directories('$VULKAN_SDK/lib')

            # mac os
            with pl.platform("Darwin"):
                with pl.compiler("clang"):
                    pl.add_include_directories("$PL_PYTHON_INCLUDES")
                    pl.add_link_directories('$VULKAN_SDK/lib')
                    pl.set_output_binary_extension(".so")
                    pl.add_compiler_flags("-std=c99", "-fmodules", "-ObjC")
                    pl.add_linker_flags("-undefined dynamic_lookup")
                    pl.add_link_frameworks("Metal", "MetalKit", "Cocoa", "IOKit", "CoreVideo", "QuartzCore")
                    pl.add_static_link_libraries("shaderc_combined", "spirv-cross-c", "spirv-cross-core", "spirv-cross-cpp",
                        "spirv-cross-glsl", "spirv-cross-hlsl", "spirv-cross-msl", "spirv-cross-reflect", "spirv-cross-util")
                    pl.add_compiler_flags("-Wno-deprecated-declarations")

    #-----------------------------------------------------------------------------
    # [SECTION] sandbox executable
    #-----------------------------------------------------------------------------

    with pl.target("pilotlight_python", pl.TargetType.EXECUTABLE, False):

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

# copy shaders
if os.path.isdir(working_directory + "/../pilotlight/shaders"):
    shutil.rmtree(working_directory + "/../pilotlight/shaders")


shutil.copytree(working_directory + "/../../pilotlight/shaders", working_directory + "/../pilotlight/shaders")
