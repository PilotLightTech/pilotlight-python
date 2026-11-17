"""
Pilot Light Python sandbox (development)

Runs the demo that ships with the package (pilotlight/demo.py, also available
as "python -m pilotlight demo" after pip install) against this repository's
build. Edit pilotlight/demo.py to change what the sandbox shows.

Run from out/ (see .vscode/launch.json); compiled shaders & settings go to
the repository's cache/ folder. Run scripts/gen_build.py first so the shaders
& resources are copied into pilotlight/.
"""

from pathlib import Path

from pilotlight.pilotlight import pl_run
from pilotlight.demo import App

pl_run(App(name="Python Sandbox", cache_dir=Path.cwd().resolve().parent / "cache"))
