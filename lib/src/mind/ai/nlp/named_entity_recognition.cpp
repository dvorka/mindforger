/*
 named_entity_recognition.cpp     MindForger thinking notebook

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
#include "named_entity_recognition.h"

namespace m8r {

using namespace std;

NamedEntityRecognition::NamedEntityRecognition()
    : initilized{false}, nerModel{}
{
}

NamedEntityRecognition::~NamedEntityRecognition()
{
}

void NamedEntityRecognition::setNerModel(const std::string& nerModel) {
    std::lock_guard<mutex> criticalSection{initMutex};

    initilized = false;
    nerModelPath = nerModel;
}

// this method is NOT synchronized - callers are synchronized so that race condition is avoided
bool NamedEntityRecognition::loadAndInitNerModel()
{
    if(!initilized) {
        // Load MITIE's named entity extractor from disk. Each file in the MITIE-models
        // folder begins with a string containing the name of the serialized class.  In
        // this case classname contains "mitie::named_entity_extractor". It can be used to
        // identify what is in any particular file. However, in this example we don't need
        // it so it is just ignored.
#ifdef DO_MF_DEBUG
        MF_DEBUG("NER loading model: " << nerModelPath << endl);
        auto begin = chrono::high_resolution_clock::now();
#endif
        string classname;
        dlib::deserialize(nerModelPath) >> classname >> nerModel;
        initilized = true;
#ifdef DO_MF_DEBUG
        auto end = chrono::high_resolution_clock::now();
        MF_DEBUG("NER model loaded in " << chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000.0 << "ms" << endl);
#endif

#ifdef DO_MF_DEBUG
        // print out what kind of tags this tagger can predict.
        const std::vector<string> tagstr = nerModel.get_tag_name_strings();
        MF_DEBUG("NER tagger supports "<< tagstr.size() <<" tags:" << endl);
        for(unsigned int i = 0; i < tagstr.size(); ++i) {
            MF_DEBUG("   " << tagstr[i] << endl);
        }
#endif
    }

    return true;
}

vector<string> NamedEntityRecognition::tokenizeFile(const string& filename)
{
    ifstream fin(filename.c_str());
    if(!fin) {
        cout << "Unable to load input text file" << endl;
        exit(EXIT_FAILURE);
    }

    // The conll_tokenizer splits the contents of an istream into a bunch of words and is
    // MITIE's default tokenization method.
    mitie::conll_tokenizer tok(fin);
    std::vector<string> tokens;
    string token;

    // Read the tokens out of the file one at a time and store into tokens.    
    while(tok(token)) {
        tokens.push_back(token);
    }

    return tokens;
}

bool NamedEntityRecognition::recognizePersons(vector<pair<string,float>>& result)
{
    UNUSED_ARG(result);



    std::lock_guard<mutex> criticalSection{initMutex};

    if(loadAndInitNerModel()) {
        // ...
    }

    return false;
}

bool NamedEntityRecognition::recognizePersons(const Outline* outline, vector<pair<string,float>>& result)
{
    UNUSED_ARG(result);



    std::lock_guard<mutex> criticalSection{initMutex};

    if(loadAndInitNerModel())     {
        try {
            // tokenize data to prepare it for the tagger
            MF_DEBUG("NER: tokenizing O " << outline->getKey() << endl);
            std::vector<string> tokens = tokenizeFile(outline->getKey());

            std::vector<pair<unsigned long, unsigned long> > chunks;
            std::vector<unsigned long> chunk_tags;
            std::vector<double> chunk_scores;

            // Now detect all the entities in the text file we loaded and print them to the screen.
            // The output of this function is a set of "chunks" of tokens, each a named entity.
            // Additionally, if it is useful for your application a confidence score for each "chunk"
            // is available by using the predict() method.  The larger the score the more
            // confident MITIE is in the tag.
    #ifdef DO_MF_DEBUG
            MF_DEBUG("NER predicting..." << endl);
            auto begin = chrono::high_resolution_clock::now();
    #endif
            nerModel.predict(tokens, chunks, chunk_tags, chunk_scores);
    #ifdef DO_MF_DEBUG
            auto end = chrono::high_resolution_clock::now();
            MF_DEBUG("NER prediction done in " << chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000.0 << "ms" << endl);
    #endif

            // If a confidence score is not necessary for your application you can detect entities
            // using the operator() method as shown in the following line.
            //ner(tokens, chunks, chunk_tags);

            MF_DEBUG("\nNumber of named entities detected: " << chunks.size() << endl);
            const std::vector<string> tagstr = nerModel.get_tag_name_strings();
            for (unsigned int i = 0; i < chunks.size(); ++i) {
                MF_DEBUG("   Tag " << chunk_tags[i] << ": ");
                MF_DEBUG("Score: " << fixed << setprecision(3) << chunk_scores[i] << ": ");
                MF_DEBUG("" << tagstr[chunk_tags[i]] << ": ");
                // chunks[i] defines a half open range in tokens that contains the entity.
                for (unsigned long j = chunks[i].first; j < chunks[i].second; ++j) {
                    MF_DEBUG(tokens[j] << " ");
                }
                MF_DEBUG(endl);
            }

            return true;
        }
        catch(std::exception& e) {
            cerr << "NRE error: " << e.what() << endl;
        }
    }

    return false;
}

} // m8r namespace
