/*
 wingman.h     MindForger thinking notebook

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_WINGMAN_H
#define M8R_WINGMAN_H

#include <string>

namespace m8r {

class Wingman
{
public:
    explicit Wingman();
    Wingman(const Wingman&) = delete;
    Wingman(const Wingman&&) = delete;
    Wingman& operator =(const Wingman&) = delete;
    Wingman& operator =(const Wingman&&) = delete;
    ~Wingman();

    // Set LLM provider: chatGPT, h2oGPT(e), HF, ... - arg w/ interface > implementation
    void setLlmProvider();

    // use cases
    void ask(std::string question);
    void ask(std::string question, std::string context);
    void ask(std::string question, std::string context, std::string style);
    void ask(std::string question, std::string context, std::string style, std::string domain);
    // ...

    void explain(std::string term);

    void fix_grammar(std::string grammar);
    void fix_spelling(std::string spelling);
    void fix_style(std::string style);
    
    void translate(std::string text);

    void summarize(std::string text);

    void beautify(std::string text);
    void beautify(std::string text, std::string style);
    void beautify(std::string text, std::string style, std::string domain);
    void beautify(std::string text, std::string style, std::string domain, std::string topic);
    // ...

    void simplify(std::string text);
    void simplify(std::string text, std::string style);
    void simplify(std::string text, std::string style, std::string domain);
    // ...

    void generate(std::string text);
    void generate(std::string text, std::string context);
    void generate(std::string text, std::string context, std::string style);
    void generate(std::string text, std::string context, std::string style, std::string domain);
    void generate(std::string text, std::string context, std::string style, std::string domain, std::string topic);
    void generate(std::string text, std::string context, std::string style, std::string domain, std::string topic, std::string author);
    void generate(std::string text, std::string context, std::string style, std::string domain, std::string topic, std::string author, std::string emotion);
    void generate(std::string text, std::string context, std::string style, std::string domain, std::string topic, std::string author, std::string emotion, std::string intent);
    void generate(std::string text, std::string context, std::string style, std::string domain, std::string topic, std::string author, std::string emotion, std::string intent, std::string audience);
    void generate(std::string text, std::string context, std::string style, std::string domain, std::string topic, std::string author, std::string emotion, std::string intent, std::string audience, std::string purpose);

    // other use cases
    void fix_source(std::string source);
    void fix_bug(std::string bug);
    void create_plan(std::string plan);
};


/**
 * llama.cpp wingman implementation.
 *
 * LLM models to use:
 *
 * - llama 7B
 *   - llama-2-7b.Q4_0.gguf
 * - Zephir 7B
 *   - ...
 * - Mistral 7B
 *   - ...
 *
 * Plan:
 *
 * TODO: first implement unit tests for the wingman UCs as it is in the lib/
 * TODO: then integrate wingman to UI
 *   - if GPT not available in RUNTIME, then hide/disable menu items
 */
class WingmanLlamaCpp: Wingman
{
public:
};

}
#endif // M8R_WINGMAN_H
