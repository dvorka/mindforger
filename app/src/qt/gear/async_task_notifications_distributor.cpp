/*
 async_task_notifications_distributor.cpp     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "async_task_notifications_distributor.h"

namespace m8r {

using namespace std;

AsyncTaskNotificationsDistributor::AsyncTaskNotificationsDistributor(MainWindowPresenter* mwp)
    : mwp(mwp)
{
    sleepInterval = Configuration::getInstance().getDistributorSleepInterval();

    QObject::connect(
        this,
        SIGNAL(statusBarShowStatistics()),
        mwp->getStatusBar(),
        SLOT(slotShowStatistics()));
    QObject::connect(
        this,
        SIGNAL(showStatusBarInfo(QString)),
        mwp->getStatusBar(),
        SLOT(slotShowInfo(QString)));

    QObject::connect(
        this,
        SIGNAL(refreshHeaderLeaderboardByValue(AssociatedNotes*)),
        mwp->getOrloj()->getOutlineHeaderView(),
        SLOT(slotRefreshHeaderLeaderboardByValue(AssociatedNotes*)));
    QObject::connect(
        this,
        SIGNAL(refreshLeaderboardByValue(AssociatedNotes*)),
        mwp->getOrloj()->getNoteView(),
        SLOT(slotRefreshLeaderboardByValue(AssociatedNotes*)));
}

AsyncTaskNotificationsDistributor::~AsyncTaskNotificationsDistributor()
{
    for(Task* t:tasks) {
        delete t;
    }
    tasks.clear();
}

void AsyncTaskNotificationsDistributor::run()
{
    // avoid re-calculation of TayW word learderboards if it's not needed
    QString lastTayWords{};
    Outline* lastTayWOutline{};
    Note* lastTayWNote{};

    while(true) {
        msleep(sleepInterval);
        //MF_DEBUG("AsyncDistributor: wake up w/ associations need " << (int)mwp->getMind()->needForAssociations() << endl);
        if(mwp->getMind()->needForAssociations()
             ||
           mwp->getOrloj()->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)
             ||
           mwp->getOrloj()->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER))
        {
            mwp->getMind()->meditateAssociations();

            /*
             * AA FTS algorithm
             */

            if(Configuration::getInstance().getAaAlgorithm()==Configuration::AssociationAssessmentAlgorithm::WEIGHTED_FTS) {

                if(Configuration::getInstance().getMindState()==Configuration::MindState::THINKING) {

                    if(mwp->getOrloj()->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE)
                         ||
                       mwp->getOrloj()->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER))
                    {
                        AssociatedNotes* associations = new AssociatedNotes{OUTLINE, mwp->getOrloj()->getOutlineView()->getCurrentOutline()};
                        mwp->getMind()->getAssociatedNotes(*associations);
                        // send signal(s) to ensure async
                        emit showStatusBarInfo("Associated Notes for Notebook '"+QString::fromStdString(mwp->getOrloj()->getOutlineView()->getCurrentOutline()->getName())+"'...");
                        emit refreshHeaderLeaderboardByValue(associations);
                    } else if(mwp->getOrloj()->isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE)) {
                        AssociatedNotes* associations = new AssociatedNotes{NOTE, mwp->getOrloj()->getNoteView()->getCurrentNote()};
                        mwp->getMind()->getAssociatedNotes(*associations);
                        // send signal(s) to ensure async
                        emit showStatusBarInfo("Associated Notes for Note '"+QString::fromStdString(mwp->getOrloj()->getNoteView()->getCurrentNote()->getName())+"'...");
                        emit refreshLeaderboardByValue(associations);
                    } else if(mwp->getOrloj()->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
                        // think as you WRITE: detect inactivity AND refresh leadearboard for active word
                        //MF_DEBUG("AsyncDistributor: think as you WRITE (N) hits: " << mwp->getOrloj()->getNoteEdit()->getHitCounter() << endl);
                        // if there is no activity, then show leaderboard
                        if(!mwp->getOrloj()->getNoteEdit()->getHitCounter()) {
                            QString words = mwp->getOrloj()->getNoteEdit()->getRelevantWords();
                            //MF_DEBUG("AsyncDistributor: think as you WRITE (N) words '" << words.toStdString() << "'" << endl);
                            if(words.size()) {
                                // refresh leaderboard ONLY if it's different
                                if(lastTayWNote!=mwp->getOrloj()->getNoteEdit()->getCurrentNote() || lastTayWords!=words) {
                                    lastTayWNote = mwp->getOrloj()->getNoteEdit()->getCurrentNote();
                                    lastTayWords = words;

                                    AssociatedNotes* associations = new AssociatedNotes{WORD, words.toStdString(), mwp->getOrloj()->getNoteEdit()->getCurrentNote()};
                                    mwp->getMind()->getAssociatedNotes(*associations);
                                    // send signal(s) to ensure async
                                    emit showStatusBarInfo("Associated Notes for word(s) '"+words+"'...");
                                    emit refreshLeaderboardByValue(associations);
                                } else {
                                    //MF_DEBUG("AsyncDistributor: SKIPPING think as you WRITE (N) for words '" << words.toStdString() << "'" << endl);
                                }
                            }
                        }

                        mwp->getOrloj()->getNoteEdit()->clearHitCounter();
                    } else if(mwp->getOrloj()->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
                        // think as you WRITE: detect inactivity AND refresh leadearboard for word(s) under cursor
                        if(!mwp->getOrloj()->getOutlineHeaderEdit()->getHitCounter()) {
                            QString words = mwp->getOrloj()->getOutlineHeaderEdit()->getRelevantWords();
                            //MF_DEBUG("AsyncDistributor: think as you WRITE (O) hits: " << mwp->getOrloj()->getOutlineHeaderEdit()->getHitCounter() << " words '" << words.toStdString() << "'" << endl);
                            if(words.size()) {
                                // refresh leaderboard ONLY if it's different
                                if(lastTayWOutline!=mwp->getOrloj()->getOutlineHeaderEdit()->getCurrentOutline() || lastTayWords!=words) {
                                    lastTayWOutline= mwp->getOrloj()->getOutlineHeaderEdit()->getCurrentOutline();
                                    lastTayWords = words;

                                    AssociatedNotes* associations = new AssociatedNotes{WORD, words.toStdString(), mwp->getOrloj()->getOutlineHeaderEdit()->getCurrentOutline()->getOutlineDescriptorAsNote()};
                                    mwp->getMind()->getAssociatedNotes(*associations);
                                    // send signal(s) to ensure async (associations instance must NOT be deleted)
                                    emit showStatusBarInfo("Associated Notes for word(s) '"+words+"'...");
                                    emit refreshHeaderLeaderboardByValue(associations);
                                } else {
                                    //MF_DEBUG("AsyncDistributor: SKIPPING think as you WRITE (O) for words '" << words.toStdString() << "'" << endl);
                                }
                            }
                        }

                        mwp->getOrloj()->getOutlineHeaderEdit()->clearHitCounter();
                    }
                }
            }

            /*
             * AA BoW algorithm - ASYNCHRONOUS (experimental & buggy as it's unable to handle O/N deletes ~ instable)
             */

            // distribute signals from asynch tasks to frontend components
            if(Configuration::getInstance().getAaAlgorithm()==Configuration::AssociationAssessmentAlgorithm::BOW && tasks.size()) {
                std::lock_guard<mutex> criticalSection{tasksMutex};

                //MF_DEBUG("AsyncDistributor: AWAKE wip[" << tasks.size() << "]" << endl);
                vector<Task*> zombies{};
                for(Task* t:tasks) {
                    // FYI future<> had to be check for f.valid() as get() in other thread destroys it
                    if(t->isReady()) {
                        //MF_DEBUG("AsyncDistributor: future FINISHED w/ " << boolalpha << t->isSuccessful() << endl);
                        if(t->isSuccessful()) {
                            switch(t->getType()) {
                            case TaskType::DREAM_TO_THINK:
                                emit statusBarShowStatistics();
                                break;
                                // DEAD code
                            //case TaskType::NOTE_ASSOCIATIONS:
                            //    emit leaderboardRefresh(t->getNote());
                            //    break;
                            }

                            zombies.push_back(t);
                            delete t;
                            //MF_DEBUG("AsyncDistributor: task DELETED" << endl);
                        }
                    } else {
                        //MF_DEBUG("AsyncDistributor: future NOT FINISHED" << endl);
                    }
                }

                if(zombies.size()) {
                    for(Task* t:zombies) {
                        //MF_DEBUG("AsyncDistributor: erasing ZOMBIE task " << t << endl);
                        tasks.erase(std::remove(tasks.begin(), tasks.end(), t), tasks.end());
                    }
                }
            }
        }
    }
}

void AsyncTaskNotificationsDistributor::slotConfigurationUpdated()
{
    sleepInterval = Configuration::getInstance().getDistributorSleepInterval();
}

} // m8r namespace
