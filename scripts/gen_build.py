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

import pl_build.core as pl
import pl_build.backend_win32 as win32

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
    pl.add_include_directories("../src")
        
    #-----------------------------------------------------------------------------
    # [SECTION] examples
    #-----------------------------------------------------------------------------

    with pl.target("pilotlight_python", pl.TargetType.EXECUTABLE, False):

        pl.add_include_directories(
            "../../pilotlight/libs",
            "../dependencies/cpython/",
            "../dependencies/cpython/Include/",
            "../dependencies/cpython/PC/"
        )

        pl.add_source_files("main.c")
        pl.set_output_binary("pilotlight_python")

        with pl.configuration("debug"):

            # win32
            with pl.platform("Windows"):
                with pl.compiler("msvc"):
                    pl.add_link_directories("../dependencies/cpython/PCbuild/amd64/")
                    pl.add_linker_flags("-noimplib", "-noexp", "-incremental:no")
                    pl.add_compiler_flags("-Zc:preprocessor", "-nologo", "-std:c11", "-W4", "-WX", "-wd4201",
                                            "-wd4100", "-wd4996", "-wd4505", "-wd4189", "-wd5105", "-wd4115",
                                            "-permissive-", "-Od", "-MDd", "-Zi")
                    pl.set_post_target_build_step(
                        '@copy "..\\dependencies\\cpython\\PCbuild\\amd64\\python314_d.dll" "..\\out\\" >nul\n'
                        '@copy "..\\dependencies\\cpython\\PCbuild\\amd64\\python314.dll" "..\\out\\" >nul\n'
                        '@copy "..\\dependencies\\cpython\\PCbuild\\amd64\\python3.dll" "..\\out\\" >nul\n'
                        '@copy "..\\dependencies\\cpython\\PCbuild\\amd64\\python3_d.dll" "..\\out\\" >nul\n'
                        )

    #-----------------------------------------------------------------------------
    # [SECTION] examples
    #-----------------------------------------------------------------------------
    
    with pl.target("pilotlight", pl.TargetType.DYNAMIC_LIBRARY, False):

        pl.add_include_directories(
            "../../pilotlight/libs",
            "../dependencies/cpython/",
            "../dependencies/cpython/Include/",
            "../dependencies/cpython/PC/"
        )

        pl.add_source_files("module_pilotlight.c")
        pl.set_output_binary("_pilotlight")

        with pl.configuration("distribute"):

            # win32
            with pl.platform("Windows"):
                with pl.compiler("msvc"):
                    pl.set_output_binary_extension(".pyd")
                    pl.add_link_directories("../dependencies/cpython/PCbuild/amd64/")
                    pl.add_linker_flags("-noimplib", "-noexp", "-incremental:no")
                    pl.add_compiler_flags("-Zc:preprocessor", "-nologo", "-std:c11", "-W4", "-WX", "-wd4201",
                                            "-wd4100", "-wd4996", "-wd4505", "-wd4189", "-wd5105", "-wd4115",
                                            "-permissive-", "-Od", "-MD", "-Zi")


                    

#-----------------------------------------------------------------------------
# [SECTION] generate scripts
#-----------------------------------------------------------------------------

win32.generate_build(working_directory + '/' + "build.bat")