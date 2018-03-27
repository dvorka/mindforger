/**@addtogroup Stemming
@brief Library for stemming words down to their root words.
@date 2003-2015
@copyright Oleander Software, Ltd.
@author Oleander Software, Ltd.
@details This program is free software; you can redistribute it and/or modify
it under the terms of the BSD License.
* @{*/

#ifndef __STEM_H__
#define __STEM_H__

#include <algorithm>
#include "../indexing/string_util.h"
#include "../indexing/common_lang_constants.h"

///Namespace for stemming classes.
namespace stemming
    {
    enum stemming_type
        {
        no_stemming,
        danish,
        dutch,
        english,
        finnish,
        french,
        german,
        italian,
        norwegian,
        portuguese,
        spanish,
        swedish,
        STEMMING_TYPE_COUNT
        };
    //these characters should not appear in an indexed word
    const wchar_t UPPER_Y_HASH = 7;//bell
    const wchar_t LOWER_Y_HASH = 9;//tab
    const wchar_t UPPER_I_HASH = 10;//line feed
    const wchar_t LOWER_I_HASH = 11;//vertical tab
    const wchar_t UPPER_U_HASH = 12;//form feed (new page)
    const wchar_t LOWER_U_HASH = 13;//carriage return

    //language constants
    static const wchar_t FRENCH_VOWELS[] = { 97, 101, 105, 111, 117, 121, 0xE2,
        0xE0, 0xEB, 0xE9,
        0xEA, 0xE8, 0xEF,
        0xEE, 0xF4, 0xFB,
        0xF9, 65, 69, 73, 79, 85, 89, 0xC2,
        0xC0, 0xCB, 0xC9,
        0xCA, 0xC8, 0xCF,
        0xCE, 0xD4, 0xDB,
        0xD9, 0 };
    static const wchar_t FRENCH_ACCENTED_E[] = { 0xE9, 0xE8,
        0xC9, 0xC8, 0 };
    static const wchar_t FRENCH_AIOUES[] = { 97, 105, 111, 117, 0xE8, 115, 65, 73, 79, 85,
        0xC8, 83, 0 };

    static const wchar_t GERMAN_VOWELS[] = { 97, 101, 105, 111, 117, 0xFC, 121,
        0xE4, 0xF6, 65, 0xC4,
        69, 73, 79, 0xD6, 85, 0xDC, 89, 0 };

    static const wchar_t DANISH_VOWELS[] = { 97, 101, 105, 111, 117, 121, 0xE6,
        0xE5, 0xF8, 65, 69, 73, 79, 85, 89,
        0xC6, 0xC5, 0xD8, 0 };
    static const wchar_t DANISH_ALPHABET[] = { 97, 98, 99, 100, 102, 103, 104, 106, 107, 108, 109, 110, 111, 112, 114,
        116, 118, 121, 122, 0xE5, 65, 66, 67, 68, 70, 71, 72, 74, 75, 76, 77, 78, 79,
        80, 82, 84, 86, 89, 90, 0xC5, 0 };

    static const wchar_t FINNISH_VOWELS[] = { 97, 101, 105, 111, 117, 121, 0xE4, 0xF6, 65, 69, 73, 79, 85, 89,
        0xC4, 0xD6, 0 };
    static const wchar_t FINNISH_VOWELS_NO_Y[] = { 97, 101, 105, 111, 117, 0xE4, 0xF6, 65, 69, 73, 79, 85,
        0xC4, 0xD6, 0 };
    static const wchar_t FINNISH_VOWELS_SIMPLE[] = { 97, 101, 105, 0xE4, 65, 69, 73, 0xC4, 0 };
    static const wchar_t FINNISH_STEP_1_SUFFIX[] = { 110, 116, 97, 101, 105, 111, 117, 121, 0xE4,
        0xF6, 78, 84, 65, 69, 73, 79, 85, 89, 0xC4, 0xD6, 0 };

    static const wchar_t DUTCH_VOWELS[] = { 97, 101, 105, 111, 117, 121, 0xE8,
        65, 69, 73, 79, 85, 89, 0xC8, 0 };
    static const wchar_t DUTCH_KDT[] = { 107, 100, 116, 75, 68, 84, 0 };
    static const wchar_t DUTCH_S_ENDING[] = { 97, 101, 0xE8, 105, 111, 117, 121, 106, 65, 69,
        0xC8, 73, 79, 85, 89, 74, 0 };

    static const wchar_t NORWEGIAN_VOWELS[] = { 97, 101, 105, 111, 0xF8, 117, 121, 0xE5,
        0xE6, 0xC5, 65, 0xC6, 69, 73, 79,
        0xD8, 85, 89, 0 };
    static const wchar_t PORTUGUESE_VOWELS[] = { 97, 101, 105, 111, 117, 0xE1, 0xE9,
        0xED, 0xF3, 0xFA, 0xE2,
        0xEA, 0xF4, 65, 69, 73, 79, 85, 0xC1,
        0xC9, 0xCD, 0xD3, 0xDA,
        0xC2, 0xCA, 0xD4, 0 };
    static const wchar_t SPANISH_VOWELS[] = { 97, 101, 105, 111, 117, 0xE1, 0xE9,
        0xED, 0xF3, 0xFA, 0xFC,
        65, 69, 73, 79, 85, 0xC1, 0xC9, 0xCD,
        0xD3, 0xDA, 0xDC, 0 };

    static const wchar_t SWEDISH_VOWELS[] = { 97, 101, 105, 111, 117, 121, 0xE5,
        0xE4, 0xF6, 65, 69, 73, 79, 85, 89,
        0xC5, 0xC4, 0xD6, 0 };

    static const wchar_t ITALIAN_VOWELS[] = { 97, 101, 105, 111, 117, 0xE0,
        0xE8, 0xEC, 0xF2,
        0xF9, 65, 69, 73, 79, 85, 0xC0,
        0xC8, 0xCC, 0xD2,
        0xD9, 0 };
    static const wchar_t ITALIAN_VOWELS_SIMPLE[] = { 97, 101, 105, 111, 0xE0,
        0xE8, 0xEC, 0xF2,
        65, 69, 73, 79, 0xC0, 0xC8,
        0xCC, 0xD2, 0 };

    /**
       @class stem
       @brief The base class for language-specific stemmers.
       @details The template argument for the stemmers are the type
       of std::basic_string that you are trying to stem, by default std::wstring (Unicode strings).
       As long as the char type of your basic_string is wchar_t, then you can use any type of basic_string.
       This is to say, if your basic_string has a custom char_traits or allocator, then just specify it in
       your template argument to the stemmer.
       
       @par Example:
       \code
        typedef std::basic_string<wchar_t, myTraits, myAllocator> myString;
        myString word(L"documentation");
        stemming::english_stem<myString> StemEnglish;
        StemEnglish(word);
       \endcode */
    template <typename string_typeT = std::wstring>
    class stem
        {
    public:
        stem() : m_r1(0), m_r2(0), m_rv(0) {}
    protected:
        //R1, R2, RV functions
        void find_r1(const string_typeT& text,
                     const wchar_t* vowel_list)
            {
            //see where the R1 section begin
            //R1 is the region after the first consonant after the first vowel
            size_t start = text.find_first_of(vowel_list, 0);
            if (start == string_typeT::npos)
                {
                //we need at least need a vowel somewhere in the word
                m_r1 = text.length();
                return;
                }

            m_r1 = text.find_first_not_of(vowel_list,++start);
            if (get_r1() == string_typeT::npos)
                {
                m_r1 = text.length();
                }
            else
                {
                ++m_r1;
                }
            }

        void find_r2(const string_typeT& text,
                     const wchar_t* vowel_list)
            {
            size_t start = 0;
            //look for R2--not required for all criteria.
            //R2 is the region after the first consonant after the first vowel after R1
            if (get_r1() != text.length() )
                {
                start = text.find_first_of(vowel_list, get_r1());
                }
            else
                {
                start = string_typeT::npos;
                }
            if (start != string_typeT::npos &&
                static_cast<int>(start) != static_cast<int>(text.length())-1)
                {
                m_r2 = text.find_first_not_of(vowel_list,++start);
                if (get_r2() == string_typeT::npos)
                    {
                    m_r2 = text.length();
                    }
                else
                    {
                    ++m_r2;
                    }
                }
            else
                {
                m_r2 = text.length();
                }
            }

        void find_spanish_rv(const string_typeT& text,
                    const wchar_t* vowel_list)
            {
            //see where the RV section begin
            if (text.length() < 4)
                {
                m_rv = text.length();
                return;
                }
            //if second letter is a consonant
            if (!string_util::is_one_of(text[1], vowel_list) )
                {
                size_t start = text.find_first_of(vowel_list, 2);
                if (start == string_typeT::npos)
                    {
                    //can't find next vowel
                    m_rv = text.length();
                    return;
                    }
                else
                    {
                    m_rv = start+1;
                    }
                }
            //if first two letters are vowels
            else if (string_util::is_one_of(text[0], vowel_list) &&
                    string_util::is_one_of(text[1], vowel_list))
                {
                size_t start = text.find_first_not_of(vowel_list, 2);
                if (start == string_typeT::npos)
                    {
                    //can't find next consonant
                    m_rv = text.length();
                    return;
                    }
                else
                    {
                    m_rv = start+1;
                    }
                }
            //consonant/vowel at beginning
            else if (!string_util::is_one_of(text[0], vowel_list) &&
                    string_util::is_one_of(text[1], vowel_list))
                {
                m_rv = 3;
                }
            else
                {
                m_rv = text.length();
                }
            }

        /*If the word begins with two vowels, RV is the region after the third letter,
        otherwise the region after the first vowel not at the beginning of the word,
        or the end of the word if these positions cannot be found.
        (Exceptionally, par, col or tap, at the begining of a word is also taken to be the region before RV.)*/
        void find_french_rv(const string_typeT& text,
                    const wchar_t* vowel_list)
            {
            //see where the RV section begin
            if (text.length() < 3)
                {
                m_rv = text.length();
                return;
                }
            /*Exceptions: If the word begins with these then RV goes right after them,
            whether it be a letter or simply the end of the word.*/
            if (text.length() >= 3 &&
                ((is_either<wchar_t>(text[0], common_lang_constants::LOWER_P, common_lang_constants::UPPER_P) &&
                is_either<wchar_t>(text[1], common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) &&
                is_either<wchar_t>(text[2], common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) ) || //par

                (is_either<wchar_t>(text[0], common_lang_constants::LOWER_C, common_lang_constants::UPPER_C) &&
                is_either<wchar_t>(text[1], common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) &&
                is_either<wchar_t>(text[2], common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) ) || //col

                (is_either<wchar_t>(text[0], common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) &&
                is_either<wchar_t>(text[1], common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) &&
                is_either<wchar_t>(text[2], common_lang_constants::LOWER_P, common_lang_constants::UPPER_P) ))//tap
                )
                {
                m_rv = 3;
                return;
                }
            //if first two letters are vowels
            if (string_util::is_one_of(text[0], vowel_list) &&
                string_util::is_one_of(text[1], vowel_list))
                {
                m_rv = 3;
                }
            else
                {
                size_t start = text.find_first_not_of(vowel_list, 0);
                if (start == string_typeT::npos)
                    {
                    //can't find first consonant
                    m_rv = text.length();
                    return;
                    }
                start = text.find_first_of(vowel_list, start);
                if (start == string_typeT::npos)
                    {
                    //can't find first vowel
                    m_rv = text.length();
                    return;
                    }
                m_rv = start+1;
                }
            }

        void find_russian_rv(const string_typeT& text,
                    const wchar_t* vowel_list)
            {
            size_t start = text.find_first_of(vowel_list);
            if (start == string_typeT::npos)
                {
                //can't find first vowel
                m_rv = text.length();
                return;
                }
            else
                {
                m_rv = start+1;
                }
            }

        inline void update_r_sections(const string_typeT& text)
            {
            if (get_r1() > text.length() )
                {
                m_r1 = text.length();
                }
            if (get_r2() > text.length() )
                {
                m_r2 = text.length();
                }
            if (get_rv() > text.length() )
                {
                m_rv = text.length();
                }
            }
        /**Determines if a character is an apostrophe (includes straight single quotes).
        @param ch The letter to be analyzed.*/
        inline bool is_apostrophe(const wchar_t& ch) const
            {
            return (ch == 39) ? //'
                true : (ch == 146) ? //apostrophe
                true : (ch == 180) ? //apostrophe
                true : (ch == 0x2019) ? //right single apostrophe
                true : false;
            }
        //---------------------------------------------
        void trim_western_punctuation(string_typeT& text) const
            {
            if (text.length() >= 3 &&
                is_apostrophe(text[text.length()-3]) &&
                is_either<wchar_t>(text[text.length()-2], common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) &&
                is_apostrophe(text[text.length()-1]) )
                { text.erase(text.length()-3); }
            else if (text.length() >= 2 &&
                is_apostrophe(text[text.length()-2]) &&
                is_either<wchar_t>(text[text.length()-1], common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                { text.erase(text.length()-2); }
            else if (is_apostrophe(text[text.length()-1]))
                { text.erase(text.length()-1); }
            while (text.length() )
                {
                const wchar_t lastChar = text[text.length()-1];
                if (!(lastChar >= 48 && lastChar <= 57) &&
                    !(lastChar >= 65 && lastChar <= 90) &&
                    !(lastChar >= 97 && lastChar <= 122) &&
                    !(lastChar >= 192 && lastChar <= 246) &&
                    !(lastChar >= 248 && lastChar <= 255) &&
                    lastChar != 0xA0)//space
                    {
                    text.erase(text.length()-1);
                    }
                else
                    { break; }
                }
            while (text.length() )
                {
                if (!(text[0] >= 48 && text[0] <= 57) &&
                    !(text[0] >= 65 && text[0] <= 90) &&
                    !(text[0] >= 97 && text[0] <= 122) &&
                    !(text[0] >= 192 && text[0] <= 246) &&
                    !(text[0] >= 248 && text[0] <= 255) )
                    { text.erase(0, 1); }
                else
                    { break; }
                }
            }

        //suffix removal determinant functions
        ///is_suffix for one character
        inline bool is_suffix(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U) const
            {
            if (text.length() < 1)
                { return false; }
            return is_either<wchar_t>(text[text.length()-1], suffix1L, suffix1U);
            }
        ///is_suffix for two characters
        inline bool is_suffix(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U) const
            {
            if (text.length() < 2)
                {
                return false;
                }
            return is_either<wchar_t>(text[text.length()-2], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix2L, suffix2U);
            }

        ///is_suffix for three characters
        inline bool is_suffix(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U) const
            {
            if (text.length() < 3)
                {
                return false;
                }
            return is_either<wchar_t>(text[text.length()-3], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-2], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix3L, suffix3U);
            }
        ///is_suffix for four characters
        inline bool is_suffix(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U) const
            {
            if (text.length() < 4)
                {
                return false;
                }
            return is_either<wchar_t>(text[text.length()-4], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-3], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[text.length()-2], suffix3L, suffix3U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix4L, suffix4U);
            }
        ///is_suffix for five characters
        inline bool is_suffix(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U) const
            {
            if (text.length() < 5)
                {
                return false;
                }
            return is_either<wchar_t>(text[text.length()-5], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-4], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[text.length()-3], suffix3L, suffix3U) &&
                    is_either<wchar_t>(text[text.length()-2], suffix4L, suffix4U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix5L, suffix5U);
            }
        ///is_suffix for six characters
        inline bool is_suffix(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U,
                    const wchar_t suffix6L, const wchar_t suffix6U) const
            {
            if (text.length() < 6)
                {
                return false;
                }
            return is_either<wchar_t>(text[text.length()-6], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-5], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[text.length()-4], suffix3L, suffix3U) &&
                    is_either<wchar_t>(text[text.length()-3], suffix4L, suffix4U) &&
                    is_either<wchar_t>(text[text.length()-2], suffix5L, suffix5U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix6L, suffix6U);
            }
        ///is_suffix for seven characters
        inline bool is_suffix(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U,
                    const wchar_t suffix6L, const wchar_t suffix6U,
                    const wchar_t suffix7L, const wchar_t suffix7U) const
            {
            if (text.length() < 7)
                {
                return false;
                }
            return is_either<wchar_t>(text[text.length()-7], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-6], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[text.length()-5], suffix3L, suffix3U) &&
                    is_either<wchar_t>(text[text.length()-4], suffix4L, suffix4U) &&
                    is_either<wchar_t>(text[text.length()-3], suffix5L, suffix5U) &&
                    is_either<wchar_t>(text[text.length()-2], suffix6L, suffix6U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix7L, suffix7U);
            }
        ///is_suffix for eight characters
        inline bool is_suffix(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U,
                    const wchar_t suffix6L, const wchar_t suffix6U,
                    const wchar_t suffix7L, const wchar_t suffix7U,
                    const wchar_t suffix8L, const wchar_t suffix8U) const
            {
            if (text.length() < 8)
                {
                return false;
                }
            return is_either<wchar_t>(text[text.length()-8], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-7], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[text.length()-6], suffix3L, suffix3U) &&
                    is_either<wchar_t>(text[text.length()-5], suffix4L, suffix4U) &&
                    is_either<wchar_t>(text[text.length()-4], suffix5L, suffix5U) &&
                    is_either<wchar_t>(text[text.length()-3], suffix6L, suffix6U) &&
                    is_either<wchar_t>(text[text.length()-2], suffix7L, suffix7U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix8L, suffix8U);
            }
        ///is_suffix for nine characters
        inline bool is_suffix(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U,
                    const wchar_t suffix6L, const wchar_t suffix6U,
                    const wchar_t suffix7L, const wchar_t suffix7U,
                    const wchar_t suffix8L, const wchar_t suffix8U,
                    const wchar_t suffix9L, const wchar_t suffix9U) const
            {
            if (text.length() < 9)
                {
                return false;
                }
            return is_either<wchar_t>(text[text.length()-9], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-8], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[text.length()-7], suffix3L, suffix3U) &&
                    is_either<wchar_t>(text[text.length()-6], suffix4L, suffix4U) &&
                    is_either<wchar_t>(text[text.length()-5], suffix5L, suffix5U) &&
                    is_either<wchar_t>(text[text.length()-4], suffix6L, suffix6U) &&
                    is_either<wchar_t>(text[text.length()-3], suffix7L, suffix7U) &&
                    is_either<wchar_t>(text[text.length()-2], suffix8L, suffix8U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix9L, suffix9U);
            }

        ///comparison for two characters
        inline bool is_partial_suffix(const string_typeT& text,
                    const size_t start_index,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U)
            {
            if ((start_index+2) >= text.length())
                { return false; }
            return (is_either<wchar_t>(text[start_index], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[start_index+1], suffix2L, suffix2U));
            }
        ///comparison for three characters
        inline bool is_partial_suffix(const string_typeT& text,
                    const size_t start_index,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U)
            {
            if ((start_index+3) >= text.length())
                { return false; }
            return (is_either<wchar_t>(text[start_index], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[start_index+1], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[start_index+2], suffix3L, suffix3U));
            }

        ///RV suffix functions
        //-------------------------------------------------
        ///RV suffix comparison for one character
        inline bool is_suffix_in_rv(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U)
            {
            if (text.length() < 1)
                {
                return false;
                }
            return (is_either<wchar_t>(text[text.length()-1], suffix1L, suffix1U) &&
                    (get_rv() <= text.length()-1) );
            }
        ///RV suffix comparison for two characters
        inline bool is_suffix_in_rv(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U)
            {
            if (text.length() < 2)
                {
                return false;
                }
            return ((is_either<wchar_t>(text[text.length()-2], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix2L, suffix2U) ) &&
                    (get_rv() <= text.length()-2) );
            }
        ///RV suffix comparison for three characters
        inline bool is_suffix_in_rv(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U)
            {
            if (text.length() < 3)
                {
                return false;
                }
            return ((is_either<wchar_t>(text[text.length()-3], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-2], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix3L, suffix3U) ) &&
                    (get_rv() <= text.length()-3) );
            }
        ///RV suffix comparison for four characters
        inline bool is_suffix_in_rv(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U)
            {
            if (text.length() < 4)
                {
                return false;
                }
            return ((is_either<wchar_t>(text[text.length()-4], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-3], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[text.length()-2], suffix3L, suffix3U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix4L, suffix4U) ) &&
                    (get_rv() <= text.length()-4) );
            }
        ///RV suffix comparison for five characters
        inline bool is_suffix_in_rv(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U)
            {
            if (text.length() < 5)
                {
                return false;
                }
            return ((is_either<wchar_t>(text[text.length()-5], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-4], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[text.length()-3], suffix3L, suffix3U) &&
                    is_either<wchar_t>(text[text.length()-2], suffix4L, suffix4U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix5L, suffix5U) ) &&
                    (get_rv() <= text.length()-5) );
            }
        ///RV suffix comparison for six characters
        inline bool is_suffix_in_rv(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U,
                    const wchar_t suffix6L, const wchar_t suffix6U)
            {
            if (text.length() < 6)
                {
                return false;
                }
            return ((is_either<wchar_t>(text[text.length()-6], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-5], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[text.length()-4], suffix3L, suffix3U) &&
                    is_either<wchar_t>(text[text.length()-3], suffix4L, suffix4U) &&
                    is_either<wchar_t>(text[text.length()-2], suffix5L, suffix5U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix6L, suffix6U) ) &&
                    (get_rv() <= text.length()-6) );
            }
        ///RV suffix comparison for seven characters
        inline bool is_suffix_in_rv(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U,
                    const wchar_t suffix6L, const wchar_t suffix6U,
                    const wchar_t suffix7L, const wchar_t suffix7U)
            {
            if (text.length() < 7)
                {
                return false;
                }
            return ((is_either<wchar_t>(text[text.length()-7], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-6], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[text.length()-5], suffix3L, suffix3U) &&
                    is_either<wchar_t>(text[text.length()-4], suffix4L, suffix4U) &&
                    is_either<wchar_t>(text[text.length()-3], suffix5L, suffix5U) &&
                    is_either<wchar_t>(text[text.length()-2], suffix6L, suffix6U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix7L, suffix7U) ) &&
                    (get_rv() <= text.length()-7) );
            }
        ///RV suffix comparison for eight characters
        inline bool is_suffix_in_rv(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U,
                    const wchar_t suffix6L, const wchar_t suffix6U,
                    const wchar_t suffix7L, const wchar_t suffix7U,
                    const wchar_t suffix8L, const wchar_t suffix8U)
            {
            if (text.length() < 8)
                {
                return false;
                }
            return ((is_either<wchar_t>(text[text.length()-8], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-7], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[text.length()-6], suffix3L, suffix3U) &&
                    is_either<wchar_t>(text[text.length()-5], suffix4L, suffix4U) &&
                    is_either<wchar_t>(text[text.length()-4], suffix5L, suffix5U) &&
                    is_either<wchar_t>(text[text.length()-3], suffix6L, suffix6U) &&
                    is_either<wchar_t>(text[text.length()-2], suffix7L, suffix7U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix8L, suffix8U) ) &&
                    (get_rv() <= text.length()-8) );
            }

        ///R1 suffix functions
        //-------------------------------------------------
        ///R1 suffix comparison for one character
        inline bool is_suffix_in_r1(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U)
            {
            if (text.length() < 1)
                {
                return false;
                }
            return (is_either<wchar_t>(text[text.length()-1], suffix1L, suffix1U) &&
                    (get_r1() <= text.length()-1) );
            }
        ///R1 suffix comparison for two characters
        inline bool is_suffix_in_r1(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U)
            {
            if (text.length() < 2)
                {
                return false;
                }
            return ((is_either<wchar_t>(text[text.length()-2], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix2L, suffix2U) ) &&
                    (get_r1() <= text.length()-2) );
            }
        ///R1 suffix comparison for three characters
        inline bool is_suffix_in_r1(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U)
            {
            if (text.length() < 3)
                {
                return false;
                }
            return ((is_either<wchar_t>(text[text.length()-3], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-2], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix3L, suffix3U) ) &&
                    (get_r1() <= text.length()-3) );
            }
        ///R1 suffix comparison for four characters
        inline bool is_suffix_in_r1(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U)
            {
            if (text.length() < 4)
                {
                return false;
                }
            return ((is_either<wchar_t>(text[text.length()-4], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-3], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[text.length()-2], suffix3L, suffix3U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix4L, suffix4U) ) &&
                    (get_r1() <= text.length()-4) );
            }
        ///R1 suffix comparison for five characters
        inline bool is_suffix_in_r1(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U)
            {
            if (text.length() < 5)
                {
                return false;
                }
            return ((is_either<wchar_t>(text[text.length()-5], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-4], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[text.length()-3], suffix3L, suffix3U) &&
                    is_either<wchar_t>(text[text.length()-2], suffix4L, suffix4U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix5L, suffix5U) ) &&
                    (get_r1() <= text.length()-5) );
            }
        ///R1 suffix comparison for six characters
        inline bool is_suffix_in_r1(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U,
                    const wchar_t suffix6L, const wchar_t suffix6U)
            {
            if (text.length() < 6)
                {
                return false;
                }
            return ((is_either<wchar_t>(text[text.length()-6], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-5], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[text.length()-4], suffix3L, suffix3U) &&
                    is_either<wchar_t>(text[text.length()-3], suffix4L, suffix4U) &&
                    is_either<wchar_t>(text[text.length()-2], suffix5L, suffix5U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix6L, suffix6U) ) &&
                    (get_r1() <= text.length()-6) );
            }

        ///R2 suffix functions
        //-------------------------------------------------
        ///R2 suffix comparison for one character
        inline bool is_suffix_in_r2(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U)
            {
            if (text.length() < 1)
                {
                return false;
                }
            return (is_either<wchar_t>(text[text.length()-1], suffix1L, suffix1U) &&
                    (get_r2() <= text.length()-1) );
            }
        ///R2 suffix comparison for two characters
        inline bool is_suffix_in_r2(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U)
            {
            if (text.length() < 2)
                {
                return false;
                }
            return ((is_either<wchar_t>(text[text.length()-2], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix2L, suffix2U) ) &&
                    (get_r2() <= text.length()-2) );
            }
        ///R2 suffix comparison for three characters
        inline bool is_suffix_in_r2(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U)
            {
            if (text.length() < 3)
                {
                return false;
                }
            return ((is_either<wchar_t>(text[text.length()-3], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-2], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix3L, suffix3U) ) &&
                    (get_r2() <= text.length()-3) );
            }
        ///R2 suffix comparison for four characters
        inline bool is_suffix_in_r2(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U)
            {
            if (text.length() < 4)
                {
                return false;
                }
            return ((is_either<wchar_t>(text[text.length()-4], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-3], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[text.length()-2], suffix3L, suffix3U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix4L, suffix4U) ) &&
                    (get_r2() <= text.length()-4) );
            }
        ///R2 suffix comparison for five characters
        inline bool is_suffix_in_r2(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U)
            {
            if (text.length() < 5)
                {
                return false;
                }
            return ((is_either<wchar_t>(text[text.length()-5], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-4], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[text.length()-3], suffix3L, suffix3U) &&
                    is_either<wchar_t>(text[text.length()-2], suffix4L, suffix4U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix5L, suffix5U) ) &&
                    (get_r2() <= text.length()-5) );
            }
        ///R2 suffix comparison for six characters
        inline bool is_suffix_in_r2(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U,
                    const wchar_t suffix6L, const wchar_t suffix6U)
            {
            if (text.length() < 6)
                {
                return false;
                }
            return ((is_either<wchar_t>(text[text.length()-6], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-5], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[text.length()-4], suffix3L, suffix3U) &&
                    is_either<wchar_t>(text[text.length()-3], suffix4L, suffix4U) &&
                    is_either<wchar_t>(text[text.length()-2], suffix5L, suffix5U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix6L, suffix6U) ) &&
                    (get_r2() <= text.length()-6) );
            }
        ///R2 suffix comparison for seven characters
        inline bool is_suffix_in_r2(const string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U,
                    const wchar_t suffix6L, const wchar_t suffix6U,
                    const wchar_t suffix7L, const wchar_t suffix7U)
            {
            if (text.length() < 7)
                {
                return false;
                }
            return ((is_either<wchar_t>(text[text.length()-7], suffix1L, suffix1U) &&
                    is_either<wchar_t>(text[text.length()-6], suffix2L, suffix2U) &&
                    is_either<wchar_t>(text[text.length()-5], suffix3L, suffix3U) &&
                    is_either<wchar_t>(text[text.length()-4], suffix4L, suffix4U) &&
                    is_either<wchar_t>(text[text.length()-3], suffix5L, suffix5U) &&
                    is_either<wchar_t>(text[text.length()-2], suffix6L, suffix6U) &&
                    is_either<wchar_t>(text[text.length()-1], suffix7L, suffix7U) ) &&
                    (get_r2() <= text.length()-7) );
            }

        //suffix removal functions
        //R1 deletion for one character suffix
        inline bool delete_if_is_in_r1(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const bool success_on_find = true)
            {
            assert(suffix1L == string_util::tolower_western(suffix1U) );
            if (text.length() < 1)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-1], suffix1L, suffix1U))
                {
                if (get_r1() <= text.length()-1)
                    {
                    text.erase(text.length()-1);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }
        //R1 deletion for two character suffix
        inline bool delete_if_is_in_r1(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const bool success_on_find = true)
            {
            if (text.length() < 2)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-2], suffix1L, suffix1U) &&
                is_either<wchar_t>(text[text.length()-1], suffix2L, suffix2U))
                {
                if (get_r1() <= text.length()-2)
                    {
                    text.erase(text.length()-2);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }
        //R1 deletion for three character suffix
        inline bool delete_if_is_in_r1(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const bool success_on_find = true)
            {
            if (text.length() < 3)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-3], suffix1L, suffix1U) &&
                is_either<wchar_t>(text[text.length()-2], suffix2L, suffix2U) &&
                is_either<wchar_t>(text[text.length()-1], suffix3L, suffix3U) )
                {
                if (get_r1() <= text.length()-3)
                    {
                    text.erase(text.length()-3);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }
        //R1 deletion for four character suffix
        inline bool delete_if_is_in_r1(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const bool success_on_find = true)
            {
            if (text.length() < 4)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-4], suffix1L, suffix1U) &&
                is_either<wchar_t>(text[text.length()-3], suffix2L, suffix2U) &&
                is_either<wchar_t>(text[text.length()-2], suffix3L, suffix3U) &&
                is_either<wchar_t>(text[text.length()-1], suffix4L, suffix4U) )
                {
                if (get_r1() <= text.length()-4)
                    {
                    text.erase(text.length()-4);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }
        //R1 deletion for five character suffix
        inline bool delete_if_is_in_r1(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U,
                    const bool success_on_find = true)
            {
            if (text.length() < 5)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-5], suffix1L, suffix1U) &&
                is_either<wchar_t>(text[text.length()-4], suffix2L, suffix2U) &&
                is_either<wchar_t>(text[text.length()-3], suffix3L, suffix3U) &&
                is_either<wchar_t>(text[text.length()-2], suffix4L, suffix4U) &&
                is_either<wchar_t>(text[text.length()-1], suffix5L, suffix5U) )
                {
                if (get_r1() <= text.length()-5)
                    {
                    text.erase(text.length()-5);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }
        //R1 deletion for six character suffix
        inline bool delete_if_is_in_r1(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U,
                    const wchar_t suffix6L, const wchar_t suffix6U,
                    const bool success_on_find = true)
            {
            if (text.length() < 6)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-6], suffix1L, suffix1U) &&
                is_either<wchar_t>(text[text.length()-5], suffix2L, suffix2U) &&
                is_either<wchar_t>(text[text.length()-4], suffix3L, suffix3U) &&
                is_either<wchar_t>(text[text.length()-3], suffix4L, suffix4U) &&
                is_either<wchar_t>(text[text.length()-2], suffix5L, suffix5U) &&
                is_either<wchar_t>(text[text.length()-1], suffix6L, suffix6U) )
                {
                if (get_r1() <= text.length()-6)
                    {
                    text.erase(text.length()-6);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }
        //R1 deletion for seven character suffix
        inline bool delete_if_is_in_r1(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U,
                    const wchar_t suffix6L, const wchar_t suffix6U,
                    const wchar_t suffix7L, const wchar_t suffix7U,
                    const bool success_on_find = true)
            {
            if (text.length() < 7)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-7], suffix1L, suffix1U) &&
                is_either<wchar_t>(text[text.length()-6], suffix2L, suffix2U) &&
                is_either<wchar_t>(text[text.length()-5], suffix3L, suffix3U) &&
                is_either<wchar_t>(text[text.length()-4], suffix4L, suffix4U) &&
                is_either<wchar_t>(text[text.length()-3], suffix5L, suffix5U) &&
                is_either<wchar_t>(text[text.length()-2], suffix6L, suffix6U) &&
                is_either<wchar_t>(text[text.length()-1], suffix7L, suffix7U) )
                {
                if (get_r1() <= text.length()-7)
                    {
                    text.erase(text.length()-7);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }

        //R2 deletion functions
        //R2 deletion for one character suffix
        inline bool delete_if_is_in_r2(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const bool success_on_find = true)
            {
            if (text.length() < 1)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-1], suffix1L, suffix1U))
                {
                if (get_r2() <= text.length()-1)
                    {
                    text.erase(text.length()-1);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }
        //R2 deletion for two character suffix
        inline bool delete_if_is_in_r2(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const bool success_on_find = true)
            {
            if (text.length() < 2)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-2], suffix1L, suffix1U) &&
                is_either<wchar_t>(text[text.length()-1], suffix2L, suffix2U))
                {
                if (get_r2() <= text.length()-2)
                    {
                    text.erase(text.length()-2);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }
        //R2 deletion for three character suffix
        inline bool delete_if_is_in_r2(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const bool success_on_find = true)
            {
            if (text.length() < 3)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-3], suffix1L, suffix1U) &&
                is_either<wchar_t>(text[text.length()-2], suffix2L, suffix2U) &&
                is_either<wchar_t>(text[text.length()-1], suffix3L, suffix3U) )
                {
                if (get_r2() <= text.length()-3)
                    {
                    text.erase(text.length()-3);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }
        //R2 deletion for four character suffix
        inline bool delete_if_is_in_r2(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const bool success_on_find = true)
            {
            if (text.length() < 4)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-4], suffix1L, suffix1U) &&
                is_either<wchar_t>(text[text.length()-3], suffix2L, suffix2U) &&
                is_either<wchar_t>(text[text.length()-2], suffix3L, suffix3U) &&
                is_either<wchar_t>(text[text.length()-1], suffix4L, suffix4U) )
                {
                if (get_r2() <= text.length()-4)
                    {
                    text.erase(text.length()-4);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }
        ///R2 deletion for five character suffix
        inline bool delete_if_is_in_r2(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U,
                    const bool success_on_find = true)
            {
            if (text.length() < 5)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-5], suffix1L, suffix1U) &&
                is_either<wchar_t>(text[text.length()-4], suffix2L, suffix2U) &&
                is_either<wchar_t>(text[text.length()-3], suffix3L, suffix3U) &&
                is_either<wchar_t>(text[text.length()-2], suffix4L, suffix4U) &&
                is_either<wchar_t>(text[text.length()-1], suffix5L, suffix5U) )
                {
                if (get_r2() <= text.length()-5)
                    {
                    text.erase(text.length()-5);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }
        ///R2 deletion for six character suffix
        inline bool delete_if_is_in_r2(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U,
                    const wchar_t suffix6L, const wchar_t suffix6U,
                    const bool success_on_find = true)
            {
            if (text.length() < 6)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-6], suffix1L, suffix1U) &&
                is_either<wchar_t>(text[text.length()-5], suffix2L, suffix2U) &&
                is_either<wchar_t>(text[text.length()-4], suffix3L, suffix3U) &&
                is_either<wchar_t>(text[text.length()-3], suffix4L, suffix4U) &&
                is_either<wchar_t>(text[text.length()-2], suffix5L, suffix5U) &&
                is_either<wchar_t>(text[text.length()-1], suffix6L, suffix6U) )
                {
                if (get_r2() <= text.length()-6)
                    {
                    text.erase(text.length()-6);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }
        ///R2 deletion for seven character suffix
        inline bool delete_if_is_in_r2(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U,
                    const wchar_t suffix6L, const wchar_t suffix6U,
                    const wchar_t suffix7L, const wchar_t suffix7U,
                    const bool success_on_find = true)
            {
            if (text.length() < 7)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-7], suffix1L, suffix1U) &&
                is_either<wchar_t>(text[text.length()-6], suffix2L, suffix2U) &&
                is_either<wchar_t>(text[text.length()-5], suffix3L, suffix3U) &&
                is_either<wchar_t>(text[text.length()-4], suffix4L, suffix4U) &&
                is_either<wchar_t>(text[text.length()-3], suffix5L, suffix5U) &&
                is_either<wchar_t>(text[text.length()-2], suffix6L, suffix6U) &&
                is_either<wchar_t>(text[text.length()-1], suffix7L, suffix7U) )
                {
                if (get_r2() <= text.length()-7)
                    {
                    text.erase(text.length()-7);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }
        ///R2 deletion for eight character suffix
        inline bool delete_if_is_in_r2(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U,
                    const wchar_t suffix6L, const wchar_t suffix6U,
                    const wchar_t suffix7L, const wchar_t suffix7U,
                    const wchar_t suffix8L, const wchar_t suffix8U,
                    const bool success_on_find = true)
            {
            if (text.length() < 8)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-8], suffix1L, suffix1U) &&
                is_either<wchar_t>(text[text.length()-7], suffix2L, suffix2U) &&
                is_either<wchar_t>(text[text.length()-6], suffix3L, suffix3U) &&
                is_either<wchar_t>(text[text.length()-5], suffix4L, suffix4U) &&
                is_either<wchar_t>(text[text.length()-4], suffix5L, suffix5U) &&
                is_either<wchar_t>(text[text.length()-3], suffix6L, suffix6U) &&
                is_either<wchar_t>(text[text.length()-2], suffix7L, suffix7U) &&
                is_either<wchar_t>(text[text.length()-1], suffix8L, suffix8U) )
                {
                if (get_r2() <= text.length()-8)
                    {
                    text.erase(text.length()-8);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }
        
        //RV deletion functions
        //RV deletion for one character suffix
        inline bool delete_if_is_in_rv(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const bool success_on_find = true)
            {
            if (text.length() < 1)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-1], suffix1L, suffix1U))
                {
                if (get_rv() <= text.length()-1)
                    {
                    text.erase(text.length()-1);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }
        //RV deletion for two character suffix
        inline bool delete_if_is_in_rv(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const bool success_on_find = true)
            {
            if (text.length() < 2)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-2], suffix1L, suffix1U) &&
                is_either<wchar_t>(text[text.length()-1], suffix2L, suffix2U))
                {
                if (get_rv() <= text.length()-2)
                    {
                    text.erase(text.length()-2);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }
        //RV deletion for three character suffix
        inline bool delete_if_is_in_rv(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const bool success_on_find = true)
            {
            if (text.length() < 3)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-3], suffix1L, suffix1U) &&
                is_either<wchar_t>(text[text.length()-2], suffix2L, suffix2U) &&
                is_either<wchar_t>(text[text.length()-1], suffix3L, suffix3U) )
                {
                if (get_rv() <= text.length()-3)
                    {
                    text.erase(text.length()-3);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }
        //RV deletion for four character suffix
        inline bool delete_if_is_in_rv(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const bool success_on_find = true)
            {
            if (text.length() < 4)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-4], suffix1L, suffix1U) &&
                is_either<wchar_t>(text[text.length()-3], suffix2L, suffix2U) &&
                is_either<wchar_t>(text[text.length()-2], suffix3L, suffix3U) &&
                is_either<wchar_t>(text[text.length()-1], suffix4L, suffix4U) )
                {
                if (get_rv() <= text.length()-4)
                    {
                    text.erase(text.length()-4);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }
        //RV deletion for five character suffix
        inline bool delete_if_is_in_rv(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U,
                    const bool success_on_find = true)
            {
            if (text.length() < 5)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-5], suffix1L, suffix1U) &&
                is_either<wchar_t>(text[text.length()-4], suffix2L, suffix2U) &&
                is_either<wchar_t>(text[text.length()-3], suffix3L, suffix3U) &&
                is_either<wchar_t>(text[text.length()-2], suffix4L, suffix4U) &&
                is_either<wchar_t>(text[text.length()-1], suffix5L, suffix5U) )
                {
                if (get_rv() <= text.length()-5)
                    {
                    text.erase(text.length()-5);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }
        //RV deletion for six character suffix
        inline bool delete_if_is_in_rv(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U,
                    const wchar_t suffix6L, const wchar_t suffix6U,
                    const bool success_on_find = true)
            {
            if (text.length() < 6)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-6], suffix1L, suffix1U) &&
                is_either<wchar_t>(text[text.length()-5], suffix2L, suffix2U) &&
                is_either<wchar_t>(text[text.length()-4], suffix3L, suffix3U) &&
                is_either<wchar_t>(text[text.length()-3], suffix4L, suffix4U) &&
                is_either<wchar_t>(text[text.length()-2], suffix5L, suffix5U) &&
                is_either<wchar_t>(text[text.length()-1], suffix6L, suffix6U) )
                {
                if (get_rv() <= text.length()-6)
                    {
                    text.erase(text.length()-6);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }
        //RV deletion for seven character suffix
        inline bool delete_if_is_in_rv(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U,
                    const wchar_t suffix6L, const wchar_t suffix6U,
                    const wchar_t suffix7L, const wchar_t suffix7U,
                    const bool success_on_find = true)
            {
            if (text.length() < 7)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-7], suffix1L, suffix1U) &&
                is_either<wchar_t>(text[text.length()-6], suffix2L, suffix2U) &&
                is_either<wchar_t>(text[text.length()-5], suffix3L, suffix3U) &&
                is_either<wchar_t>(text[text.length()-4], suffix4L, suffix4U) &&
                is_either<wchar_t>(text[text.length()-3], suffix5L, suffix5U) &&
                is_either<wchar_t>(text[text.length()-2], suffix6L, suffix6U) &&
                is_either<wchar_t>(text[text.length()-1], suffix7L, suffix7U) )
                {
                if (get_rv() <= text.length()-7)
                    {
                    text.erase(text.length()-7);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }
        //RV deletion for eight character suffix
        inline bool delete_if_is_in_rv(string_typeT& text,
                    const wchar_t suffix1L, const wchar_t suffix1U,
                    const wchar_t suffix2L, const wchar_t suffix2U,
                    const wchar_t suffix3L, const wchar_t suffix3U,
                    const wchar_t suffix4L, const wchar_t suffix4U,
                    const wchar_t suffix5L, const wchar_t suffix5U,
                    const wchar_t suffix6L, const wchar_t suffix6U,
                    const wchar_t suffix7L, const wchar_t suffix7U,
                    const wchar_t suffix8L, const wchar_t suffix8U,
                    const bool success_on_find = true)
            {
            if (text.length() < 8)
                {
                return false;
                }
            if (is_either<wchar_t>(text[text.length()-8], suffix1L, suffix1U) &&
                is_either<wchar_t>(text[text.length()-7], suffix2L, suffix2U) &&
                is_either<wchar_t>(text[text.length()-6], suffix3L, suffix3U) &&
                is_either<wchar_t>(text[text.length()-5], suffix4L, suffix4U) &&
                is_either<wchar_t>(text[text.length()-4], suffix5L, suffix5U) &&
                is_either<wchar_t>(text[text.length()-3], suffix6L, suffix6U) &&
                is_either<wchar_t>(text[text.length()-2], suffix7L, suffix7U) &&
                is_either<wchar_t>(text[text.length()-1], suffix8L, suffix8U) )
                {
                if (get_rv() <= text.length()-8)
                    {
                    text.erase(text.length()-8);
                    update_r_sections(text);
                    return true;
                    }
                return success_on_find;
                }
            else
                {
                return false;
                }
            }

        //string support functions
        void remove_german_umlauts(string_typeT& text)
            {
            for (size_t i = 0; i < text.length(); ++i)
                {
                if (text[i] == 0xC4)
                    {
                    text[i] = common_lang_constants::UPPER_A;
                    }
                else if (text[i] == 0xD6)
                    {
                    text[i] = common_lang_constants::UPPER_O;
                    }
                else if (text[i] == 0xDC)
                    {
                    text[i] = common_lang_constants::UPPER_U;
                    }
                else if (text[i] == 0xE4 )
                    {
                    text[i] = common_lang_constants::LOWER_A;
                    }
                else if (text[i] == 0xF6)
                    {
                    text[i] = common_lang_constants::LOWER_O;
                    }
                else if (text[i] == 0xFC)
                    {
                    text[i] = common_lang_constants::LOWER_U;
                    }
                }
            }
        void italian_acutes_to_graves(string_typeT& text)
            {
            for (size_t i = 0; i < text.length(); ++i)
                {
                if (text[i] == 0xC1)//A acute
                    {
                    text[i] = 0xC0;
                    }
                else if (text[i] == 0xC9)//E acute
                    {
                    text[i] = 0xC8;
                    }
                else if (text[i] == 0xCD)//I acute
                    {
                    text[i] = 0xCC;
                    }
                else if (text[i] == 0xD3)//O acute
                    {
                    text[i] = 0xD2;
                    }
                else if (text[i] == 0xDA)//U acute
                    {
                    text[i] = 0xD9;
                    }
                else if (text[i] == 0xE1)//a acute
                    {
                    text[i] = 0xE0;
                    }
                else if (text[i] == 0xE9)//e acute
                    {
                    text[i] = 0xE8;
                    }
                else if (text[i] == 0xED)//i acute
                    {
                    text[i] = 0xEC;
                    }
                else if (text[i] == 0xF3)//o acute
                    {
                    text[i] = 0xF2;
                    }
                else if (text[i] == 0xFA)//u acute
                    {
                    text[i] = 0xF9;
                    }
                }
            }
        ///Hash initial y, y after a vowel, and i between vowels into hashed character.
        //----------------------------------------------------------
        void hash_dutch_yi(string_typeT& text,
                    const wchar_t* vowel_string)
            {
            //need at least 2 letters for hashing
            if (text.length() < 2)
                { return; }

            if (text[0] == common_lang_constants::LOWER_Y)
                {
                text[0] = LOWER_Y_HASH;
                }
            else if (text[0] == common_lang_constants::UPPER_Y)
                {
                text[0] = UPPER_Y_HASH;
                }
            bool in_vowel_block = string_util::is_one_of(text[0], vowel_string);

            size_t i = 1;
            for (i = 1; i < text.length()-1; ++i)
                {
                if (in_vowel_block &&
                    text[i] == common_lang_constants::LOWER_I &&
                    string_util::is_one_of(text[i+1], vowel_string) )
                    {
                    text[i] = LOWER_I_HASH;
                    in_vowel_block = false;
                    }
                else if (in_vowel_block &&
                    text[i] == common_lang_constants::UPPER_I &&
                    string_util::is_one_of(text[i+1], vowel_string) )
                    {
                    text[i] = UPPER_I_HASH;
                    in_vowel_block = false;
                    }
                else if (in_vowel_block &&
                    text[i] == common_lang_constants::LOWER_Y)
                    {
                    text[i] = LOWER_Y_HASH;
                    in_vowel_block = false;
                    }
                else if (in_vowel_block &&
                    text[i] == common_lang_constants::UPPER_Y)
                    {
                    text[i] = UPPER_Y_HASH;
                    in_vowel_block = false;
                    }
                else if (string_util::is_one_of(text[i], vowel_string) )
                    {
                    in_vowel_block = true;
                    }
                else
                    {
                    in_vowel_block = false;
                    }
                }
            //check the last letter
            if (in_vowel_block &&
                text[i] == common_lang_constants::LOWER_Y)
                {
                text[i] = LOWER_Y_HASH;
                in_vowel_block = false;
                }
            else if (in_vowel_block &&
                text[i] == common_lang_constants::UPPER_Y)
                {
                text[i] = UPPER_Y_HASH;
                in_vowel_block = false;
                }
            }

        //----------------------------------------------------------
        inline void unhash_dutch_yi(string_typeT& text)
            {
            string_util::replace_all<wchar_t, string_typeT >(text, LOWER_Y_HASH, common_lang_constants::LOWER_Y);
            string_util::replace_all<wchar_t, string_typeT >(text, UPPER_Y_HASH, common_lang_constants::UPPER_Y);
            string_util::replace_all<wchar_t, string_typeT >(text, LOWER_I_HASH, common_lang_constants::LOWER_I);
            string_util::replace_all<wchar_t, string_typeT >(text, UPPER_I_HASH, common_lang_constants::UPPER_I);
            }

        ///Hash 'u' and 'y' between vowels
        //----------------------------------------------------------
        void hash_german_yu(string_typeT& text,
                    const wchar_t* vowel_string)
            {
            //need at least 2 letters for hashing
            if (text.length() < 2)
                { return; }

            bool in_vowel_block = string_util::is_one_of(text[0], vowel_string);

            for (size_t i = 1; i < text.length()-1; ++i)
                {
                if (in_vowel_block &&
                    string_util::is_one_of(text[i], vowel_string) &&
                    string_util::is_one_of(text[i+1], vowel_string) )
                    {
                    if (text[i] == common_lang_constants::LOWER_Y)
                        {
                        text[i] = LOWER_Y_HASH;
                        }
                    else if (text[i] == common_lang_constants::UPPER_Y)
                        {
                        text[i] = UPPER_Y_HASH;
                        }
                    else if (text[i] == common_lang_constants::LOWER_U)
                        {
                        text[i] = LOWER_U_HASH;
                        }
                    else if (text[i] == common_lang_constants::UPPER_U)
                        {
                        text[i] = UPPER_U_HASH;
                        }
                    }
                else if (string_util::is_one_of(text[i], vowel_string) )
                    {
                    in_vowel_block = true;
                    }
                else
                    {
                    in_vowel_block = false;
                    }
                }
            //hashable values must be between vowels, so don't bother looking at last letter
            }

        //----------------------------------------------------------
        inline void unhash_german_yu(string_typeT& text)
            {
            string_util::replace_all<wchar_t, string_typeT >(text, LOWER_Y_HASH, common_lang_constants::LOWER_Y);
            string_util::replace_all<wchar_t, string_typeT >(text, UPPER_Y_HASH, common_lang_constants::UPPER_Y);
            string_util::replace_all<wchar_t, string_typeT >(text, LOWER_U_HASH, common_lang_constants::LOWER_U);
            string_util::replace_all<wchar_t, string_typeT >(text, UPPER_U_HASH, common_lang_constants::UPPER_U);
            }

        /**Hash u or i preceded and followed by a vowel, and y preceded or followed by a vowel.
        u after q is also hashed. For example,
        jouer         ->         joUer
        ennuie         ->         ennuIe
        yeux         ->         Yeux
        quand         ->         qUand*/
        //----------------------------------------------------------
        void hash_french_yui(string_typeT& text,
                    const wchar_t* vowel_string)
            {
            //need at least 2 letters for hashing
            if (text.length() < 2)
                { return; }

            bool in_vowel_block = false;

            //start loop at zero because 'y' at start of string can be hashed
            size_t i = 0;
            for (i = 0; i < text.length()-1; ++i)
                {
                if (in_vowel_block &&
                    string_util::is_one_of(text[i], vowel_string) &&
                    string_util::is_one_of(text[i+1], vowel_string) )
                    {
                    if (text[i] == common_lang_constants::LOWER_Y)
                        {
                        text[i] = LOWER_Y_HASH;
                        in_vowel_block = false;
                        }
                    else if (text[i] == common_lang_constants::UPPER_Y)
                        {
                        text[i] = UPPER_Y_HASH;
                        in_vowel_block = false;
                        }
                    else if (text[i] == common_lang_constants::LOWER_U)
                        {
                        text[i] = LOWER_U_HASH;
                        in_vowel_block = false;
                        }
                    else if (text[i] == common_lang_constants::UPPER_U)
                        {
                        text[i] = UPPER_U_HASH;
                        in_vowel_block = false;
                        }
                    else if (text[i] == common_lang_constants::LOWER_I)
                        {
                        text[i] = LOWER_I_HASH;
                        in_vowel_block = false;
                        }
                    else if (text[i] == common_lang_constants::UPPER_I)
                        {
                        text[i] = UPPER_I_HASH;
                        in_vowel_block = false;
                        }
                    }
                //if just previous letter is a vowel then examine for 'y'
                else if (in_vowel_block &&
                        text[i] == common_lang_constants::LOWER_Y)
                    {
                    text[i] = LOWER_Y_HASH;
                    in_vowel_block = false;
                    }
                else if (in_vowel_block &&
                        text[i] == common_lang_constants::UPPER_Y)
                    {
                    text[i] = UPPER_Y_HASH;
                    in_vowel_block = false;
                    }
                //if just following letter is a vowel then examine for 'y'
                else if (text[i] == common_lang_constants::LOWER_Y &&
                        string_util::is_one_of(text[i+1], vowel_string) &&
                        is_neither<wchar_t>(text[i+1], common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y) )
                    {
                    text[i] = LOWER_Y_HASH;
                    in_vowel_block = false;
                    }
                else if (text[i] == common_lang_constants::UPPER_Y &&
                        string_util::is_one_of(text[i+1], vowel_string) &&
                        is_neither<wchar_t>(text[i+1], common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y) )
                    {
                    text[i] = UPPER_Y_HASH;
                    in_vowel_block = false;
                    }
                else if (string_util::is_one_of(text[i], vowel_string) )
                    {
                    if (text[i] == common_lang_constants::LOWER_U &&
                        (i > 0) &&
                        is_either<wchar_t>(text[i-1], common_lang_constants::LOWER_Q, common_lang_constants::UPPER_Q) )
                        {
                        text[i] = LOWER_U_HASH;
                        in_vowel_block = false;
                        }
                    else if (text[i] == common_lang_constants::UPPER_U &&
                        (i > 0) &&
                        is_either<wchar_t>(text[i-1], common_lang_constants::LOWER_Q, common_lang_constants::UPPER_Q) )
                        {
                        text[i] = UPPER_U_HASH;
                        in_vowel_block = false;
                        }
                    else
                        {
                        in_vowel_block = true;
                        }
                    }
                else
                    {
                    in_vowel_block = false;
                    }
                }
            //verify that the last letter
            if (text[i] == common_lang_constants::LOWER_Y &&
                (i > 0) &&
                string_util::is_one_of(text[i-1], vowel_string)    )
                {
                text[i] = LOWER_Y_HASH;
                }
            else if (text[i] == common_lang_constants::UPPER_Y &&
                    (i > 0) &&
                    string_util::is_one_of(text[i-1], vowel_string) )
                {
                text[i] = UPPER_Y_HASH;
                }
            else if (text[i] == common_lang_constants::LOWER_U &&
                    (i > 0) &&
                    is_either<wchar_t>(text[i-1], common_lang_constants::LOWER_Q, common_lang_constants::UPPER_Q) )
                {
                text[i] = LOWER_U_HASH;
                }
            else if (text[i] == common_lang_constants::UPPER_U &&
                    (i > 0) &&
                    is_either<wchar_t>(text[i-1], common_lang_constants::LOWER_Q, common_lang_constants::UPPER_Q) )
                {
                text[i] = UPPER_U_HASH;
                }
            }

        void unhash_french_yui(string_typeT& text)
            {
            string_util::replace_all<wchar_t, string_typeT >(text, LOWER_Y_HASH, common_lang_constants::LOWER_Y);
            string_util::replace_all<wchar_t, string_typeT >(text, UPPER_Y_HASH, common_lang_constants::UPPER_Y);
            string_util::replace_all<wchar_t, string_typeT >(text, LOWER_U_HASH, common_lang_constants::LOWER_U);
            string_util::replace_all<wchar_t, string_typeT >(text, UPPER_U_HASH, common_lang_constants::UPPER_U);
            string_util::replace_all<wchar_t, string_typeT >(text, LOWER_I_HASH, common_lang_constants::LOWER_I);
            string_util::replace_all<wchar_t, string_typeT >(text, UPPER_I_HASH, common_lang_constants::UPPER_I);
            }

        //----------------------------------------------------------
        void hash_y(string_typeT& text,
                    const wchar_t* vowel_string)
            {
            //need at least 2 letters for hashing
            if (text.length() < 2)
                { return; }

            //if first letter is a 'y', then it is likely not a vowel
            if (text[0] == common_lang_constants::LOWER_Y)
                {
                text[0] = LOWER_Y_HASH;
                }
            else if (text[0] == common_lang_constants::UPPER_Y)
                {
                text[0] = UPPER_Y_HASH;
                }

            bool in_vowel_block = string_util::is_one_of(text[0], vowel_string);

            for (size_t i = 1; i < text.length(); ++i)
                {
                //LOWER_Y after vowel is a consonant
                if (in_vowel_block &&
                    text[i] == common_lang_constants::LOWER_Y)
                    {
                    text[i] = LOWER_Y_HASH;
                    in_vowel_block = false;
                    }
                else if (in_vowel_block &&
                    text[i] == common_lang_constants::UPPER_Y)
                    {
                    text[i] = UPPER_Y_HASH;
                    in_vowel_block = false;
                    }
                else if (string_util::is_one_of(text[i], vowel_string) )
                    {
                    in_vowel_block = true;
                    }
                //we are on a consonant
                else
                    {
                    in_vowel_block = false;
                    }
                }
            }

        //----------------------------------------------------------
        inline void unhash_y(string_typeT& text)
            {
            string_util::replace_all<wchar_t, string_typeT >(text, LOWER_Y_HASH, common_lang_constants::LOWER_Y);
            string_util::replace_all<wchar_t, string_typeT >(text, UPPER_Y_HASH, common_lang_constants::UPPER_Y);
            }

        ///Hash u after q, and u, i between vowels
        //----------------------------------------------------------
        void hash_italian_ui(string_typeT& text,
                    const wchar_t* vowel_string)
            {
            //need at least 2 letters for hashing
            if (text.length() < 2)
                { return; }

            bool in_vowel_block = string_util::is_one_of(text[0], vowel_string);

            size_t i = 1;
            for (i = 1; i < text.length()-1; ++i)
                {
                if (in_vowel_block &&
                    string_util::is_one_of(text[i], vowel_string) &&
                    string_util::is_one_of(text[i+1], vowel_string) )
                    {
                    if (text[i] == common_lang_constants::LOWER_I )
                        {
                        text[i] = LOWER_I_HASH;
                        }
                    else if (text[i] == common_lang_constants::UPPER_I )
                        {
                        text[i] = UPPER_I_HASH;
                        }
                    else if (text[i] == common_lang_constants::LOWER_U)
                        {
                        text[i] = LOWER_U_HASH;
                        }
                    else if (text[i] == common_lang_constants::UPPER_U)
                        {
                        text[i] = UPPER_U_HASH;
                        }
                    }
                else if (string_util::is_one_of(text[i], vowel_string) )
                    {
                    /*u after q should be encrypted and not be
                    treated as a vowel*/
                    if (text[i] == common_lang_constants::LOWER_U &&
                        (i > 0) &&
                        is_either<wchar_t>(text[i-1], common_lang_constants::LOWER_Q, common_lang_constants::UPPER_Q) )
                        {
                        text[i] = LOWER_U_HASH;
                        in_vowel_block = false;
                        }
                    else if (text[i] == common_lang_constants::UPPER_U &&
                        (i > 0) &&
                        is_either<wchar_t>(text[i-1], common_lang_constants::LOWER_Q, common_lang_constants::UPPER_Q) )
                        {
                        text[i] = UPPER_U_HASH;
                        in_vowel_block = false;
                        }
                    else
                        {
                        in_vowel_block = true;
                        }
                    }
                //we are on a consonant
                else
                    {
                    in_vowel_block = false;
                    }
                }
            //verify the last letter
            if (text[i] == common_lang_constants::LOWER_U &&
                (i > 0) &&
                is_either<wchar_t>(text[i-1], common_lang_constants::LOWER_Q, common_lang_constants::UPPER_Q) )
                {
                text[i] = LOWER_U_HASH;
                }
            else if (text[i] == common_lang_constants::UPPER_U &&
                (i > 0) &&
                is_either<wchar_t>(text[i-1], common_lang_constants::LOWER_Q, common_lang_constants::UPPER_Q) )
                {
                text[i] = UPPER_U_HASH;
                }
            }

        //----------------------------------------------------------
        inline void unhash_italian_ui(string_typeT& text)
            {
            string_util::replace_all<wchar_t, string_typeT >(text, LOWER_I_HASH, common_lang_constants::LOWER_I);
            string_util::replace_all<wchar_t, string_typeT >(text, UPPER_I_HASH, common_lang_constants::UPPER_I);
            string_util::replace_all<wchar_t, string_typeT >(text, LOWER_U_HASH, common_lang_constants::LOWER_U);
            string_util::replace_all<wchar_t, string_typeT >(text, UPPER_U_HASH, common_lang_constants::UPPER_U);
            }

        //----------------------------------------------------------
        void remove_dutch_umlauts(string_typeT& text)
            {
            for (size_t i = 0; i < text.length(); ++i)
                {
                if (text[i] == 0xC4)
                    {
                    text[i] = common_lang_constants::UPPER_A;
                    }
                else if (text[i] == 0xCB)
                    {
                    text[i] = common_lang_constants::UPPER_E;
                    }
                else if (text[i] == 0xCF)
                    {
                    text[i] = common_lang_constants::UPPER_I;
                    }
                else if (text[i] == 0xD6)
                    {
                    text[i] = common_lang_constants::UPPER_O;
                    }
                else if (text[i] == 0xDC)
                    {
                    text[i] = common_lang_constants::UPPER_U;
                    }
                else if (text[i] == 0xE4)
                    {
                    text[i] = common_lang_constants::LOWER_A;
                    }
                else if (text[i] == 0xEB)
                    {
                    text[i] = common_lang_constants::LOWER_E;
                    }
                else if (text[i] == 0xEF)
                    {
                    text[i] = common_lang_constants::LOWER_I;
                    }
                else if (text[i] == 0xF6)
                    {
                    text[i] = common_lang_constants::LOWER_O;
                    }
                else if (text[i] == 0xFC)
                    {
                    text[i] = common_lang_constants::LOWER_U;
                    }
                }
            }

        //----------------------------------------------------------
        void remove_dutch_acutes(string_typeT& text)
            {
            for (size_t i = 0; i < text.length(); ++i)
                {
                if (text[i] == 0xC1)
                    {
                    text[i] = common_lang_constants::UPPER_A;
                    }
                else if (text[i] == 0xC9)
                    {
                    text[i] = common_lang_constants::UPPER_E;
                    }
                else if (text[i] == 0xCD)
                    {
                    text[i] = common_lang_constants::UPPER_I;
                    }
                else if (text[i] == 0xD3)
                    {
                    text[i] = common_lang_constants::UPPER_O;
                    }
                else if (text[i] == 0xDA)
                    {
                    text[i] = common_lang_constants::UPPER_U;
                    }
                else if (text[i] == 0xE1)
                    {
                    text[i] = common_lang_constants::LOWER_A;
                    }
                else if (text[i] == 0xE9)
                    {
                    text[i] = common_lang_constants::LOWER_E;
                    }
                else if (text[i] == 0xED)
                    {
                    text[i] = common_lang_constants::LOWER_I;
                    }
                else if (text[i] == 0xF3)
                    {
                    text[i] = common_lang_constants::LOWER_O;
                    }
                else if (text[i] == 0xFA)
                    {
                    text[i] = common_lang_constants::LOWER_U;
                    }
                }
            }

        //----------------------------------------------------------
        void remove_spanish_acutes(string_typeT& text)
            {
            for (size_t i = 0; i < text.length(); ++i)
                {
                if (text[i] == 0xC1)
                    {
                    text[i] = common_lang_constants::UPPER_A;
                    }
                else if (text[i] == 0xC9)
                    {
                    text[i] = common_lang_constants::UPPER_E;
                    }
                else if (text[i] == 0xCD)
                    {
                    text[i] = common_lang_constants::UPPER_I;
                    }
                else if (text[i] == 0xD3)
                    {
                    text[i] = common_lang_constants::UPPER_O;
                    }
                else if (text[i] == 0xDA)
                    {
                    text[i] = common_lang_constants::UPPER_U;
                    }
                else if (text[i] == 0xE1)
                    {
                    text[i] = common_lang_constants::LOWER_A;
                    }
                else if (text[i] == 0xE9)
                    {
                    text[i] = common_lang_constants::LOWER_E;
                    }
                else if (text[i] == 0xED)
                    {
                    text[i] = common_lang_constants::LOWER_I;
                    }
                else if (text[i] == 0xF3)
                    {
                    text[i] = common_lang_constants::LOWER_O;
                    }
                else if (text[i] == 0xFA)
                    {
                    text[i] = common_lang_constants::LOWER_U;
                    }
                }
            }

        inline size_t get_r1() const
            { return m_r1; }
        inline void set_r1(const size_t val)
            { m_r1 = val; }

        inline size_t get_r2() const
            { return m_r2; }
        inline void set_r2(const size_t val)
            { m_r2 = val; }

        inline size_t get_rv() const
            { return m_rv; }
        inline void set_rv(const size_t val)
            { m_rv = val; }

        void reset_r_values()
            { m_r1 = m_r2 = m_rv = 0; }
    private:
        size_t m_r1;
        size_t m_r2;
        //only used for romance/russian languages
        size_t m_rv;
        };

    //------------------------------------------------------
    /*A non-operational stemmer that is used in place of regular stemmers when
    you don't want the system to actually stem anything.*/
    template <typename string_typeT = std::wstring>
    class no_op_stem
        {
    public:
        ///No-op stemming of declared string type
        inline void operator()(const string_typeT&) const
            {}
        ///No-op stemming of flexible string type
        template <typename T>
        inline void operator()(const T&) const
            {}
        };
    }

/** @}*/

#endif //__STEM_H__
