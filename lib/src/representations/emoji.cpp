/*
 emoji.cpp     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "emoji.h"

#include "../gear/string_utils.h"

namespace m8r {

using namespace std;

/*
 * Emoji
 */

Emoji::Emoji(
    const string& character,
    const string& name,
    const string& keywords
) : character(character),
    name(name),
    keywords(keywords)
{
    string searchable{name};
    if(!keywords.empty()) {
        searchable.append(" ");
        searchable.append(keywords);
    }
    stringToLower(searchable, haystack);
}

bool Emoji::matches(const vector<string>& words) const
{
    for(const string& word:words) {
        if(haystack.find(word) == string::npos) {
            return false;
        }
    }

    return true;
}

/*
 * EmojiSection
 */

EmojiSection::EmojiSection(const string& name)
    : name(name)
{
}

void EmojiSection::addEmoji(
    const string& character,
    const string& name,
    const string& keywords)
{
    emojis.push_back(Emoji{character, name, keywords});
}

vector<const Emoji*> EmojiSection::findEmojis(const vector<string>& words) const
{
    vector<const Emoji*> result{};
    for(const Emoji& emoji:emojis) {
        if(emoji.matches(words)) {
            result.push_back(&emoji);
        }
    }

    return result;
}

/*
 * Emojis
 */

vector<string> Emojis::toSearchWords(const string& expression)
{
    string lowerExpression{};
    stringToLower(expression, lowerExpression);

    vector<string> words{};
    for(const string& word:stringSplit(lowerExpression, "\\s+")) {
        if(!word.empty()) {
            words.push_back(word);
        }
    }

    return words;
}

