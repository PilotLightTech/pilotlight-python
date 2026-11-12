import os
import sys
from setuptools import setup, Distribution

# build with: python -m build --wheel
#   (the compiled extension must already be in pilotlight/, see scripts/gen_build.py)

class BinaryDistribution(Distribution):
    """the package contains a prebuilt extension module (platform specific wheel)"""
    def has_ext_modules(self):
        return True

def setup_package():

    src_path = os.path.dirname(os.path.abspath(__file__))
    old_path = os.getcwd()
    os.chdir(src_path)
    sys.path.insert(0, src_path)

    with open(os.path.join(src_path, "README.md"), encoding="utf-8") as readme_file:
        long_description = readme_file.read()

    # prebuilt extension module (release builds only, never the debug "pilotlight_d")
    if sys.platform == "win32":
        extension_module = "pilotlight.pyd"
    else:
        extension_module = "pilotlight.so"
    if not os.path.isfile(os.path.join(src_path, "pilotlight", extension_module)):
        raise RuntimeError(f"pilotlight/{extension_module} not found, build it first (scripts/gen_build.py, then src/build.* -c deploy)")

    metadata = dict(
        name='pilotlight',
        version="0.2.0",
        author="Jonathan Hoffstadt",
        author_email="jonathanhoffstadt@yahoo.com",
        description='Python bindings and project tools for the Pilot Light real-time application framework',
        long_description=long_description,
        long_description_content_type='text/markdown',
        url='https://github.com/PilotLightTech/pilotlight-python',
        # bundled/linked third party components are listed in THIRD_PARTY_NOTICES.md
        # license='MIT AND OFL-1.1 AND Apache-2.0 AND BSD-3-Clause',
        # license_files=['LICENSE', 'LICENSES/*', 'THIRD_PARTY_NOTICES.md'],
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
        package_data={
            'pilotlight': [
                extension_module,
                "*.pyi",
                "py.typed",
                "shaders/*",    # trimmed to what the bindings use by scripts/gen_build.py
                "resources/*",  # trimmed to what the bindings use by scripts/gen_build.py
                "data/*",       # project generator templates (python -m pilotlight new)
            ]
        },
        include_package_data=False,
        distclass=BinaryDistribution
    )

    try:
        setup(**metadata)
    finally:
        del sys.path[0]
        os.chdir(old_path)

if __name__ == '__main__':
    setup_package()
