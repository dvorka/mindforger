#!/usr/bin/env python3
#
# MindForger thinking notebook
#
# Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>
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

# refresh PAD.xml release fields from the single sources of truth:
#
# - Program_Version                          from lib/src/app_info.h
# - Program_Release_Month/Day/Year            to today
# - Program_Change_Info                       from the latest Changelog entry
# - Program_Release_Status                    from the version bump (vs. the
#                                              previous Changelog entry)
# - File_Size_Bytes/File_Size_K/File_Size_MB  from the built installer/tarball,
#                                              when one is found
#
# this script is cross-platform (stdlib only, never shells out) so it runs the
# same on Linux, macOS and Windows; PAD.xml is edited in place with targeted
# text replacements to preserve its hand-written formatting

import datetime
import pathlib
import re
import sys
from xml.sax import saxutils

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
PAD_FILE = ROOT / "PAD.xml"
APP_INFO_FILE = ROOT / "lib" / "src" / "app_info.h"
CHANGELOG_FILE = ROOT / "Changelog"

RELEASE_STATUS = {
    "major": "Major Update",
    "minor": "Minor Update",
    "patch": "Minor Bug Fixes",
}

# PAD Program_Change_Info should stay concise; cap the generated summary
MAX_CHANGE_INFO_CHARS = 500


def read_version() -> str:
    """Return the MindForger version string from lib/src/app_info.h."""
    text = APP_INFO_FILE.read_text(encoding="utf-8")
    match = re.search(r'MINDFORGER_VERSION_STRING\s+"([^"]+)"', text)
    if not match:
        sys.exit(f"ERROR: could not read MINDFORGER_VERSION_STRING from {APP_INFO_FILE}")
    return match.group(1)


def strip_markdown(line: str) -> str:
    """Reduce a Changelog bullet to plain, single-line text."""
    line = re.sub(r"\[([^\]]+)\]\([^)]+\)", r"\1", line)
    line = line.replace("`", "").replace("**", "").replace("*", "")
    return re.sub(r"\s+", " ", line).strip()


def collect_bullets(body: list) -> list:
    """Flatten a GNU ChangeLog bullet list (lines starting with ``* ``).

    A bullet may wrap across following indented continuation lines that are
    neither blank nor the start of a new bullet.
    """
    bullets = []
    current = []

    def flush():
        if current:
            bullets.append(strip_markdown(" ".join(current)))
            current.clear()

    for line in body:
        stripped = line.strip()
        if stripped.startswith("* "):
            flush()
            current.append(stripped[2:])
        elif current and stripped:
            current.append(stripped)
        else:
            flush()
    flush()
    return bullets


def changelog_versions() -> list:
    """Return [(version, [bullets]), ...] for every ``* Released vX.Y.Z`` entry.

    MindForger's ``Changelog`` uses the GNU ChangeLog format: dated blocks of
    ``* ``-prefixed bullets, with the release bullet reading
    ``* Released vX.Y.Z - <summary>.``. Entries are returned newest-first, as
    they appear in the file.
    """
    lines = CHANGELOG_FILE.read_text(encoding="utf-8").splitlines()

    # split into date-header blocks: a block starts at a line matching
    # "YYYY-MM-DD  Name  <email>" and runs until the next such line
    blocks = []
    current = []
    header_re = re.compile(r"^\d{4}-\d{2}-\d{2}\s")
    for line in lines:
        if header_re.match(line):
            if current:
                blocks.append(current)
            current = []
        else:
            current.append(line)
    if current:
        blocks.append(current)

    versions = []
    released_re = re.compile(r"Released\s+v(\d+\.\d+\.\d+)")
    for block in blocks:
        bullets = collect_bullets(block)
        version = None
        for bullet in bullets:
            match = released_re.search(bullet)
            if match:
                version = match.group(1)
                break
        if version:
            versions.append((version, bullets))
    return versions


