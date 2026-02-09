import os
import shutil


if not os.path.isdir("../out/pilotlight"):
    os.mkdir("../out/pilotlight")

# copy core headers
shutil.copy("../pilotlight/__init__.py", "../out/pilotlight/__init__.py")
shutil.copy("../pilotlight/pilotlight.pyi", "../out/pilotlight/pilotlight.pyi")
shutil.copy("../pilotlight/pilotlight.pyd", "../out/pilotlight/pilotlight.pyd")
shutil.copy("../pilotlight/pl_core.py", "../out/pilotlight/pl_core.py")
shutil.copy("../pilotlight/pl_draw_ext.py", "../out/pilotlight/pl_draw_ext.py")
shutil.copy("../pilotlight/pl_starter_ext.py", "../out/pilotlight/pl_starter_ext.py")
shutil.copy("../pilotlight/pl_ui_ext.py", "../out/pilotlight/pl_ui_ext.py")
shutil.copy("../pilotlight/pl_vfs_ext.py", "../out/pilotlight/pl_vfs_ext.py")
shutil.copy("../pilotlight/spirv-cross-c-shared.dll", "../out/pilotlight/spirv-cross-c-shared.dll")
