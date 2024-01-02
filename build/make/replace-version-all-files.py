#!/usr/bin/env python3
#
# MindForger thinking notebook
#
# Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

# This script replaces version in all MindForger project files

import os

SEMANTIC_VERSION_FILES = [
    "../../build/debian/debian-aptly-add-deb.sh",
    # ^ export NEW_DEB="mindforger_1.54.0-1_amd64.deb"
    # ^ export NEW_VERSION="1.54.0"
    "../../build/debian/debian-make-deb.sh",
    # ^ export ARG_VERSION="1.54.0"
    "../../build/debian/debian/changelog",
    # ^ mindforger (1.54.0-1) unstable; urgency=low
    "../../build/fedora/fedora-rpm-from-deb.sh",
    # ^ export MFVERSION="1.54.0"
    "../../PAD.xml",
    # ^ <Program_Version>1.54.0</Program_Version>
    "../../build/Makefile",
    # ^ MINDFORGER_VERSION := 1.54.0
    "../../build/macos/env.sh",
    # ^ export MINDFORGER_VERSION="1.54.0"
    "../../build/tarball/tarball-build.sh",
    # ^ export ARG_VERSION="1.54.0"
    "../../lib/src/app_info.h",
    # ^ #define MINDFORGER_VERSION_STRING "1.54.0",
]

MINOR_VERSION_FILES = [
    "../../lib/src/app_info.h",
    # ^ #define MINDFORGER_VERSION_DWORD 1,55,0,2
    # ^ #define MINDFORGER_VERSION_MINOR "55"
]


def replace_version(
        file_path: str,
        old_version: str,
        new_version: str
):
    if os.path.isfile(file_path):
        with open(file_path, 'r') as file:
            data = file.read()

        if data and old_version and new_version:
            print(f"Replacing {old_version} -> {new_version} in {file_path}")

            updated_data = data.replace(old_version, new_version)

            with open(file_path, 'w') as file:
                file.write(updated_data)

            return

    raise FileNotFoundError(f"File {file_path} not found!")


def replace_files(
        file_paths: list,
        old_version: str,
        new_version: str
):
    for file_path in file_paths:
        replace_version(
            file_path=file_path,
            old_version=old_version,
            new_version=new_version,
        )


if __name__ == "__main__":
    old_major_version = "1"
    old_minor_version = "55"

    new_major_version = "1"
    new_minor_version = "56"

    # common files replacement
    replace_files(
        file_paths=SEMANTIC_VERSION_FILES,
        old_version=f"{old_major_version}.{old_minor_version}.0",
        new_version=f"{new_major_version}.{new_minor_version}.0",
    )

    # special files replacement
    replace_version(
        file_path=MINOR_VERSION_FILES[0],
        old_version=f"{old_major_version},{old_minor_version},0",
        new_version=f"{new_major_version},{new_minor_version},0",
    )

    # special files replacement
    replace_version(
        file_path=MINOR_VERSION_FILES[0],
        old_version=f"\"{old_minor_version}\"",
        new_version=f"\"{new_minor_version}\"",
    )
