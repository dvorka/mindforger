/*
 status_bar_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "status_bar_presenter.h"

using namespace std;

namespace m8r {

StatusBarPresenter::StatusBarPresenter(const StatusBarView* view, Mind* mind)
    : view(view), mind(mind)
{
}

void StatusBarPresenter::showInfo(const QString& message)
{
    view->showInfo(message);
}

void StatusBarPresenter::showWarning(const QString& message)
{
    view->showWarning(message);
}

void StatusBarPresenter::showError(const QString& message)
{
    view->showError(message);
}

void StatusBarPresenter::showMindStatistics()
{
    // IMPROVE string builder
    // IMPROVE const delimiter "    "

    status.clear();

    status += " ";
    switch(Configuration::getInstance().getMindState()) {
    case Configuration::MindState::THINKING:
        status += "Thinking";
        break;
    case Configuration::MindState::DREAMING:
        status += "Dreaming";
        break;
    case Configuration::MindState::SLEEPING:
        status += "Sleeping";
        break;
    }
    status += "   ";

    switch(Configuration::getInstance().getActiveRepository()->getType()) {
    case Repository::RepositoryType::MINDFORGER:
        status += "MF";
        break;
    case Repository::RepositoryType::MARKDOWN:
        status += "MD";
        break;
    }
    switch(Configuration::getInstance().getActiveRepository()->getMode()) {
    case Repository::RepositoryMode::REPOSITORY:
        status += " repository   ";
        break;
    case Repository::RepositoryMode::FILE:
        status += " file   ";
        break;
    }

    // IMPROVE helper method @ QString gear
    status += stringFormatIntAsUs(mind->remind().getOutlinesCount());
    status += " notebooks   ";
    status += stringFormatIntAsUs(mind->remind().getNotesCount());
    status += " notes   ";
#ifdef MF_WIP
    status += stringFormatIntAsUs(mind->getTriplesCount());
    status += " triples   ";
#endif
    status += stringFormatIntAsUs(mind->remind().getOutlineMarkdownsSize());
    status += " bytes   ";
    if(mind->getScopeAspect().isEnabled()) {
        status += "scope:";
        if(mind->getTimeScopeAspect().isEnabled()) {
            status += mind->getTimeScopeAsString().c_str();
            if(mind->getTagsScopeAspect().isEnabled()) {
                status += "+tags";
            }
        } else {
            if(mind->getTagsScopeAspect().isEnabled()) {
                status += "tags";
            }
        }
        status += "   ";
    }

    if(Configuration::getInstance().isAutolinking()) {
        status += "autolinking   ";
    }


#ifdef DO_MF_DEBUG
    status += "watermark:";
    status += stringFormatIntAsUs(mind->getDeleteWatermark());
#endif

    view->showInfo(status);
}

/*
 * progress callback
 */

StatusBarProgressCallbackCtx::StatusBarProgressCallbackCtx(StatusBarPresenter* presenter)
    : presenter{presenter}
{
}

StatusBarProgressCallbackCtx::~StatusBarProgressCallbackCtx()
{
}

void StatusBarProgressCallbackCtx::updateProgress(float progress)
{
    this->presenter->showInfo(
        QString("Progress %1 %").arg(this->normalizeProgressToPercent(progress)).toStdString()
    );

    QApplication::processEvents();
}

} // namespace
