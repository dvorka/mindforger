/**@addtogroup Stemming
@brief Library for stemming words down to their root words.
@date 2003-2015
@copyright Oleander Software, Ltd.
@author Oleander Software, Ltd.
@details This program is free software; you can redistribute it and/or modify
it under the terms of the BSD License.
* @{*/

#ifndef __ENGLISH_STEM_H__
#define __ENGLISH_STEM_H__

#include "stemming.h"

namespace stemming
    {
    /**
    @brief English stemmer.

    @par Overview:

    I have made more than one attempt to improve the structure of the Porter algorithm 
    by making it follow the pattern of ending removal of the Romance language stemmers.
    It is not hard to see why one should want to do this: step 1b of the Porter stemmer 
    removes ed and ing, which are i-suffixes (*) attached to verbs. If these suffixes are
    removed, there should be no need to remove d-suffixes which are not verbal, although it
    will try to do so. This seems to be a deficiency in the Porter stemmer, not shared by
    the Romance stemmers. Again, the divisions between steps 2, 3 and 4 seem rather arbitrary,
    and are not found in the Romance stemmers.

    Nevertheless, these attempts at improvement have been abandoned. They seem to lead to a
    more complicated algorithm with no very obvious improvements. A reason for not taking
    note of the outcome of step 1b may be that English endings do not determine word categories
    quite as strongly as endings in the Romance languages. For example, condition and position
    in French have to be nouns, but in English they can be verbs as well as nouns,

    We are all conditioned by advertising
    They are positioning themselves differently today

    A possible reason for having separate steps 2, 3 and 4 is that d-suffix combinations in
    English are quite complex, a point which has been made elsewhere.

    But it is hardly surprising that after twenty years of use of the Porter stemmer, certain
    improvements do suggest themselves, and a new algorithm for English is therefore offered
    here. (It could be called the 'Porter2' stemmer to distinguish it from the Porter stemmer,
    from which it derives.) The changes are not so very extensive: (1) terminating y is changed
    to i rather less often, (2) suffix us does not lose its s, (3) a few additional suffixes
    are included for removal, including (4) suffix ly. In addition, a small list of exceptional
    forms is included. In December 2001 there were two further adjustments: (5) Steps 5a and 5b
    of the old Porter stemmer were combined into a single step. This means that undoubling final
    ll is not done with removal of final e. (6) In Step 3 ative is removed only when in region R2. 

    To begin with, here is the basic algorithm without reference to the exceptional forms.
    An exact comparison with the Porter algorithm needs to be done quite carefully if done at
    all. Here we indicate by * points of departure, and by + additional features.
    In the sample vocabulary, Porter and Porter2 stem slightly under 5% of words to different forms.

    Dr. Martin Porter

    Define a vowel as one of
        - a e i o u y 

    Define a double as one of
        - bb dd ff gg mm nn pp rr tt 

    Define a valid li-ending as one of
        - c d e g h k m n r t 

    Define a short syllable in a word as either (a) a vowel followed by a non-vowel
    other than w, x or Y and preceded by a non-vowel, or * (b) a vowel at the beginning
    of the word followed by a non-vowel.

    So rap, trap, entrap end with a short syllable, and ow, on, at are classed as short syllables.
    But uproot, bestow, disturb do not end with a short syllable.

    A word is called short if it consists of a short syllable preceded by zero or more consonants.
    R1 is the region after the first non-vowel following a vowel, or the end of the word if there is no such non-vowel.
    R2 is the region after the first non-vowel following a vowel in R1, or the end of the word if there is no such non-vowel.
    If the word has two letters or less, leave it as it is.
    Otherwise, do each of the following operations,
    Set initial y, or y after a vowel, to Y, and then establish the regions R1 and R2.
    
    @par Algorithm:

    <b>Step 1a:</b>

    Search for the longest among the following suffixes, and perform the action indicated:
            - sses
                - Replace by ss.
            - ied+ ies*
                - Replace by i if preceded by just one letter, otherwise by ie (so ties -> tie, cries -> cri).
            - s
                - Delete if the preceding word part contains a vowel not immediately before the s (so gas and this retain the s, gaps and kiwis lose it).
            - us+ ss
                - Do nothing.

    <b>Step 1b:</b>

    Search for the longest among the following suffixes, and perform the action indicated:
            - eed eedly+
                - Replace by ee if in R1.
            - ed edly+ ing ingly+
                - Delete if the preceding word part contains a vowel, and then 
                - If the word ends at, bl or iz add e (so luxuriat -> luxuriate), or 
                - If the word ends with a double remove the last letter (so hopp -> hop), or 
                - If the word is short, add e (so hop -> hope).

    <b>Step 1c:</b>

    Replace suffix y or Y by i if preceded by a non-vowel which is
    not the first letter of the word (so cry -> cri, by -> by, say -> say)

    <b>Step 2:</b>

    Search for the longest among the following suffixes, and, if found and in R1, perform the action indicated:
            - tional
                - Replace by tion.
            - enci
                - Replace by ence.
            - anci
                - Replace by ance 
            - abli
                - Replace by able.
            - entli
                - Replace by ent.
            - izer   ization
                - Replace by ize.
            - ational   ation   ator
                - Replace by ate.
            - alism   aliti   alli
                - Replace by al.
            - fulness
                - Replace by ful.
            - ousli   ousness
                - Replace by ous.
            - iveness   iviti
                - Replace by ive.
            - biliti   bli+
                - Replace by ble. 
            - ogi+
                - Replace by og if preceded by l.
            - fulli+
                - Replace by ful.
            - lessli+
                - Replace by less.
            - li+
                - Delete if preceded by a valid li-ending.

    <b>Step 3:</b>

    Search for the longest among the following suffixes, and, if found and in R1, perform the action indicated:
            - tional+
                - Replace by tion.
            - ational+
                - Replace by ate.
            - alize
                - Replace by al.
            - icate iciti   ical
                - Replace by ic.
            - ful ness
                - Delete.
            - ative*
                - Delete if in R2.

    <b>Step 4:</b>

    Search for the longest among the following suffixes, and, if found and in R2, perform the action indicated:
            - al ance ence er ic able ible ant ement ment ent ism ate iti ous ive ize 
                - Delete 
            - ion 
                - Delete if preceded by s or t.

    <b>Step 5:</b>

    Search for the following suffixes, and, if found, perform the action indicated:
            - e 
                - Delete if in R2, or in R1 and not preceded by a short syllable.
            - l 
                - Delete if in R2 and preceded by l.
    */
    /** @} */
    //------------------------------------------------------
    template <typename string_typeT = std::wstring>
    class english_stem : public stem<string_typeT>
        {
    public:
        english_stem() : m_first_vowel(string_typeT::npos)
            {}
        //---------------------------------------------
        /**@param[in,out] text English string to stem.*/
        void operator()(string_typeT& text)
            {
            if (text.length() < 3)
                {
                return;
                }

            //reset internal data
            m_first_vowel = string_typeT::npos;
            stem<string_typeT>::reset_r_values();

            stem<string_typeT>::trim_western_punctuation(text);

            //handle exceptions first
            if (is_exception(text) )
                {
                return;
                }

            stem<string_typeT>::hash_y(text, L"aeiouyAEIOUY");
            m_first_vowel = text.find_first_of(L"aeiouyAEIOUY");
            if (m_first_vowel == string_typeT::npos)
                { return; }

            if (text.length() >= 5 &&
                /*gener*/
                (is_either<wchar_t>(text[0], common_lang_constants::LOWER_G, common_lang_constants::UPPER_G) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[4], common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) ) )
                {
                stem<string_typeT>::set_r1(5);
                }
            else if (text.length() >= 6 &&
                /*commun*/
                (is_either<wchar_t>(text[0], common_lang_constants::LOWER_C, common_lang_constants::UPPER_C) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_M, common_lang_constants::UPPER_M) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_M, common_lang_constants::UPPER_M) &&
                    is_either<wchar_t>(text[4], common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) &&
                    is_either<wchar_t>(text[5], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) ) )
                {
                stem<string_typeT>::set_r1(6);
                }
            else if (text.length() >= 5 &&
                /*arsen*/
                (is_either<wchar_t>(text[0], common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[4], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) ) )
                {
                stem<string_typeT>::set_r1(5);
                }
            else
                {
                stem<string_typeT>::find_r1(text, L"aeiouyAEIOUY");
                }

            stem<string_typeT>::find_r2(text, L"aeiouyAEIOUY");

            //step 1a:
            step_1a(text);
            //exception #2
            if (is_exception_post_step1a(text) )
                {
                return;
                }
            //step 1b:
            step_1b(text);
            //step 1c:
            step_1c(text);
            //step 2:
            step_2(text);
            //step 3:
            step_3(text);
            //step 4:
            step_4(text);
            //step 5:
            step_5(text);

            stem<string_typeT>::unhash_y(text);
            }
    private:
        //---------------------------------------------
        bool is_exception(string_typeT& text)
            {
            //exception #0
            /*skis*/
            if (text.length() == 4 &&
                is_either<wchar_t>(text[0], common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) &&
                is_either<wchar_t>(text[1], common_lang_constants::LOWER_K, common_lang_constants::UPPER_K) &&
                is_either<wchar_t>(text[2], common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) &&
                is_either<wchar_t>(text[3], common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                text = L"ski";
                return true;
                }
            /*skies*/
            else if (text.length() == 5 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_K, common_lang_constants::UPPER_K) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[4], common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                text = L"sky";
                return true;
                }
            /*dying*/
            else if (text.length() == 5 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[4], common_lang_constants::LOWER_G, common_lang_constants::UPPER_G) )
                {
                text = L"die";
                return true;
                }
            /*lying*/
            else if (text.length() == 5 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[4], common_lang_constants::LOWER_G, common_lang_constants::UPPER_G) )
                {
                text = L"lie";
                return true;
                }
            /*tying*/
            else if (text.length() == 5 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[4], common_lang_constants::LOWER_G, common_lang_constants::UPPER_G) )
                {
                text = L"tie";
                return true;
                }
            /*idly*/
            else if (text.length() == 4 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y) )
                {
                text = L"idl";
                return true;
                }
            /*gently*/
            else if (text.length() == 6 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_G, common_lang_constants::UPPER_G) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) &&
                    is_either<wchar_t>(text[4], common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) &&
                    is_either<wchar_t>(text[5], common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y) )
                {
                text = L"gentl";
                return true;
                }
            /*ugly*/
            else if (text.length() == 4 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_G, common_lang_constants::UPPER_G) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y) )
                {
                text = L"ugli";
                return true;
                }
            /*early*/
            else if (text.length() == 5 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) &&
                    is_either<wchar_t>(text[4], common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y) )
                {
                text = L"earli";
                return true;
                }
            /*only*/
            else if (text.length() == 4 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y) )
                {
                text = L"onli";
                return true;
                }
            /*singly*/
            else if (text.length() == 6 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_G, common_lang_constants::UPPER_G) &&
                    is_either<wchar_t>(text[4], common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) &&
                    is_either<wchar_t>(text[5], common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y) )
                {
                text = L"singl";
                return true;
                }
            //exception #1
            else if (
                /*sky*/
                (text.length() == 3 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_K, common_lang_constants::UPPER_K) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y) ) ||
                /*news*/
                (text.length() == 4 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_W, common_lang_constants::UPPER_W) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) ) ||
                /*howe*/
                (text.length() == 4 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_H, common_lang_constants::UPPER_H) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_W, common_lang_constants::UPPER_W) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ) ||
                /*atlas*/
                (text.length() == 5 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) &&
                    is_either<wchar_t>(text[4], common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) ) ||
                /*cosmos*/
                (text.length() == 6 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_C, common_lang_constants::UPPER_C) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_M, common_lang_constants::UPPER_M) &&
                    is_either<wchar_t>(text[4], common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) &&
                    is_either<wchar_t>(text[5], common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) ) ||
                /*bias*/
                (text.length() == 4 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_B, common_lang_constants::UPPER_B) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) ) ||
                /*andes*/
                (text.length() == 5 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[4], common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) ) )
                {
                return true;
                }
            return false;
            }

        //---------------------------------------------
        bool is_exception_post_step1a(string_typeT& text)
            {
            //exception #2
            if (/*inning*/
                (text.length() == 6 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) &&
                    is_either<wchar_t>(text[4], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[5], common_lang_constants::LOWER_G, common_lang_constants::UPPER_G) ) ||
                /*outing*/
                (text.length() == 6 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) &&
                    is_either<wchar_t>(text[4], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[5], common_lang_constants::LOWER_G, common_lang_constants::UPPER_G) ) ||
                /*canning*/
                (text.length() == 7 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_C, common_lang_constants::UPPER_C) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[4], common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) &&
                    is_either<wchar_t>(text[5], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[6], common_lang_constants::LOWER_G, common_lang_constants::UPPER_G) ) ||
                /*herring*/
                (text.length() == 7 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_H, common_lang_constants::UPPER_H) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) &&
                    is_either<wchar_t>(text[4], common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) &&
                    is_either<wchar_t>(text[5], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[6], common_lang_constants::LOWER_G, common_lang_constants::UPPER_G) ) ||
                /*earring*/
                (text.length() == 7 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) &&
                    is_either<wchar_t>(text[4], common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) &&
                    is_either<wchar_t>(text[5], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[6], common_lang_constants::LOWER_G, common_lang_constants::UPPER_G) ) ||
                /*proceed*/
                (text.length() == 7 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_P, common_lang_constants::UPPER_P) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_C, common_lang_constants::UPPER_C) &&
                    is_either<wchar_t>(text[4], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[5], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[6], common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) ) ||
                /*exceed*/
                (text.length() == 6 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_X, common_lang_constants::UPPER_X) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_C, common_lang_constants::UPPER_C) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[4], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[5], common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) ) ||
                /*succeed*/
                (text.length() == 7 &&
                    is_either<wchar_t>(text[0], common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) &&
                    is_either<wchar_t>(text[1], common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) &&
                    is_either<wchar_t>(text[2], common_lang_constants::LOWER_C, common_lang_constants::UPPER_C) &&
                    is_either<wchar_t>(text[3], common_lang_constants::LOWER_C, common_lang_constants::UPPER_C) &&
                    is_either<wchar_t>(text[4], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[5], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[6], common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) ) )
                {
                return true;
                }
            return false;
            }
        //---------------------------------------------
        void step_1a(string_typeT& text)
            {
            if (stem<string_typeT>::is_suffix(text,/*sses*/common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                text.erase(text.length()-2);
                stem<string_typeT>::update_r_sections(text);
                }
            else if (stem<string_typeT>::is_suffix(text,/*ied*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) ||
                    stem<string_typeT>::is_suffix(text,/*ies*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (text.length() == 3 || text.length() == 4)
                    {
                    text.erase(text.length()-1);
                    stem<string_typeT>::update_r_sections(text);
                    }
                else
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (text.length() >= 2 &&
                    is_either<wchar_t>(text[text.length()-1], common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) &&
                    m_first_vowel < text.length()-2 &&
                    !string_util::is_one_of(text[text.length()-2], L"suSU") )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                }
            }
        //---------------------------------------------
        void step_1b(string_typeT& text)
            {
            //if the preceding word contains a vowel
            bool regress_trim = false;

            if (stem<string_typeT>::is_suffix(text,/*eed*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-3)
                    {
                    text.erase(text.length()-1);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (stem<string_typeT>::is_suffix(text,/*eedly*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-5)
                    {
                    text.erase(text.length()-3);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (stem<string_typeT>::is_suffix(text,/*ed*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) &&
                m_first_vowel < text.length()-2)
                {
                text.erase(text.length()-2);
                stem<string_typeT>::update_r_sections(text);
                regress_trim = true;
                }
            else if (stem<string_typeT>::is_suffix(text,/*edly*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y) &&
                m_first_vowel < text.length()-4)
                {
                text.erase(text.length()-4);
                stem<string_typeT>::update_r_sections(text);
                regress_trim = true;
                }
            else if (stem<string_typeT>::is_suffix(text,/*ing*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G) &&
                m_first_vowel < text.length()-3)
                {
                text.erase(text.length()-3);
                stem<string_typeT>::update_r_sections(text);
                regress_trim = true;
                }
            else if (stem<string_typeT>::is_suffix(text,/*ingly*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y) &&
                m_first_vowel < text.length()-5)
                {
                text.erase(text.length()-5);
                stem<string_typeT>::update_r_sections(text);
                regress_trim = true;
                }
            if (regress_trim)
                {
                if (stem<string_typeT>::is_suffix(text,/*at*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) ||
                    stem<string_typeT>::is_suffix(text,/*bl*/common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) ||
                    stem<string_typeT>::is_suffix(text,/*iz*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z) )
                    {
                    text += common_lang_constants::LOWER_E;
                    //need to search for r2 again because the 'e' added here may change that
                    stem<string_typeT>::find_r2(text, L"aeiouyAEIOUY");
                    }
                else if (stem<string_typeT>::is_suffix(text,/*bb*/common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B) ||
                        stem<string_typeT>::is_suffix(text,/*dd*/common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) ||
                        stem<string_typeT>::is_suffix(text,/*ff*/common_lang_constants::LOWER_F, common_lang_constants::UPPER_F, common_lang_constants::LOWER_F, common_lang_constants::UPPER_F) ||
                        stem<string_typeT>::is_suffix(text,/*gg*/common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G) ||
                        stem<string_typeT>::is_suffix(text,/*mm*/common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M) ||
                        stem<string_typeT>::is_suffix(text,/*nn*/common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) ||
                        stem<string_typeT>::is_suffix(text,/*pp*/common_lang_constants::LOWER_P, common_lang_constants::UPPER_P, common_lang_constants::LOWER_P, common_lang_constants::UPPER_P) ||
                        stem<string_typeT>::is_suffix(text,/*rr*/common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) ||
                        stem<string_typeT>::is_suffix(text,/*tt*/common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                    {
                    text.erase(text.length()-1);
                    stem<string_typeT>::update_r_sections(text);
                    }
                else if (is_short_word(text, text.length() ) )
                    {
                    text += common_lang_constants::LOWER_E;
                    //need to search for r2 again because the 'e' added here may change that
                    stem<string_typeT>::find_r2(text, L"aeiouyAEIOUY");
                    }
                }
            }
        //---------------------------------------------
        void step_1c(string_typeT& text)
            {
            //proceeding consonant cannot be first letter in word
            if (text.length() > 2 &&
                !is_vowel(text[text.length()-2]) )
                {
                if (is_either<wchar_t>(text[text.length()-1], common_lang_constants::LOWER_Y, LOWER_Y_HASH) )
                    {
                    text[text.length()-1] = common_lang_constants::LOWER_I;
                    }
                else if (is_either<wchar_t>(text[text.length()-1], common_lang_constants::UPPER_Y, UPPER_Y_HASH) )
                    {
                    text[text.length()-1] = common_lang_constants::UPPER_I;
                    }
                }
            }
        //---------------------------------------------
        void step_2(string_typeT& text)
            {
            if (text.length() >= 7 &&
                (stem<string_typeT>::is_suffix(text,/*ization*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) ||
                stem<string_typeT>::is_suffix(text,/*ational*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) ) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-7)
                    {
                    text.erase(text.length()-4);
                    text[static_cast<int>(text.length()-1)] = common_lang_constants::LOWER_E;
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (text.length() >= 7 &&
                (stem<string_typeT>::is_suffix(text,/*fulness*/common_lang_constants::LOWER_F, common_lang_constants::UPPER_F, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) ||
                stem<string_typeT>::is_suffix(text,/*ousness*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) ||
                stem<string_typeT>::is_suffix(text,/*iveness*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) ) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-7)
                    {
                    text.erase(text.length()-4);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (text.length() >= 6 &&
                (stem<string_typeT>::is_suffix(text,/*tional*/common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) ||
                stem<string_typeT>::is_suffix(text,/*lessli*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-6)
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (text.length() >= 6 && stem<string_typeT>::is_suffix(text,/*biliti*/common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-6)
                    {
                    text.erase(text.length()-3);
                    text[text.length()-2] = common_lang_constants::LOWER_L;
                    text[text.length()-1] = common_lang_constants::LOWER_E;
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (text.length() >= 5 &&
                (stem<string_typeT>::is_suffix(text,/*iviti*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ||
                stem<string_typeT>::is_suffix(text,/*ation*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) ) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-5)
                    {
                    text.erase(text.length()-2);
                    text[text.length()-1] = common_lang_constants::LOWER_E;
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (text.length() >= 5 &&
                (stem<string_typeT>::is_suffix(text,/*alism*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M) ||
                stem<string_typeT>::is_suffix(text,/*aliti*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-5)
                    {
                    text.erase(text.length()-3);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (text.length() >= 5 &&
                (stem<string_typeT>::is_suffix(text,/*ousli*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ||
                stem<string_typeT>::is_suffix(text,/*entli*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ||
                stem<string_typeT>::is_suffix(text,/*fulli*/common_lang_constants::LOWER_F, common_lang_constants::UPPER_F, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-5)
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (text.length() >= 4 && stem<string_typeT>::is_suffix(text,/*alli*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-4)
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (text.length() >= 4 &&
                (stem<string_typeT>::is_suffix(text,/*enci*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ||
                stem<string_typeT>::is_suffix(text,/*anci*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ||
                stem<string_typeT>::is_suffix(text,/*abli*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-4)
                    {
                    text[text.length()-1] = common_lang_constants::LOWER_E;
                    }
                }
            else if (text.length() >= 4 && stem<string_typeT>::is_suffix(text,/*izer*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-4)
                    {
                    text.erase(text.length()-1);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (text.length() >= 4 && stem<string_typeT>::is_suffix(text,/*ator*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-4)
                    {
                    text.erase(text.length()-1);
                    text[text.length()-1] = common_lang_constants::LOWER_E;
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (text.length() >= 3 &&
                stem<string_typeT>::get_r1() <= (text.length()-3) &&
                stem<string_typeT>::is_suffix(text,/*bli*/common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                text[text.length()-1] = common_lang_constants::LOWER_E;
                }
            else if (text.length() >= 3 &&
                stem<string_typeT>::get_r1() <= (text.length()-3) &&
                stem<string_typeT>::is_suffix(text,/*ogi*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                if (is_either<wchar_t>(text[text.length()-4], common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) )
                    {
                    text.erase(text.length()-1);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (text.length() >= 2 &&
                    stem<string_typeT>::get_r1() <= (text.length()-2) &&
                    stem<string_typeT>::is_suffix(text,/*li*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                if (string_util::is_one_of(text[text.length()-3], L"cdeghkmnrtCDEGHKMNRT") )
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            }
        //---------------------------------------------
        void step_3(string_typeT& text) 
            {
            if (text.length() >= 7 && stem<string_typeT>::is_suffix(text,/*ational*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-7)
                    {
                    text.erase(text.length()-4);
                    text[text.length()-1] = common_lang_constants::LOWER_E;
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (text.length() >= 6 && stem<string_typeT>::is_suffix(text,/*tional*/common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-6)
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (text.length() >= 5 &&
                (stem<string_typeT>::is_suffix(text,/*icate*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ||
                stem<string_typeT>::is_suffix(text,/*iciti*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ||
                stem<string_typeT>::is_suffix(text,/*alize*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-5)
                    {
                    text.erase(text.length()-3);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (text.length() >= 5 && stem<string_typeT>::is_suffix(text,/*ative*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (stem<string_typeT>::get_r2() <= text.length()-5)
                    {
                    text.erase(text.length()-5);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (text.length() >= 4 && stem<string_typeT>::is_suffix(text,/*ical*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-4)
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (text.length() >= 4 && stem<string_typeT>::is_suffix(text,/*ness*/common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-4)
                    {
                    text.erase(text.length()-4);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (text.length() >= 3 && stem<string_typeT>::is_suffix(text,/*ful*/common_lang_constants::LOWER_F, common_lang_constants::UPPER_F, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-3)
                    {
                    text.erase(text.length()-3);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            }
        //---------------------------------------------
        void step_4(string_typeT& text)
            {
            if (text.length() >= 5 &&
                stem<string_typeT>::is_suffix(text,/*ement*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                if (stem<string_typeT>::get_r2() <= text.length()-5)
                    {
                    text.erase(text.length()-5);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (text.length() >= 4 &&
                    (stem<string_typeT>::is_suffix(text,/*able*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ||
                    stem<string_typeT>::is_suffix(text,/*ible*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ||
                    stem<string_typeT>::is_suffix(text,/*ment*/common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) ||
                    stem<string_typeT>::is_suffix(text,/*ence*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ||
                    stem<string_typeT>::is_suffix(text,/*ance*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E)) )
                {
                if (stem<string_typeT>::get_r2() <= text.length()-4)
                    {
                    text.erase(text.length()-4);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (text.length() >= 4 &&
                    (stem<string_typeT>::is_suffix(text,/*sion*/common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) ||
                    stem<string_typeT>::is_suffix(text,/*tion*/common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N)) )
                {
                if (stem<string_typeT>::get_r2() <= text.length()-3)
                    {
                    text.erase(text.length()-3);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (text.length() >= 3 &&
                    (stem<string_typeT>::is_suffix(text,/*ant*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) ||
                    stem<string_typeT>::is_suffix(text,/*ent*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) ||
                    stem<string_typeT>::is_suffix(text,/*ism*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M) ||
                    stem<string_typeT>::is_suffix(text,/*ate*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ||
                    stem<string_typeT>::is_suffix(text,/*iti*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ||
                    stem<string_typeT>::is_suffix(text,/*ous*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) ||
                    stem<string_typeT>::is_suffix(text,/*ive*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ||
                    stem<string_typeT>::is_suffix(text,/*ize*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E)) )
                {
                if (stem<string_typeT>::get_r2() <= text.length()-3)
                    {
                    text.erase(text.length()-3);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (text.length() >= 2 &&
                    (stem<string_typeT>::is_suffix(text,/*al*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) ||
                    stem<string_typeT>::is_suffix(text,/*er*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) ||
                    stem<string_typeT>::is_suffix(text,/*ic*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C)) )
                {
                if (stem<string_typeT>::get_r2() <= text.length()-2)
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            }
        //---------------------------------------------
        void step_5(string_typeT& text)
            {
            if (text.length() >= 1 &&
                is_either<wchar_t>(text[text.length()-1], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (stem<string_typeT>::get_r2() != text.length())
                    {
                    text.erase(text.length()-1);
                    stem<string_typeT>::update_r_sections(text);
                    }
                else if (stem<string_typeT>::get_r1() != text.length() &&
                    text.length() >= 2 &&
                    //look at the part of the word in front of the last 'e' to see if it ends with
                    //a short syllable.
                    !ends_with_short_syllable(text, text.length()-1))
                    {
                    text.erase(text.length()-1);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            else if (stem<string_typeT>::get_r2() != text.length() &&
                stem<string_typeT>::is_suffix(text,/*ll*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                }
            }
        /**Define a short syllable in a word as either
        (a) a vowel followed by a non-vowel other than w, x or Y and preceded by a non-vowel, or 
        (b) a vowel at the beginning of the word followed by a non-vowel.

        So rap, trap, entrap end with a short syllable, and ow, on, at are classed as short syllables.
        But uproot, bestow, disturb do not end with a short syllable.*/
        //---------------------------------------------
        bool ends_with_short_syllable(const string_typeT& text, const size_t length) const
            {
            if (length == 2)
                {
                if (is_vowel(text[0]) )
                    { return (!is_vowel(text[1]) ); }
                else
                    { return false; }
                }
            else if (length > 2)
                {
                size_t start = text.find_last_of(L"aeiouyAEIOUY", length-1);
                if (start == string_typeT::npos)
                    { return false; }
                if (start > 0 &&
                    start == (length-2) &&
                    //following letter
                    (!is_vowel(text[start+1]) &&
                    !string_util::is_one_of(text[start+1], L"wxWX") &&
                    is_neither(text[start+1], LOWER_Y_HASH, UPPER_Y_HASH)) &&
                    //proceeding letter
                    !is_vowel(text[start-1]) )
                    { return true; }
                else
                    { return false; }
                }
            else
                { return false; }
            }
        ///A word is called short if it ends in a short syllable, and if R1 is null.
        //---------------------------------------------
        inline bool is_short_word(const string_typeT& text, const size_t length) const
            { return (ends_with_short_syllable(text, length) && stem<string_typeT>::get_r1() == text.length()); }
        //---------------------------------------------
        inline bool is_vowel(const wchar_t character) const
            { return (string_util::is_one_of(character, L"aeiouyAEIOUY") ); }

        size_t m_first_vowel;
        };
    }

/** @}*/

#endif //__ENGLISH_STEM_H__
