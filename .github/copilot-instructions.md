# Copilot instructions

This document provides instructions to the copilot AI assistants for dvorka/mindforger project.

## General instructions

- Always write beautiful, readable, and maintainable code.
- Handle errors, exceptions, and corner cases.
- Prefer clarity over cleverness. Optimize only when needed and measured.
- Always KISS - keep changes small and focused.
- Always DRY the code - do not duplicate code; create reusable classes, functions and methods; do not repeat yourself.
- Always add tests alongside code changes.

## Functional architecture instructions

- Contribute to this repository which is the thinking notebook and Markdown IDE desktop.

## Technology stack instructions

- The project is written in C++.
- The code is portable so that it can be compiled on Linux, Windows and macOS.
- The application is written using Qt framework.
- Always use C++ 11 and avoid newer language features.
- Always start code comments with lowercase letter.
- Always use `MF_DEBUG` to write debugging output.
- Always use `_WIN32` to identify Windows specific code.
- Always use `__APPLE__` to identify macOS specific code.

## Code quality instructions

- Use code formatting style as used in mindforger.cpp - comments, indentatation, parenthesis, namespaces, directives, ...

## Repository conventions

- The project is structured to the library which is then used by Qt application.
- Library dependencies are stored in `deps/` - each have its own build style.
- Library code lives under `lib/`.
- Qt application code lives under `app/`.
- Tests code lives under `lib/tests/`.
- Licenses are stored in `licenses/`.

## Test instructions

- Always use `gtest` (Google test) framework to write the test.
- Each test (function) is structured into 3 sections: `// GIVEN`, `// WHEN`, and `// THEN`. `GIVEN` section prepares the data, `WHEN` section calls the function, and `THEN` section prints results, asserts results and checks results.
- `THEN` section of the test must have at least one assert statement.
- Keep tests deterministic.
- Always use text to indicate success/failure/progress like DONE, ERROR or WIP - never use (unicode) characters like ✓ or ✗.
- Always print or log intermediate values only when they aid debugging.
- Always make sure that tests which test new feature or fix are in green.

## Build instructions

- Qt is used to describe the project structure using `*.pro` files and to build it using `qmake`.
- Makefile to build, test, run and package the project is located in `build/Makefile`.
  - Always use `make help` to find out what are the targets.

## Documentation instructions

- Markdown user documentation sources live under `doc/`.
- Doxygen documentation can be build using a target defined in `build/Makefile` - use details from that target.
- Always use `build/Makefile` targets to build the Doxygen documentation.

## Continuous Integration instructions

- GitHub Actions is used as CI for Linux and macOS.
- AppVeyor is used as CI for Windows.
- GitHub Actions CI configuration is stored under `.github/workflows/`.

## Security and secrets instructions

- Always use environment variables and secret stores.
- Always use GitHub actions secrets.
- Never commit secrets, credentials or sensitive data.
- Validate, sanitize and anonymize all external inputs.
- Always run security-focused checks.
- Always add new license to `licenses/` when you add new direct dependency.

## Release versioning instructions

- Always use semantic versioning: MAJOR.MINOR.PATCH.
- Note that releases has Git tag like `vMAJOR.MINOR.PATCH`.
- Note that releases are being developed in `dev-MAJOR.MINOR.PATCH` branches.
- Note that Git branches use naming convention for fix branch (`bug-NUMBER/DESCRIPTION`), features and enhancements (`feat-NUMBER/DESCRIPTION`) and documentation (`doc-NUMBER/DESCRIPTION`).
- Note that Conventional commits (conventionalcommits.org) are used for the commit messages.
- Always update change log stored in `Changelog` whenever you do a fix, change, or enhancement.
- Always make sure that the version is consistent in `app_info.h`, `Makefile`, `debian/debian-make-deb.sh`, `debian/changelog`, `macos/env.h`, `snap/snapcraft.yaml`, `tarball/tarball-build.sh` and `ubuntu/debian/changelog` - `app_info.h.py` is the one and only authoritative version source.
