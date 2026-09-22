#!/usr/bin/env bash
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

# Hints:
# - nerd English is OOTB ~ strings in the source code
# - cn ... user friendly Chinese translation
# - cs ... user friendly Czech translation
# - en ... user friendly English translation
# - hi ... user friendly Hindi translation
# - sp ... user friendly Spanish translation

MF_LANG="cs"

if [[ "${1}" ]]
then
    MF_LANG="${1}"
fi

echo "Running Qt Linguist for language: ${MF_LANG} (options: cs, en, es, hi, zh_cn)"

# edit ONE language in Qt Linguist, then compile ALL languages to .qm catalogs
pushd . && cd ../../app/resources/qt/translations && linguist mindforger_${MF_LANG}.ts && popd \
&& ./l10n-release-strings.sh

# eof
