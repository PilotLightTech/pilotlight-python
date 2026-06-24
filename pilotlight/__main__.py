import os
import sys
import shutil
import glob

if len(sys.argv) <= 1:
    print("Pilot Light - New Project Script");
    print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    print("Usage: python -m pilotlight <command> [options]");
    exit()

target_directory = ""
file_directory = os.path.dirname(os.path.abspath(__file__))

newProject = True
option_2d = True
option_3d = False
operation = ""

if len(sys.argv) > 1:
    operation = sys.argv[1]

if operation == "--help" or operation == "":
    print("Usage: python -m pilotlight <command>");
    print("\nCommands:")
    print("        help   -> how you got here")
    print("        new    -> generates new projects")
    
elif operation == "new":
    for i in range(2, len(sys.argv)):
        if sys.argv[i] == "--list-options":
            print("Usage: python -m pilotlight new <name> [options]");
            print("\nOptions:")
            print("        -2d    Generate 2D Project template")
            print("        -3d    Generate 3D Project template")
            exit()
        elif sys.argv[i] == "-2d":
            option_2d = True
        elif sys.argv[i] == "-3d":
            option_3d = True
        else:
            target_directory = sys.argv[i]

    if target_directory == "":
            print("Usage: python -m pilotlight new <name> [options]");
            print("\nOptions:")
            print("        -2d    Generate 2D Project template")
            print("        -3d    Generate 3D Project template")
            exit()
    if os.path.isdir(target_directory):
        print("Project already exists")
        exit()

    print("Generating New Project")

    os.mkdir(target_directory)
    os.mkdir(target_directory + "/src")
    os.mkdir(target_directory + "/scripts")
    os.mkdir(target_directory + "/docs")
    os.mkdir(target_directory + "/tests")
    os.mkdir(target_directory + "/shader-temp")

    shutil.copytree(file_directory + "/assets", target_directory + "/assets")
    shutil.copytree(file_directory + "/shaders", target_directory + "/shaders")
    shutil.copy(file_directory + "/data/template.gitignore", target_directory + "/.gitignore")
    shutil.copy(file_directory + "/data/README.md", target_directory + "/README.md")

    if option_2d:
        shutil.copy(file_directory + "/data/template_2d_app.py", target_directory + "/src/app.py")
    elif option_3d:
        shutil.copy(file_directory + "/data/template_3d_app.py", target_directory + "/src/app.py")