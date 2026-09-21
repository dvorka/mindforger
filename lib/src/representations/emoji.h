/*
 emoji.h     MindForger thinking notebook

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
#ifndef M8R_EMOJI_H
#define M8R_EMOJI_H

#include <string>
#include <vector>

namespace m8r {

/**
 * @brief Emoji - a unicode character with a name and search keywords.
 *
 * The name and the keywords make the character findable by typing - without
 * them an emoji can be picked by eye (and mouse) only.
 */
class Emoji
{
private:
    std::string character;
    std::string name;
    std::string keywords;

    // lowercased "name keywords" which is searched by matches()
    std::string haystack;

public:
    explicit Emoji(
        const std::string& character,
        const std::string& name,
        const std::string& keywords);

    const std::string& getCharacter() const { return character; }
    const std::string& getName() const { return name; }
    const std::string& getKeywords() const { return keywords; }

    /**
     * @brief Does the emoji match ALL the given (lowercased) search words?
     *
     * Empty words match every emoji.
     */
    bool matches(const std::vector<std::string>& words) const;
};

/**
 * @brief Named section of emojis like "Emoji" or "Greek alphabet".
 */
class EmojiSection
{
private:
    std::string name;
    std::vector<Emoji> emojis;

public:
    explicit EmojiSection(const std::string& name);

    const std::string& getName() const { return name; }
    const std::vector<Emoji>& getEmojis() const { return emojis; }

    void addEmoji(
        const std::string& character,
        const std::string& name,
        const std::string& keywords);

    /**
     * @brief Emojis of this section matching ALL the given search words.
     */
    std::vector<const Emoji*> findEmojis(
        const std::vector<std::string>& words) const;
};

/**
 * @brief Catalog of emojis and special characters offered by MindForger.
 *
 * @see Unicode table: https://unicode-table.com/
 * @see Emojipedia: https://emojipedia.org/
 */
class Emojis
{
public:
    static Emojis& getInstance() {
        static Emojis SINGLETON{};
        return SINGLETON;
    }

    /**
     * @brief Split a search expression to lowercased, non-empty words.
     */
    static std::vector<std::string> toSearchWords(
        const std::string& expression);

private:
    std::vector<EmojiSection> sections;

    Emojis();

public:
    Emojis(const Emojis&) = delete;
    Emojis(const Emojis&&) = delete;
    Emojis& operator=(const Emojis&) = delete;
    Emojis& operator=(const Emojis&&) = delete;
    ~Emojis();

    const std::vector<EmojiSection>& getSections() const { return sections; }

    /**
     * @brief All emojis (of all sections) matching the search expression.
     *
     * Emojis are returned in the catalog order, an empty (or whitespace only)
     * expression matches all of them.
     */
    std::vector<const Emoji*> findEmojis(const std::string& expression) const;
};

}
#endif // M8R_EMOJI_H
