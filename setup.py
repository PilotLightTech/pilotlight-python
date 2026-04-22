from setuptools import setup, Distribution
from setuptools.command import build_py
import distutils.cmd
import os
import sys
import shutil
import glob

def get_platform():

    platforms = {
        'linux' : 'Linux',
        'linux1' : 'Linux',
        'linux2' : 'Linux',
        'darwin' : 'OS X',
        'win32' : 'Windows'
    }
    if sys.platform not in platforms:
        return sys.platform
    
    return platforms[sys.platform]

class BinaryDistribution(Distribution):
    def has_ext_modules(var):
        return True

def setup_package():

    src_path = os.path.dirname(os.path.abspath(__file__))
    old_path = os.getcwd()
    os.chdir(src_path)
    sys.path.insert(0, src_path)

    metadata = dict(
        name='pilotlight',                                      # Required
        version="0.1.15",                              # Required
        author="Jonathan Hoffstadt",       # Optional
        author_email="jonathanhoffstadt@yahoo.com",            # Optional
        description='Pilot Light',  # Required
        # long_description=long_description,                     # Optional
        # long_description_content_type='text/markdown',         # Optional
        url='https://github.com/PilotLightTech/pilotlight-python',          # Optional
        license = 'MIT',
        python_requires='>=3.14',
        classifiers=[
                'Development Status :: 3 - Alpha',
                'Intended Audience :: Education',
                'Intended Audience :: Developers',
                'Intended Audience :: Science/Research',
                'Operating System :: MacOS',
                'Operating System :: Microsoft :: Windows :: Windows 10',
                'Operating System :: POSIX',
                'Operating System :: Unix',
                'Programming Language :: Python :: 3.14',
                'Programming Language :: Python :: Implementation :: CPython',
                'Programming Language :: Python :: 3 :: Only',
                'Topic :: Software Development :: User Interfaces',
                'Topic :: Software Development :: Libraries :: Python Modules',
            ],
        packages=['pilotlight'],
        package_data={},
        distclass=BinaryDistribution
    )

    if get_platform() == "Windows":
        metadata['package_data']['pilotlight'] = [
            "__init__.py",
            "*.pyd",
            "*.py",
            "*.pyi",
            "shaders/*.inc",
            "shaders/*.comp",
            "shaders/*.glsl",
            "shaders/*.frag",
            "shaders/*.vert"
            ]
    else:
        metadata['package_data']['pilotlight'] = [
            "__init__.py",
            "pilotlight.so",
            "imgui.so",
            "*.py",
            "*.pyi",
            "shaders/*.inc",
            "shaders/*.comp",
            "shaders/*.glsl",
            "shaders/*.frag",
            "shaders/*.vert"
            ]

    if "--force" in sys.argv:
        sys.argv.remove('--force')

    try:
        setup(**metadata)
    finally:
        del sys.path[0]
        os.chdir(old_path)
    return

if __name__ == '__main__':
    setup_package()
