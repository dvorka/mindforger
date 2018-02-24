/*
 note_type.h     MindForger thinking notebook

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
#ifndef M8R_NOTE_TYPE_H_
#define M8R_NOTE_TYPE_H_

#include "../config/color.h"
#include "../mind/ontology/clazz.h"

namespace m8r {

class NoteType : public Clazz
{
private:
    const Color& color;

public:
    // static initialization order fiasco prevention
    static const std::string& KeyNote() {
        static const std::string KEY_NOTE = std::string{"Note"};
        return KEY_NOTE;
    }
    static const std::string& KeyIdea() {
        static const std::string KEY_IDEA = std::string{"Idea"};
        return KEY_IDEA;
    }
    static const std::string& KeyQuestion() {
        static const std::string KEY_QUESTION = std::string{"Question"};
        return KEY_QUESTION;
    }
    static const std::string& KeyStrength() {
        static const std::string KEY_STRENGTH = std::string{"Strength"};
        return KEY_STRENGTH;
    }
    static const std::string& KeyWeakness() {
        static const std::string KEY_WEAKNESS = std::string{"Weakness"};
        return KEY_WEAKNESS;
    }
    static const std::string& KeyFact() {
        static const std::string KEY_FACT = std::string{"Fact"};
        return KEY_FACT;
    }
    static const std::string& KeyOption() {
        static const std::string KEY_OPTION = std::string{"Option"};
        return KEY_OPTION;
    }
    static const std::string& KeyOpportunity() {
        static const std::string KEY_OPPORTUNITY = std::string{"Opportunity"};
        return KEY_OPPORTUNITY;
    }
    static const std::string& KeyThreat() {
        static const std::string KEY_THREAT = std::string{"Threat"};
        return KEY_THREAT;
    }
    static const std::string& KeyAction() {
        static const std::string KEY_ACTION = std::string{"Action"};
        return KEY_ACTION;
    }
    static const std::string& KeyTask() {
        static const std::string KEY_TASK = std::string{"Task"};
        return KEY_TASK;
    }
    static const std::string& KeyResult() {
        static const std::string KEY_RESULT = std::string{"Result"};
        return KEY_RESULT;
    }
    static const std::string& KeySolution() {
        static const std::string KEY_SOLUTION = std::string{"Solution"};
        return KEY_SOLUTION;
    }
    static const std::string& KeyLesson() {
        static const std::string KEY_LESSON = std::string{"Lesson"};
        return KEY_LESSON;
    }
    static const std::string& KeyExperience() {
        static const std::string KEY_EXPERIENCE = std::string{"Experience"};
        return KEY_EXPERIENCE;
    }
    static const std::string& KeyConclusion() {
        static const std::string KEY_CONCLUSION = std::string{"Conclusion"};
        return KEY_CONCLUSION;
    }

    NoteType() = delete;
    NoteType(std::string name, const Color& color);
    NoteType(const NoteType&) = delete;
    NoteType(const NoteType&&) = delete;
    NoteType &operator=(const NoteType&) = delete;
    NoteType &operator=(const NoteType&&) = delete;
    virtual ~NoteType();

    const Color& getColor() const { return color; }
};

} // m8r namespace

#endif /* M8R_NOTE_TYPE_H_ */
