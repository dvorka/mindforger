/*
 ner_main_window_worker_thread.h     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_NER_MAIN_WINDOW_WORKER_THREAD_H
#define M8RUI_NER_MAIN_WINDOW_WORKER_THREAD_H

#include <vector>

#include <QtWidgets>

#include "orloj_presenter.h"

namespace m8r {

class OrlojPresenter;

/**
 * @brief NER worker thread class.
 *
 * IMPORTANT: remember that QThread subclassing is ANTIPATTERN - QThread is just envelope.
 *
 * IMPORTANT: NEVER allocate heap objects (using new) in the constructor of the QObject
 * class as this allocation is then performed on the main thread and not on the
 * new QThread instance, meaning that the newly created object is then owned by the
 * main thread and not the QThread instance. This will make your code fail to work.
 * Instead, allocate such resources in the main function slot such as process() in
 * this case as when that is called the object will be on the new thread instance
 * and thus it will own the resource.
 */
class NerMainWindowWorkerThread : public QObject
{
    Q_OBJECT

    // just (parent) thread handle allowing to delete it when worker finishes
    QThread* thread;

    Mind* mind;
    OrlojPresenter* orloj;
    int entityFilter;
    std::vector<NerNamedEntity>* result;
    QDialog* progressDialog;

public:
    explicit NerMainWindowWorkerThread(
        QThread* t,
        Mind* m,
        OrlojPresenter* o,
        int f,
        std::vector<NerNamedEntity>* r,
        QDialog* d)
    {
        this->thread = t;
        this->mind = m;
        this->orloj = o;
        this->entityFilter = f;
        this->result = r;
        this->progressDialog = d;
    }
    NerMainWindowWorkerThread(const NerMainWindowWorkerThread&) = delete;
    NerMainWindowWorkerThread(const NerMainWindowWorkerThread&&) = delete;
    NerMainWindowWorkerThread &operator=(const NerMainWindowWorkerThread&) = delete;
    NerMainWindowWorkerThread &operator=(const NerMainWindowWorkerThread&&) = delete;
    ~NerMainWindowWorkerThread();

    std::vector<NerNamedEntity>* getResult() { return result; }

public slots:
    void process();

signals:
    void finished();
    void error(QString error);
};

}
#endif // M8RUI_NER_MAIN_WINDOW_WORKER_THREAD_H
