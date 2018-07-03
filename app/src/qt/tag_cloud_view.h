/*
 tag_cloud_view.h     MindForger thinking notebook

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
#ifndef M8RUI_TAG_CLOUD_VIEW_H
#define M8RUI_TAG_CLOUD_VIEW_H

#include <QtWidgets>

#include "../../lib/src/debug.h"
#include "../../lib/src/model/tag.h"
#include "../../lib/src/gear/string_utils.h"

#include "layouts/flow_layout.h"

namespace m8r {

class TagCloudView : public QWidget
        // CONDEMNED QTextBrowser: does NOT support font-size in HTML
{
    Q_OBJECT

    FlowLayout* l;

public:
    explicit TagCloudView(QWidget* parent);
    TagCloudView(const TagCloudView&) = delete;
    TagCloudView(const TagCloudView&&) = delete;
    TagCloudView &operator=(const TagCloudView&) = delete;
    TagCloudView &operator=(const TagCloudView&&) = delete;
    ~TagCloudView();

    void refresh(std::map<const Tag*,int>);
};

}
#endif // M8RUI_TAG_CLOUD_VIEW_H
