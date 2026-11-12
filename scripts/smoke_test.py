# smoke_test.py
#   checks an installed pilotlight wheel (used by .github/workflows/deploy.yml)
#
#   usage (run outside the repo so the installed package is imported, not ./pilotlight):
#     python path/to/smoke_test.py

import os
import sys
import shutil
import tempfile
import subprocess

# windows: python doesn't search PATH for extension module dependencies, so
# point it at the Vulkan SDK's loader if one is installed (CI machines may not
# have a GPU driver providing vulkan-1.dll)
if sys.platform == "win32" and "VULKAN_SDK" in os.environ:
    sdk_bin = os.path.join(os.environ["VULKAN_SDK"], "Bin")
    if os.path.isdir(sdk_bin):
        os.add_dll_directory(sdk_bin)

import pilotlight
import pilotlight.pilotlight as pl
import pilotlight.enums
import pilotlight.types
import pilotlight.pl_script_camera

package_dir = os.path.dirname(os.path.abspath(pilotlight.__file__))
repo_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
try:
    imported_from_repo = os.path.commonpath([package_dir, repo_dir]) == repo_dir
except ValueError: # different drives (windows), so not inside the repo
    imported_from_repo = False
if imported_from_repo:
    sys.exit(f"FAIL: imported the repository's pilotlight ({package_dir}), run from outside the repo")

print("imported:", pl.__file__)

# package data
expected_package_files = [
    "py.typed",
    "pilotlight.pyi",
    "data/template_2d_app.py",
    "data/template_3d_app.py",
    "data/template.gitignore",
    "data/README.md",
    "resources/Cousine-Regular.ttf",
    "shaders/pl_draw_2d.vert",
    "shaders/pl_draw_2d.frag",
    "shaders/pl_draw_3d.vert",
]
missing = [f for f in expected_package_files if not os.path.isfile(os.path.join(package_dir, f))]
if missing:
    sys.exit(f"FAIL: missing package files: {missing}")

# project generator
with tempfile.TemporaryDirectory() as temp_dir:
    for option, template_marker in [("-2d", "2D App Template"), ("-3d", "3D App Template")]:
        project = os.path.join(temp_dir, "project" + option)
        subprocess.run([sys.executable, "-m", "pilotlight", "new", project, option], check=True)
        for path in ["src/app.py", ".gitignore", "README.md", "resources/Cousine-Regular.ttf", "shaders/pl_draw_2d.vert"]:
            if not os.path.isfile(os.path.join(project, path)):
                sys.exit(f"FAIL: 'new {option}' is missing {path}")
        with open(os.path.join(project, "src/app.py")) as app_file:
            if template_marker not in app_file.read():
                sys.exit(f"FAIL: 'new {option}' generated the wrong template")
        shutil.rmtree(project)

print("smoke test passed")
