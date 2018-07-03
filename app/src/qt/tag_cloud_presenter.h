/*
 tag_cloud_presenter.h     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_TAG_CLOUD_PRESENTER_H
#define M8RUI_TAG_CLOUD_PRESENTER_H

#include <QtWidgets>

#include "../../lib/src/model/tag.h"
#include "../../lib/src/mind/ontology/taxonomy.h"
#include "tag_cloud_view.h"

namespace m8r {

class TagCloudPresenter : public QObject
{
    Q_OBJECT

    TagCloudView* view;

public:
    explicit TagCloudPresenter(TagCloudView* view);
    TagCloudPresenter(const TagCloudPresenter&) = delete;
    TagCloudPresenter(const TagCloudPresenter&&) = delete;
    TagCloudPresenter &operator=(const TagCloudPresenter&) = delete;
    TagCloudPresenter &operator=(const TagCloudPresenter&&) = delete;
    ~TagCloudPresenter();

    void refresh(std::map<const Tag*,int>);
};

}
#endif // M8RUI_TAG_CLOUD_PRESENTER_H
