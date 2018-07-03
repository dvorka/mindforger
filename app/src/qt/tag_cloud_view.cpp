/*
 tag_cloud_view.cpp     MindForger thinking notebook

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
#include "tag_cloud_view.h"

namespace m8r {

using namespace std;

TagCloudView::TagCloudView(QWidget* parent)
    : QWidget(parent)
{
    l = new FlowLayout(this);
    setLayout(l);
}

TagCloudView::~TagCloudView()
{
}

void TagCloudView::refresh(map<const Tag*,int> tags)
{
    string html{};

    if(tags.size()) {
        // slow, ugly and stupid: find max count as it's needed upfront
        int maxCardinality = 1;
        for(auto const& t:tags) {
            if(t.second > maxCardinality) {
                maxCardinality = t.second;
            }
        }

        float minPxSize = 10.0;
        float maxPxSize = 50.0;
        float lineTagCapacity = 9.0; // top estimate
        float linesEstimate = static_cast<float>(tags.size())/lineTagCapacity;
        float linesPxEstimate = maxPxSize*linesEstimate;
        float topBottomMargin = (static_cast<float>(height())-linesPxEstimate)/2;
        float slotCount = 10.0;
        float slotPxSize = (maxPxSize-minPxSize)/slotCount; // 4px
        float slotSize = static_cast<float>(maxCardinality)/slotCount;
        int fontPxSize;

#ifdef DO_MF_DEBUG
        MF_DEBUG("Tag cloud:" << endl);
        MF_DEBUG("  Widget height    : " << height() << endl);
        MF_DEBUG("  Widget width     : " << size().width() << endl);
        MF_DEBUG("  Lines estimate   : " << linesEstimate << endl);
        MF_DEBUG("  Lines px estimate: " << linesPxEstimate << endl);
#endif

        QSpacerItem* spacer = new QSpacerItem(1000, topBottomMargin);
        l->addItem(spacer);

        // TODO create new layout every time to ensure proper cleanup

        for(auto const& t:tags) {
            if(t.second) {
                if(!stringistring(string("none"), t.first->getName())) {
                    html.clear();
                    html += " &nbsp;<a href='abc'><span style='color: #ffffff; background-color: ";
                    html += t.first->getColor().asHtml();
                    html += "; font-weight: normal;'>&nbsp;";
                    html += t.first->getName();
                    html += "&nbsp;(";
                    html += std::to_string(t.second);
                    html += ")&nbsp;</span></a>&nbsp; ";

                    QLabel* b = new QLabel(QString::fromStdString(html), this);
                    QFont f = QApplication::font();
                    fontPxSize = minPxSize + (static_cast<float>(t.second)/slotSize)*slotPxSize;
                    f.setPointSize(fontPxSize);
                    b->setFont(f);
                    l->addWidget(b);
                }
            }
        }
    } else {
        // TODO show "No tags"
    }
}

} // m8r namespace
