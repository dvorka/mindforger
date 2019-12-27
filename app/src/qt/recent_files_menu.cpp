/*
 * Copyright 2013 Christian Loose <christian.loose@hamburg.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "recent_files_menu.h"

#include <QFileInfo>
#include <QDir>
#include <QSettings>

namespace m8r {

RecentFilesMenu::RecentFilesMenu(const QString &title, QWidget* parent) :
    QMenu(title, parent)
{
    setIcon(QIcon(":/menu-icons/open-recent.svg"));
}

void RecentFilesMenu::readState()
{
    QSettings settings;

    int size = settings.beginReadArray("recentFiles");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        recentFiles << settings.value("fileName").toString();
    }
    settings.endArray();

    updateMenu();
}

void RecentFilesMenu::saveState() const
{
    QSettings settings;

    settings.beginWriteArray("recentFiles");
    for (int i = 0; i < recentFiles.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("fileName", recentFiles.at(i));
    }
    settings.endArray();
}

void RecentFilesMenu::addFile(const QString& fileName)
{
    QFileInfo fileInfo(fileName);
    QString absoluteNativeFileName(QDir::toNativeSeparators(fileInfo.absoluteFilePath()));

    // add file to top of list
    recentFiles.removeAll(absoluteNativeFileName);
    recentFiles.prepend(absoluteNativeFileName);

    // remove last entry if list contains more than 10 entries
    if (recentFiles.size() > 10) {
        recentFiles.removeLast();
    }

    updateMenu();
}

void RecentFilesMenu::clearMenu()
{
    recentFiles.clear();
    updateMenu();
}

void RecentFilesMenu::recentFileTriggered()
{
    QAction *action = qobject_cast<QAction*>(sender());
    emit recentFileTriggered(action->data().toString());
}

void RecentFilesMenu::updateMenu()
{
    clear();

    foreach (const QString& recentFile, recentFiles) {
        QAction *action = addAction(QIcon(":/menu-icons/open-recent.svg"), recentFile);
        action->setData(recentFile);

        connect(action, SIGNAL(triggered()),
                this, SLOT(recentFileTriggered()));
    }

    addSeparator();
    addAction(
        QIcon(":/menu-icons/clear.svg"),
        tr("Clear Menu"),
        this,
        SLOT(clearMenu())
    );

    setEnabled(!recentFiles.empty());
}

} // namespace
