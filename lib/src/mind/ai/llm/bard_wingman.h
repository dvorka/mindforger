/*
 bard_wingman.h     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_BARD_WINGMAN_H
#define M8R_BARD_WINGMAN_H

namespace m8r {

/*

PaLM2 prompt: Give C++ call to bard API to get inference

ANSWER:

To call the Bard API in C++ to get an inference, you can use the following code:

#include <curl/curl.h>
#include <iostream>

int main() {
  // Initialize CURL
  CURL *curl = curl_easy_init();
  if (!curl) {
    std::cerr << "Failed to initialize CURL" << std::endl;
    return 1;
  }

  // Set the URL of the Bard API endpoint
  curl_easy_setopt(curl, CURLOPT_URL, "https://bard.google.com/v2/query");

  // Set the request body
  std::string requestBody = "query=Hello, world!";
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBody.c_str());

  // Set the content type
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER,
                   std::vector<std::string>({
                       {"Content-Type: application/json"},
                   }));

  // Perform the request
  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    std::cerr << "Failed to perform the request: " << curl_easy_strerror(res)
              << std::endl;
    return 1;
  }

  // Get the response
  std::string responseBody;
  curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &responseBody.size());
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res);
  if (res != CURLE_OK || responseBody.empty()) {
    std::cerr << "Failed to get a valid response" << std::endl;
    return 1;
  }

  // Read the response body
  std::vector<char> responseBuffer(responseBody.size());
  curl_easy_read_buffer(curl, responseBuffer.data(), responseBuffer.size());
  responseBody.assign(responseBuffer.data(), responseBuffer.size());

  // Close the connection
  curl_easy_cleanup(curl);

  // Print the response
  std::cout << responseBody << std::endl;

  return 0;
}

*/
class BardWingman
{
public:
    explicit BardWingman();
    BardWingman(const BardWingman&) = delete;
    BardWingman(const BardWingman&&) = delete;
    BardWingman& operator =(const BardWingman&) = delete;
    BardWingman& operator =(const BardWingman&&) = delete;
    ~BardWingman();
};

}
#endif // M8R_BARD_WINGMAN_H