def release_status(version: str, previous_version: str) -> str:
    """Classify ``version`` vs. ``previous_version`` as major/minor/patch."""
    major, minor, patch = (int(p) for p in version.split("."))
    if previous_version is None:
        release_type = "major" if not (minor or patch) else "minor"
    else:
        p_major, p_minor, _ = (int(p) for p in previous_version.split("."))
        if major != p_major:
            release_type = "major"
        elif minor != p_minor:
            release_type = "minor"
        else:
            release_type = "patch"
    return RELEASE_STATUS[release_type]


def change_info(version: str, bullets: list) -> str:
    """Return the human-curated one-line summary from the 'Released vX.Y.Z - ...'
    bullet, rather than concatenating every bullet into a wall of text."""
    prefix = f"Released v{version}"
    summary = f"MindForger {version} release."
    for bullet in bullets:
        if bullet.startswith(prefix):
            _, _, text = bullet.partition("-")
            summary = text.strip() or summary
            break
    if len(summary) > MAX_CHANGE_INFO_CHARS:
        summary = summary[: MAX_CHANGE_INFO_CHARS - 1].rstrip() + "..."
    return summary


def installer_size_bytes(version: str):
    """Return the size in bytes of a built distributable for ``version``, if any.

    Checked in priority order: the Windows installer built in-tree, then the
    release tarball built by ``make dist-tarball`` into the sibling release
    directory (``../release/<version>-maker/``, alongside this repo checkout).
    Returns None when nothing has been built locally - PAD.xml's file size
    fields are then left unchanged.
    """
    candidates = [
        ROOT / "app" / "release" / "installer" / "mindforger-setup.exe",
        ROOT.parent / "release" / f"{version}-maker" / f"mindforger_{version}_tarball.tgz",
    ]
    for candidate in candidates:
        if candidate.is_file():
            return candidate.stat().st_size
    return None


def set_tag(xml: str, tag: str, value: str) -> str:
    """Replace the text content of ``<tag>`` (open/close or self-closing form)."""
    escaped = saxutils.escape(value)
    paired = re.compile(rf"<{tag}>.*?</{tag}>", re.DOTALL)
    if paired.search(xml):
        return paired.sub(lambda _: f"<{tag}>{escaped}</{tag}>", xml, count=1)
    selfclosing = re.compile(rf"<{tag}\s*/>")
    if selfclosing.search(xml):
        return selfclosing.sub(lambda _: f"<{tag}>{escaped}</{tag}>", xml, count=1)
    sys.exit(f"ERROR: tag <{tag}> not found in {PAD_FILE}")


def main() -> None:
    version = read_version()
    today = datetime.date.today()

    versions = changelog_versions()
    if not versions or versions[0][0] != version:
        sys.exit(
            f"ERROR: no Changelog entry '* Released v{version}' found as the "
            f"latest entry in {CHANGELOG_FILE}"
        )
    bullets = versions[0][1]
    previous_version = versions[1][0] if len(versions) > 1 else None

    status = release_status(version, previous_version)
    summary = change_info(version, bullets)

    xml = PAD_FILE.read_text(encoding="utf-8")
    xml = set_tag(xml, "Program_Version", version)
    xml = set_tag(xml, "Program_Release_Month", f"{today.month:02d}")
    xml = set_tag(xml, "Program_Release_Day", f"{today.day:02d}")
    xml = set_tag(xml, "Program_Release_Year", f"{today.year}")
    xml = set_tag(xml, "Program_Release_Status", status)
    xml = set_tag(xml, "Program_Change_Info", summary)

    size = installer_size_bytes(version)
    if size is not None:
        xml = set_tag(xml, "File_Size_Bytes", str(size))
        xml = set_tag(xml, "File_Size_K", f"{size / 1024:.2f}")
        xml = set_tag(xml, "File_Size_MB", f"{size / 1024 / 1024:.2f}")

    PAD_FILE.write_text(xml, encoding="utf-8")

    print(f"PAD.xml refreshed for MindForger {version} ({today.isoformat()})")
    print(f"  release status: {status}")
    print(f"  change info:    {summary[:80]}...")
    if size is not None:
        print(f"  installer size: {size} bytes ({size / 1024 / 1024:.2f} MB)")
    else:
        print("  installer size: unchanged (no built installer/tarball found)")
    print("DONE")


if __name__ == "__main__":
    main()
