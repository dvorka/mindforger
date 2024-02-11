/*
 run_tool_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "run_tool_dialog.h"

namespace m8r {

using namespace std;

RunToolDialog::RunToolDialog(QWidget* parent)
    : QDialog(parent)
{
    // UI
    phraseLabel = new QLabel{tr("Phrase:"), parent};
    phraseEdit = new QLineEdit{parent};

    toolLabel = new QLabel{tr("Knowledge source:"), parent};
    toolCombo = new QComboBox{this};
    for(string toolId: KnowledgeTool::getToolIds()) {
        toolCombo->addItem(
            getToolNameForToolId(toolId),
            QString::fromStdString(toolId));
    }

    templateLabel = new QLabel{tr("Template:"), parent};
    templateEdit = new QLineEdit{parent};

    // IMPROVE disable/enable find button if text/path is valid: freedom vs validation
    runButton = new QPushButton{tr("&Get")};
    runButton->setDefault(true);
    closeButton = new QPushButton{tr("&Cancel")};

    // signals
    QObject::connect(
        closeButton, SIGNAL(clicked()),
        this, SLOT(close()));

    // assembly
    QVBoxLayout* mainLayout = new QVBoxLayout{};
    mainLayout->addWidget(phraseLabel);
    mainLayout->addWidget(phraseEdit);
    mainLayout->addWidget(toolLabel);
    mainLayout->addWidget(toolCombo);
    mainLayout->addWidget(templateLabel);
    mainLayout->addWidget(templateEdit);

    QHBoxLayout* buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(runButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // signals
    QObject::connect(
        toolCombo, SIGNAL(currentIndexChanged(QString)),
        this, SLOT(handleChangeToolCombo(QString))
    );

    // dialog
    setWindowTitle(tr("Retrieve Knowledge"));
    resize(fontMetrics().averageCharWidth()*55, height());
    setModal(true);
}

RunToolDialog::~RunToolDialog()
{
}

QString RunToolDialog::getToolNameForToolId(std::string toolId) const
{
    if(KnowledgeTool::ARXIV == toolId) {
        return tr("arXiv");
    } else if(KnowledgeTool::WIKIPEDIA == toolId) {
        return tr("Wikipedia");
    } else if(KnowledgeTool::GH_REPOS == toolId) {
        return tr("GitHub Repositories");
    } else if(KnowledgeTool::GH_CODE == toolId) {
        return tr("GitHub Code");
    } else if(KnowledgeTool::DUCKDUCKGO == toolId) {
        return tr("DuckDuckGo");
    } else if(KnowledgeTool::GOOGLE == toolId) {
        return tr("Google");
    } else if(KnowledgeTool::STACK_OVERFLOW == toolId) {
        return tr("StackOverflow");
    } else if(KnowledgeTool::CPP == toolId) {
        return tr("CPP reference");
    } else if(KnowledgeTool::PYTHON == toolId) {
        return tr("Python documentation");
    }

    // no suitable name available
    return QString::fromStdString(toolId);
}

bool RunToolDialog::selectToolById(std::string toolId) {
    MF_DEBUG(
        "RunToolDialog::selectToolById(" << toolId << ") in variantData: " << std::endl);

    QVariant desiredValue = QVariant::fromValue<QString>(
        QString::fromStdString(toolId));
    int index = this->toolCombo->findData(desiredValue, Qt::UserRole);
    if(index != -1) {
        this->toolCombo->setCurrentIndex(index);
        return true;
    }

    MF_DEBUG(
        "ERROR: RunToolDialog::setSelectedToolId() - toolId '"
        << toolId << "' not found!" << std::endl);
    return false;
}

void RunToolDialog::show()
{
    QDialog::show();
}

void RunToolDialog::handleChangeToolCombo(const QString& text)
{
    MF_DEBUG("Tool changed: " << text.toStdString() << endl);

    this->templateEdit->setText(
        QString::fromStdString(
            KnowledgeTool::getUrlTemplateForToolId(
                text.toStdString())));
}

} // m8r namespace