Emojis::Emojis()
{
    // IMPROVE load emojis from the main MF configuration file

    EmojiSection emoji{"Emoji"};
    emoji.addEmoji("🎯", "direct hit", "target bullseye goal aim focus");
    emoji.addEmoji("🚀", "rocket", "launch start boost ship release fast");
    emoji.addEmoji("🌟", "glowing star", "enhancement feature shine sparkle new");
    emoji.addEmoji("🔧", "wrench", "tool fix repair configuration settings");
    emoji.addEmoji("🐞", "lady beetle", "bug defect fix error issue");
    emoji.addEmoji("🧪", "test tube", "test experiment laboratory science chemistry");
    emoji.addEmoji("📚", "books", "documentation docs library reading study");
    emoji.addEmoji("🔗", "link", "chain url reference hyperlink");
    emoji.addEmoji("⛑", "rescue worker helmet", "help safety support protection");
    emoji.addEmoji("🚧", "construction", "wip work in progress roadblock building");
    emoji.addEmoji("👁️", "eye", "see watch look review observe");
    emoji.addEmoji("🗑", "wastebasket", "trash delete remove bin garbage");
    emoji.addEmoji("❗", "exclamation mark", "important attention warning bang");
    emoji.addEmoji("❌", "cross mark", "no wrong error fail cancel");
    emoji.addEmoji("✔", "check mark", "ok yes done correct tick");
    emoji.addEmoji("📌", "pushpin", "pin important note mark");
    emoji.addEmoji("✂️", "scissors", "cut clip snip");
    emoji.addEmoji("📎", "paperclip", "attach attachment clip");
    emoji.addEmoji("📄", "page", "document file text sheet");
    emoji.addEmoji("📋", "clipboard", "copy paste list checklist");
    emoji.addEmoji("📝", "memo", "note write edit pencil draft");
    emoji.addEmoji("📅", "calendar", "date schedule day planning");
    emoji.addEmoji("📈", "chart increasing", "graph growth trend up statistics");
    emoji.addEmoji("🖼️", "framed picture", "image photo frame screenshot");
    emoji.addEmoji("🟢", "green circle", "ok done status good");
    emoji.addEmoji("🟠", "orange circle", "warning status");
    emoji.addEmoji("🔴", "red circle", "error blocked status stop");
    emoji.addEmoji("🟡", "yellow circle", "pending status");
    emoji.addEmoji("🔵", "blue circle", "info status");
    emoji.addEmoji("🟣", "purple circle", "status");
    emoji.addEmoji("🔮", "crystal ball", "future prediction magic forecast");
    emoji.addEmoji("♥", "heart", "love favorite like red");
    emoji.addEmoji("💙", "blue heart", "love favorite like");
    emoji.addEmoji("💛", "yellow heart", "love favorite like");
    emoji.addEmoji("💚", "green heart", "love favorite like");
    emoji.addEmoji("🚫", "prohibited", "no forbidden ban stop deny");
    emoji.addEmoji("⚙️", "gear", "settings configuration cog mechanics");
    emoji.addEmoji("🙂", "slightly smiling face", "smile happy");
    emoji.addEmoji("😃", "grinning face", "smile happy joy laugh");
    emoji.addEmoji("🥶", "cold face", "freezing frozen ice");
    emoji.addEmoji("🥰", "smiling face with hearts", "love adore affection");
    emoji.addEmoji("🐻", "bear", "animal teddy");
    emoji.addEmoji("🐸", "frog", "animal toad");
    emoji.addEmoji("🤖", "robot", "bot ai automation machine");
    emoji.addEmoji("💩", "pile of poo", "bad crap junk rubbish");
    emoji.addEmoji("👻", "ghost", "spooky halloween boo");
    emoji.addEmoji("🎉", "party popper", "celebration success release party");
    emoji.addEmoji("💣", "bomb", "explosive danger critical blast");
    emoji.addEmoji("☠️", "skull and crossbones", "death danger poison fatal");
    emoji.addEmoji("⚽", "soccer ball", "football sport game");
    emoji.addEmoji("💪", "flexed biceps", "strong muscle power effort");
    emoji.addEmoji("👍", "thumbs up", "like yes approve good agree");
    emoji.addEmoji("👎", "thumbs down", "dislike no reject bad disagree");
    emoji.addEmoji("🤞", "crossed fingers", "luck hope wish");
    emoji.addEmoji("🤙", "call me hand", "shaka cool hang loose");
    emoji.addEmoji("👌", "ok hand", "fine good perfect");
    emoji.addEmoji("🙏", "folded hands", "please thanks pray gratitude");
    emoji.addEmoji("🤦", "face palm", "facepalm disbelief frustration");
    emoji.addEmoji("🦑", "squid", "animal sea calamari");
    emoji.addEmoji("🐙", "octopus", "animal sea tentacles");
    emoji.addEmoji("👾", "alien monster", "game retro space invader");
    emoji.addEmoji("🐉", "dragon", "animal fantasy mythology");
    emoji.addEmoji("🛠", "hammer and wrench", "tools fix build maintenance");
    emoji.addEmoji("🔨", "hammer", "build tool fix nail");
    emoji.addEmoji("💎", "gem stone", "diamond valuable precious quality");
    emoji.addEmoji("🛡", "shield", "protection security defense guard");
    emoji.addEmoji("💥", "collision", "explosion boom crash impact");
    emoji.addEmoji("🔥", "fire", "hot burn trending urgent flame");
    emoji.addEmoji("🧬", "dna", "genetics biology science helix");
    emoji.addEmoji("🧙‍", "mage", "wizard magic sorcerer");
    emoji.addEmoji("🧠", "brain", "mind think knowledge intelligence memory");
    emoji.addEmoji("🔋", "battery", "power energy charge");
    emoji.addEmoji("⦀", "triple vertical bar", "separator divider delimiter");
    emoji.addEmoji("🛸", "flying saucer", "ufo alien space");
    emoji.addEmoji("📡", "satellite antenna", "signal broadcast communication network");
    emoji.addEmoji("🌊", "water wave", "sea ocean flow surf");
    emoji.addEmoji("🎖", "military medal", "award honor achievement decoration");
    emoji.addEmoji("🍔", "hamburger", "food burger fast food");
    emoji.addEmoji("🥋", "martial arts uniform", "karate judo sport kimono");
    emoji.addEmoji("💍", "ring", "wedding marriage jewel engagement");
    emoji.addEmoji("🥔", "potato", "food vegetable");
    emoji.addEmoji("🎨", "artist palette", "art design paint color");
    emoji.addEmoji("🌻", "sunflower", "flower plant summer");
    emoji.addEmoji("🌲", "evergreen tree", "tree forest nature wood");
    emoji.addEmoji("📣", "megaphone", "announce shout broadcast cheer");
    emoji.addEmoji("📢", "loudspeaker", "announce broadcast public address");
    emoji.addEmoji("🧲", "magnet", "attract magnetic pull");
    emoji.addEmoji("🏁", "chequered flag", "finish race end goal");
    emoji.addEmoji("🚩", "triangular flag", "flag mark milestone");
    emoji.addEmoji("💯", "hundred points", "perfect score full hundred");

    sections.push_back(emoji);

    // greek letters are named after the letter - capital and small letters
    // share the name, so that typing e.g. "alpha" offers both of them
    struct GreekLetter {
        const char* capital;
        const char* small;
        const char* name;
    };
    static const GreekLetter GREEK_ALPHABET[] = {
        {"Α", "α", "alpha"},   {"Β", "β", "beta"},    {"Γ", "γ", "gamma"},
        {"Δ", "δ", "delta"},   {"Ε", "ε", "epsilon"}, {"Ζ", "ζ", "zeta"},
        {"Η", "η", "eta"},     {"Θ", "θ", "theta"},   {"Ι", "ι", "iota"},
        {"Κ", "κ", "kappa"},   {"Λ", "λ", "lambda"},  {"Μ", "μ", "mu"},
        {"Ν", "ν", "nu"},      {"Ξ", "ξ", "xi"},      {"Ο", "ο", "omicron"},
        {"Π", "π", "pi"},      {"Ρ", "ρ", "rho"},     {"Σ", "σ", "sigma"},
        {"Τ", "τ", "tau"},     {"Υ", "υ", "upsilon"}, {"Φ", "φ", "phi"},
        {"Χ", "χ", "chi"},     {"Ψ", "ψ", "psi"},     {"Ω", "ω", "omega"}
    };

    EmojiSection greek{"Greek alphabet"};
    for(const GreekLetter& letter:GREEK_ALPHABET) {
        greek.addEmoji(
            letter.capital,
            string{"capital "}.append(letter.name),
            "greek letter uppercase");
        greek.addEmoji(
            letter.small,
            letter.name,
            "greek letter lowercase small");
        // sigma has an extra glyph which is used at the end of a word
        if(string{letter.name} == "sigma") {
            greek.addEmoji(
                "ς", "final sigma", "greek letter lowercase small word end");
        }
    }

    sections.push_back(greek);

    EmojiSection math{"Math and statistics"};
    math.addEmoji("x̄", "x bar", "mean average sample statistics");
    sections.push_back(math);

    EmojiSection physics{"Physics"};
    physics.addEmoji("°", "degree", "degrees temperature angle physics");
    sections.push_back(physics);
}

Emojis::~Emojis()
{
}

vector<const Emoji*> Emojis::findEmojis(const string& expression) const
{
    const vector<string> words = toSearchWords(expression);

    vector<const Emoji*> result{};
    for(const EmojiSection& section:sections) {
        const vector<const Emoji*> found = section.findEmojis(words);
        result.insert(result.end(), found.begin(), found.end());
    }

    return result;
}

} // m8r namespace
