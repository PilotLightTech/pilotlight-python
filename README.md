# <h1 align="center">pilotlight-python</h1>

<p align="center">
  Python bindings and project tools for the
  <a href="https://github.com/PilotLightTech/pilotlight">Pilot Light</a>
  real-time application framework.
</p>

<p align="center">
  <a href="https://github.com/PilotLightTech/pilotlight-python/actions/workflows/build.yml">
    <img src="https://github.com/PilotLightTech/pilotlight-python/actions/workflows/build.yml/badge.svg?branch=master" alt="Build status">
  </a>
  <a href="https://pypi.org/project/pilotlight/">
    <img src="https://img.shields.io/pypi/v/pilotlight?label=pilotlight" alt="PyPI version">
  </a>
</p>

<p align="center">
  <a href="#about">About</a> •
  <a href="#installation">Installation</a> •
  <a href="#quick-start">Quick Start</a> •
  <a href="#using-the-python-api">Python API</a> •
  <a href="#development">Development</a> •
  <a href="#license">License</a>
</p>

---

## About

**pilotlight-python** provides Python bindings and development tools for the Pilot Light real-time application framework.

It can be used to:

* create Pilot Light projects from ready-to-use templates
* prototype real-time applications in Python
* access Pilot Light engine systems through Python
* build development tools and editor workflows
* automate asset processing and other engine tasks

Pilot Light is designed around a modular collection of APIs and extensions. Applications can use only the systems they need while retaining access to rendering, input, UI, ECS, asset loading, and other framework features.

## Installation

Install the latest published version from PyPI:

```bash
python -m pip install pilotlight
```

To upgrade an existing installation:

```bash
python -m pip install --upgrade pilotlight
```

## Quick Start - Project Generator

The project generator is the recommended starting point for new applications:

```bash
python -m pilotlight new <project-name> [options]
```

For example:

```bash
python -m pilotlight new MyGame -2d
```

This creates a new `MyGame` directory containing the files and configuration needed to get started.

```text
MyGame/
├── ...
└── ..
```

Using the generator helps ensure that a new project has the expected structure, startup code, build configuration, and supporting files for the installed Pilot Light version.

You can now run the project like so:

```bash
cd MyGame
python src/app.py
```

## Using the Python API

More complete examples are available in the generated projects and the repository examples. Documentation is lacking at the moment but will come soon.

> The Python API is still under active development. Some names and interfaces may change between releases until the package reaches a stable version.
> 
## Development
WIP

## Related repositories

* [Pilot Light](https://github.com/PilotLightTech/pilotlight) — core real-time application framework

## Status

The project is under active development.

Bug reports, documentation improvements, examples, and other contributions are welcome through the GitHub issue tracker.

## License

**pilotlight-python** is licensed under the [MIT License](LICENSE), matching the core Pilot Light framework.
