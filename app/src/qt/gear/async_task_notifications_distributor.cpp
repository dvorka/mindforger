/*
 async_task_notifications_distributor.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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
    while(true) {
        MF_DEBUG("AsyncDistributor: SLEEP..." << endl);
        // IMPROVE consider a condition variable & activiation of checking ONLY if WIP non-empty
        msleep(5000);

        if(tasks.size()) {
            std::lock_guard<mutex> criticalSection{tasksMutex};

            MF_DEBUG("AsyncDistributor: AWAKE wip[" << tasks.size() << "]" << endl);
            vector<Task*> zombies{};
            for(Task* t:tasks) {
                // FYI future<> had to be check for f.valid() as get() in other thread destroys it
                if(t->isReady()) {
                    MF_DEBUG("AsyncDistributor: future FINISHED w/ " << boolalpha << t->isSuccessful() << endl);
                    if(t->isSuccessful()) {
                        switch(t->getType()) {
                        case TaskType::DREAM_TO_THINK:
                            emit statusBarShowStatistics();
                            break;
                        case TaskType::NOTE_ASSOCIATIONS:
                            emit leaderboardRefresh(t->getNote());
                            break;
                        }

                        zombies.push_back(t);
                        delete t;
                        MF_DEBUG("AsyncDistributor: task DELETED" << endl);
                    }
                } else {
                    MF_DEBUG("AsyncDistributor: future NOT FINISHED" << endl);
                }
            }

            if(zombies.size()) {
                for(Task* t:zombies) {
                    MF_DEBUG("AsyncDistributor: erasing ZOMBIE task " << t << endl);
                    tasks.erase(std::remove(tasks.begin(), tasks.end(), t), tasks.end());
                }
            }
        }
    }
}

} // m8r namespace
