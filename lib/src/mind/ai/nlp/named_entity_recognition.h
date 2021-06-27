/*
 named_entity_recognition.h     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_NAMED_ENTITY_RECOGNITION_H
#define M8R_NAMED_ENTITY_RECOGNITION_H

#include <vector>
#include <string>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <mutex>

#include "../../deps/mitie/mitielib/include/mitie/named_entity_extractor.h"
#include "../../deps/mitie/mitielib/include/mitie/conll_tokenizer.h"
#include "../../deps/mitie/mitielib/include/mitie.h"

#include "ner_named_entity.h"

#include "../../../model/outline.h"

namespace m8r {

class NamedEntityRecognition
{
private:
    std::mutex initMutex;
    bool initilized;

    std::string  nerModelPath;
    mitie::named_entity_extractor nerModel;

public:
    explicit NamedEntityRecognition();
    NamedEntityRecognition(const NamedEntityRecognition&) = delete;
    NamedEntityRecognition(const NamedEntityRecognition&&) = delete;
    NamedEntityRecognition &operator=(const NamedEntityRecognition&) = delete;
    NamedEntityRecognition &operator=(const NamedEntityRecognition&&) = delete;
    ~NamedEntityRecognition();

    bool isInitialized() const { return initilized; }

    /**
     * @brief Set NER model location.
     *
     * This set path to the method, but it does NOT load and initialize it.
     */
    void setNerModel(const std::string& nerModel);

    /**
     * @brief NRE persons in memory.
     */
    bool recognizePersons(std::vector<NerNamedEntity>& result);

    /**
     * @brief NRE persons in O.
     */
    bool recognizePersons(const Outline* outline, int entityTypeFilter, std::vector<NerNamedEntity>& result);

private:
    std::vector<std::string> tokenizeFile(const std::string& filename);

    /**
     * @brief Load and initialize NER model file.
     *
     * NER file is typically huge (MBs) therefore it is loaded and initialized on demand.
     */
    bool loadAndInitNerModel();
};

}
#endif // M8R_NAMED_ENTITY_RECOGNITION_H
