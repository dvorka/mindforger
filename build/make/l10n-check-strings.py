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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

"""Check that MindForger l10n catalogs (.ts) of all languages are in sync.

Checks:

  1. every catalog knows the very same set of source strings
  2. every catalog translates all of them - the English catalogs are an
     exception as an empty translation there means "use the source string"
  3. no language is behind the reference language in the user friendly
     (normal) vs. hacker (nerd) terminology - like Knowledge vs. Mind
  4. deprecated terminology (repository) is not used where the source
     string already uses the new one (workspace)

Run it either directly or using the 'l10n-check' target of build/Makefile.
"""

import os
import re
import sys
import xml.etree.ElementTree as ET

# catalogs live next to the .qm binary catalogs bundled by app/mf-resources.qrc
TRANSLATIONS_DIR = os.path.normpath(
    os.path.join(
        os.path.dirname(os.path.abspath(__file__)),
        "..", "..", "app", "resources", "qt", "translations"
    )
)
# languages: user friendly (normal) and hacker (nerd) flavor of each of them
LANGUAGES = ["en", "cs", "es", "hi", "zh_cn"]
# reference language whose terminology split is required from all the others
REFERENCE_LANGUAGE = "en"
# English catalogs override source strings only where terminology differs
FALLBACK_LANGUAGES = ["en"]
# translations of the source strings which were migrated to 'workspace'
DEPRECATED_TERMS = {
    "en": r"repositor",
    "cs": r"úložišt|repozitář",
    "es": r"repositorio",
    "hi": r"रिपॉज़िटरी|रिपोजिटरी",
    "zh_cn": r"仓库|存储库",
}
# translations which legitimately keep the deprecated term (Git hosting)
DEPRECATED_TERMS_ALLOWED = ["GitHub Repositories"]


def catalog_name(language, nerd):
    """Build .ts catalog file name of the given language flavor."""
    return "mindforger_{}{}.ts".format("nerd_" if nerd else "", language)


def load_catalog(name):
    """Load live (neither obsolete, nor vanished) messages of a catalog."""
    messages = {}
    path = os.path.join(TRANSLATIONS_DIR, name)
    for context in ET.parse(path).getroot().findall("context"):
        context_name = context.findtext("name")
        for message in context.findall("message"):
            translation = message.find("translation")
            if translation.get("type") in ("obsolete", "vanished"):
                continue
            key = (context_name, message.findtext("source") or "")
            messages[key] = translation.text or ""
    return messages


def check_sources_in_sync(catalogs, errors):
    """Check that all catalogs know the very same source strings."""
    reference = catalog_name(REFERENCE_LANGUAGE, False)
    for name, messages in catalogs.items():
        for key in sorted(set(catalogs[reference]) - set(messages)):
            errors.append(
                "ERROR: {}: missing source string [{}] '{}'".format(
                    name, key[0], key[1]))
        for key in sorted(set(messages) - set(catalogs[reference])):
            errors.append(
                "ERROR: {}: unknown source string [{}] '{}'".format(
                    name, key[0], key[1]))


def check_translated(catalogs, errors):
    """Check that no language but the fallback ones has empty translations."""
    for name, messages in catalogs.items():
        if any(name == catalog_name(l, n)
               for l in FALLBACK_LANGUAGES for n in (False, True)):
            continue
        for key in sorted(k for k, v in messages.items() if not v.strip()):
            errors.append(
                "ERROR: {}: untranslated string [{}] '{}'".format(
                    name, key[0], key[1]))


def check_terminology_split(catalogs, errors):
    """Check normal vs. nerd terminology split of the reference language."""
    reference = catalogs[catalog_name(REFERENCE_LANGUAGE, False)]
    reference_nerd = catalogs[catalog_name(REFERENCE_LANGUAGE, True)]
    split = [
        key for key, value in reference.items()
        if value.strip() and value != reference_nerd.get(key, "")
    ]
    for language in LANGUAGES:
        if language == REFERENCE_LANGUAGE:
            continue
        name = catalog_name(language, False)
        normal, nerd = catalogs[name], catalogs[catalog_name(language, True)]
        for key in sorted(k for k in split if normal.get(k) == nerd.get(k)):
            errors.append(
                "ERROR: {}: '{}' is behind {} - normal and nerd flavor of "
                "'{}' are the same".format(
                    name, key[1], reference_nerd.get(key, ""), normal.get(key)))


def check_deprecated_terms(catalogs, errors):
    """Check that translations don't use terminology dropped by the source."""
    for language in LANGUAGES:
        deprecated = re.compile(DEPRECATED_TERMS[language], re.IGNORECASE)
        for nerd in (False, True):
            name = catalog_name(language, nerd)
            for key, value in sorted(catalogs[name].items()):
                if key[1] in DEPRECATED_TERMS_ALLOWED:
                    continue
                if deprecated.search(value):
                    errors.append(
                        "ERROR: {}: deprecated term in translation of "
                        "[{}] '{}': '{}'".format(name, key[0], key[1], value))


def main():
    print("Checking l10n catalogs in: {}".format(TRANSLATIONS_DIR))
    catalogs = {}
    for language in LANGUAGES:
        for nerd in (False, True):
            name = catalog_name(language, nerd)
            catalogs[name] = load_catalog(name)
            print("  {:30s} {:5d} live strings".format(name, len(catalogs[name])))

    errors = []
    check_sources_in_sync(catalogs, errors)
    check_translated(catalogs, errors)
    check_terminology_split(catalogs, errors)
    check_deprecated_terms(catalogs, errors)

    if errors:
        print("\n".join(errors))
        print("ERROR: l10n catalogs are NOT in sync - {} problem(s) found".format(
            len(errors)))
        return 1
    print("DONE: l10n catalogs of all languages are in sync")
    return 0


if __name__ == "__main__":
    sys.exit(main())

# eof
