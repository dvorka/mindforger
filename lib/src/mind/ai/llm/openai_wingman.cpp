/*
 openai_wingman.cpp     MindForger thinking notebook

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
#include "openai_wingman.h"

namespace m8r {

using namespace std;

/*
 * cURL callback for writing data to string.
 */

static size_t WriteCallback(
    void* contents, size_t size, size_t nmemb, void* userp
) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);

    return size * nmemb;
}

/*
 * OpenAi Wingman class implementation.
 */

OpenAiWingman::OpenAiWingman()
    : Wingman(WingmanLlmProviders::WINGMAN_PROVIDER_OPENAI)
{
}

OpenAiWingman::~OpenAiWingman()
{
}

void OpenAiWingman::curlGet(string& url, string& readBuffer)
{
    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // https://curl.se/libcurl/c/curl_easy_perform.html
        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);
        MF_DEBUG("Wingman::curlGet() result:" << res << endl);
        MF_DEBUG("Wingman::curlGet() response:" << readBuffer << endl);
    }
}

void OpenAiWingman::summarize(const string& text, string& summary)
{
    MF_DEBUG("OpenAiWingman::summarize() text:" << text << endl);

    summary = "SUMMARY(OpenAI, '"+text+"')";

    MF_DEBUG("OpenAiWingman::summarize() summary:" << summary << endl);
}

} // m8r namespace
