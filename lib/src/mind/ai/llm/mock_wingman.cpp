/*
 mock_wingman.cpp     MindForger thinking notebook

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
#include "mock_wingman.h"

namespace m8r {

using namespace std;

MockWingman::MockWingman(const string& llmModel)
    : Wingman(WingmanLlmProviders::WINGMAN_PROVIDER_MOCK),
      llmModel{llmModel}
{
}

MockWingman::~MockWingman()
{
}

void MockWingman::chat(CommandWingmanChat& command) {
    MF_DEBUG("MockWingman::chat() prompt:" << command.prompt << endl);

    command.httpResponse.clear();
    command.status=WingmanStatusCode::WINGMAN_STATUS_CODE_OK;
    command.errorMessage.clear();
    command.answerLlmModel.assign(this->llmModel);
    command.promptTokens=42;
    command.answerTokens=42198;
    command.answerHtml.assign("chat(MOCK, '"+command.prompt+"')");

    MF_DEBUG("MockWingman::chat() answer:" << command.answerHtml << endl);
}

} // m8r namespace
