/**@addtogroup Stemming
@brief Library for stemming words down to their root words.
@date 2003-2015
@copyright Oleander Software, Ltd.
@author Oleander Software, Ltd.
@details This program is free software; you can redistribute it and/or modify
it under the terms of the BSD License.
* @{*/

#ifndef __FINNISH_STEM_H__
#define __FINNISH_STEM_H__

#include "stemming.h"

namespace stemming
    {
    /**
    @brief Finnish stemmer.
    @date 2004
    @par Overview:

    Finnish is not an Indo-European language, but belongs to the Finno-Ugric group,
    which again belongs to the Uralic group. Distinctions between a-, i- and d-suffixes
    can be made in Finnish, but they are much less sharply separated than in an
    Indo-European language. The system of endings is extremely elaborate, but strictly
    defined, and applies equally to all nominals, that is, to nouns, adjectives and pronouns.
    Verb endings have a close similarity to nominal endings, which again makes Finnish
    very different from any Indo-European language.

    More problematical than the endings themselves is the change that can be effected in a
    stem as a result of taking a particular ending. A stem typically has two forms
    ,strong and weak, where one class of ending follows the strong form and the complementary
    class the weak. Normalising strong and weak forms after ending removal is not generally possible,
    although the common case where strong and weak forms only differ in the single or double form of
    a final consonant can be dealt with.

    @par Algorithm:

    Finnish includes the following accented forms:
        - ä ö

    The following letters are vowels:
        - a e i o u y ä ö

    R1 and R2 are then defined in the usual way.

    @par Algorithm:

    <b>Step 1 (particles etc.):</b>

    Search for the longest among the following suffixes in R1, and perform the action indicated:

    - kin kaan kään ko kö han hän pa pä
        - Delete if preceded by n, t or a vowel.
    - sti
        - Delete if in R2.
        (Of course, the n, t or vowel of 1(a) need not be in R1:
        only the suffix removed must be in R1. And similarly below.

    <b>Step 2 (possessives):</b>

    Search for the longest among the following suffixes in R1, and perform the action indicated:

    - si
        - Delete if not preceded by k.
    - ni
        - Delete. If preceded by kse, replace with ksi.
    - nsa nsä mme nne
        - Delete.
    - an
        - Delete if preceded by one of:   ta   ssa   sta   lla   lta   na.
    - än
        - Delete if preceded by one of: tä ssä stä llä ltä nä.
    - en
        - Delete if preceded by one of:   lle   ine.

    <b>Step 3 (cases):</b>

    Define a v (vowel) as one of:
        - a   e   i   o   u   y   ä ö.

    Define a V (restricted vowel) as one of:
        - a   e   i   o   u   ä ö.

    So Vi means a V followed by letter i.

    Define LV (long vowel) as one of:
        - aa   ee   ii   oo   uu   ää öö.

    Define a c (consonant) as a character other than a v.
    So cv means a c followed by a v.

    Search for the longest among the following suffixes in R1, and perform the action indicated:

    - hXn   (preceded by X, where X is a V, other than u [a/han, e/hen etc]) 
    - siin   den   tten   (preceded by Vi)
    - seen   (preceded by LV)
    - a ä    (preceded by cv)
    - tta   ttä   (preceded by e)
    - ta   tä   ssa   ssä   sta   stä   lla   llä   lta   ltä   lle   na   nä   ksi   ine
        - Delete.
    - n
        - Delete, and if preceded by LV or ie, delete the last vowel.

    <b>Step 4 (other endings):</b>

    Search for the longest among the following suffixes in R2, and perform the action indicated:

    - mpi   mpa   mpä   mmi   mma   mmä
        - Delete if not preceded by po.
    - impi   impa   impä   immi   imma   immä   eja   ejä 
        - Delete.

    <b>Step 5 (plurals): </b>

    If an ending was removed in step 3, delete a final 'i' or 'j' if in R1;
    otherwise, if an ending was not removed in step 3,
    then delete the final 't' in R1 if it follows a vowel, and, if a 't' is removed,
    then delete a final "mma" or "imma" in R2, unless the "mma" is preceded by "po".

    <b>Step 6 (tidying up):</b>

    Perform steps (a), (b), (c), (d), restricting all tests to the R1 region.

    <b>Step 6(a):</b>

    If R1 ends with LV then delete the last letter.

    <b>Step 6(b):</b>

    If R1 ends with cX, c a consonant and X one of:
    - a   ä   e   i
        -Delete the last letter.

    <b>Step 6(c):</b>

    If R1 ends with oj or uj, then delete the last letter.

    <b>Step 6(d):</b>

    If R1 ends with jo then, delete the last letter.

    <b>Step 6(e):</b>

    This is not restricted to R1. If the word ends with a double consonant followed by zero or more vowels,
    then remove the last consonant (so eläkk -> eläk, aatonaatto -> aatonaato).
    */
    //------------------------------------------------------
    template <typename string_typeT = std::wstring>
    class finnish_stem : public stem<string_typeT>
        {
    public:
        finnish_stem() : m_step_3_successful(false) {}
        //---------------------------------------------
        /**@param[in,out] text string to stem*/
        void operator()(string_typeT& text)
            {
            if (text.length() < 2)
                {
                return;
                }

            //reset internal data
            m_step_3_successful = false;
            stem<string_typeT>::reset_r_values();

            stem<string_typeT>::trim_western_punctuation(text);

            stem<string_typeT>::find_r1(text, FINNISH_VOWELS);
            stem<string_typeT>::find_r2(text, FINNISH_VOWELS);

            step_1(text);
            step_2(text);
            step_3(text);
            step_4(text);
            step_5(text);
            step_6(text);
            }
    private:
        //---------------------------------------------
        void step_1(string_typeT& text)
            {
            if (stem<string_typeT>::is_suffix_in_r1(text,/*kaan*/common_lang_constants::LOWER_K, common_lang_constants::UPPER_K, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) ||
                stem<string_typeT>::is_suffix_in_r1(text,/*kään*/common_lang_constants::LOWER_K, common_lang_constants::UPPER_K, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) )
                {
                if (text.length() >= 5 &&
                    string_util::is_one_of(text[text.length()-5], FINNISH_STEP_1_SUFFIX) )
                    {
                    text.erase(text.length()-4);
                    stem<string_typeT>::update_r_sections(text);
                    }
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_r1(text,/*kin*/common_lang_constants::LOWER_K, common_lang_constants::UPPER_K, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) ||
                stem<string_typeT>::is_suffix_in_r1(text,/*han*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) ||
                stem<string_typeT>::is_suffix_in_r1(text,/*hän*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) )
                {
                if (text.length() >= 4 &&
                    string_util::is_one_of(text[text.length()-4], FINNISH_STEP_1_SUFFIX) )
                    {
                    text.erase(text.length()-3);
                    stem<string_typeT>::update_r_sections(text);
                    }
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_r1(text,/*sti*/common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                stem<string_typeT>::delete_if_is_in_r2(text,/*sti*/common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I);
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_r1(text,/*ko*/common_lang_constants::LOWER_K, common_lang_constants::UPPER_K, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) ||
                stem<string_typeT>::is_suffix_in_r1(text,/*kö*/common_lang_constants::LOWER_K, common_lang_constants::UPPER_K, common_lang_constants::LOWER_O_UMLAUTS, common_lang_constants::UPPER_O_UMLAUTS) ||
                stem<string_typeT>::is_suffix_in_r1(text,/*pa*/common_lang_constants::LOWER_P, common_lang_constants::UPPER_P, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) ||
                stem<string_typeT>::is_suffix_in_r1(text,/*pä*/common_lang_constants::LOWER_P, common_lang_constants::UPPER_P,common_lang_constants:: LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS) )
                {
                if (text.length() >= 3 &&
                    string_util::is_one_of(text[text.length()-3], FINNISH_STEP_1_SUFFIX) )
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    }
                return;
                }
            }
        //---------------------------------------------
        void step_2(string_typeT& text)
            {
            if (stem<string_typeT>::delete_if_is_in_r1(text,/*nsa*/common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) ||
                stem<string_typeT>::delete_if_is_in_r1(text,/*nsä*/common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS, false) ||
                stem<string_typeT>::delete_if_is_in_r1(text,/*mme*/common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) ||
                stem<string_typeT>::delete_if_is_in_r1(text,/*nne*/common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_r1(text,/*si*/common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                if (text.length() >= 3 &&
                    !(text[text.length()-3] == common_lang_constants::LOWER_K || text[text.length()-3] == common_lang_constants::UPPER_K))
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    }
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ni*/common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) )
                {
                if (stem<string_typeT>::is_suffix(text, /*kse*/common_lang_constants::LOWER_K, common_lang_constants::UPPER_K, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                    {
                    text[text.length()-1] = common_lang_constants::LOWER_I;
                    }
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_r1(text,/*an*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) )
                {
                if ((text.length() >= 4 &&
                    (stem<string_typeT>::is_partial_suffix(text, (text.length()-4), common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) ||
                    stem<string_typeT>::is_partial_suffix(text, (text.length()-4), common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A)) ) ||
                    (text.length() >= 5 &&
                    (stem<string_typeT>::is_partial_suffix(text, (text.length()-5), common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) ||
                    stem<string_typeT>::is_partial_suffix(text, (text.length()-5), common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) ||
                    stem<string_typeT>::is_partial_suffix(text, (text.length()-5), common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) ||
                    stem<string_typeT>::is_partial_suffix(text, (text.length()-5), common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) ) ) )
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    }
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_r1(text,/*än*/common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) )
                {
                if ((text.length() >= 4 &&
                    (stem<string_typeT>::is_partial_suffix(text, (text.length()-4), common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS) ||
                    stem<string_typeT>::is_partial_suffix(text, (text.length()-4), common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS)) ) ||
                    (text.length() >= 5 &&
                    (stem<string_typeT>::is_partial_suffix(text, (text.length()-5), common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS) ||
                    stem<string_typeT>::is_partial_suffix(text, (text.length()-5), common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS) ||
                    stem<string_typeT>::is_partial_suffix(text, (text.length()-5), common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS) ||
                    stem<string_typeT>::is_partial_suffix(text, (text.length()-5), common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS) ) ) )
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    }
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_r1(text,/*en*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) )
                {
                if (text.length() >= 5 &&
                    (stem<string_typeT>::is_partial_suffix(text, (text.length()-5), common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ||
                    stem<string_typeT>::is_partial_suffix(text, (text.length()-5), common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ) )
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    }
                return;
                }
            }
        //---------------------------------------------
        void step_3(string_typeT& text)
            {
            //seen followed by LV
            if (text.length() >= 6 &&
                stem<string_typeT>::is_suffix_in_r1(text,/*seen*/common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                string_util::is_one_of(text[text.length()-5], FINNISH_VOWELS_NO_Y) &&
                string_util::tolower_western(text[text.length()-5]) == string_util::tolower_western(text[text.length()-6]) )
                {
                text.erase(text.length()-4);
                stem<string_typeT>::update_r_sections(text);
                m_step_3_successful = true;
                return;
                }
            //suffix followed by Vi
            else if (text.length() >= 6 &&
                    is_either<wchar_t>(text[text.length()-5], common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) &&
                    (stem<string_typeT>::is_suffix_in_r1(text,/*siin*/common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) ||
                    stem<string_typeT>::is_suffix_in_r1(text,/*tten*/common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) ) &&
                    string_util::is_one_of(text[text.length()-6], FINNISH_VOWELS_NO_Y) )
                {
                text.erase(text.length()-4);
                stem<string_typeT>::update_r_sections(text);
                m_step_3_successful = true;
                return;
                }
            //suffix followed by Vi
            else if (text.length() >= 5 &&
                    is_either<wchar_t>(text[text.length()-4], common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) &&
                    stem<string_typeT>::is_suffix_in_r1(text,/*den*/common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    string_util::is_one_of(text[text.length()-5], FINNISH_VOWELS_NO_Y) )
                {
                text.erase(text.length()-3);
                stem<string_typeT>::update_r_sections(text);
                m_step_3_successful = true;
                return;
                }
            else if ((stem<string_typeT>::is_suffix_in_r1(text,/*tta*/common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) ||
                stem<string_typeT>::is_suffix_in_r1(text,/*ttä*/common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS)) &&
                is_either<wchar_t>(text[text.length()-4], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                text.erase(text.length()-3);
                stem<string_typeT>::update_r_sections(text);
                m_step_3_successful = true;
                return;
                }
            //ends if VHVN
            else if (
                (stem<string_typeT>::is_suffix_in_r1(text,/*han*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) ||
                stem<string_typeT>::is_suffix_in_r1(text,/*hen*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) ||
                stem<string_typeT>::is_suffix_in_r1(text,/*hin*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) ||
                stem<string_typeT>::is_suffix_in_r1(text,/*hon*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) ||
                stem<string_typeT>::is_suffix_in_r1(text,/*hän*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) ||
                stem<string_typeT>::is_suffix_in_r1(text,/*hön*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_O_UMLAUTS, common_lang_constants::UPPER_O_UMLAUTS, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) ) )
                {
                if (string_util::tolower_western(text[text.length()-2]) == string_util::tolower_western(text[text.length()-4]) )
                    {
                    text.erase(text.length()-3);
                    stem<string_typeT>::update_r_sections(text);
                    m_step_3_successful = true;
                    }
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ssa*/common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) ||
                stem<string_typeT>::delete_if_is_in_r1(text,/*ssä*/common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS, false) ||
                stem<string_typeT>::delete_if_is_in_r1(text,/*sta*/common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) ||
                stem<string_typeT>::delete_if_is_in_r1(text,/*stä*/common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS, false) ||
                stem<string_typeT>::delete_if_is_in_r1(text,/*lla*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) ||
                stem<string_typeT>::delete_if_is_in_r1(text,/*llä*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS, false) ||
                stem<string_typeT>::delete_if_is_in_r1(text,/*lta*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) ||
                stem<string_typeT>::delete_if_is_in_r1(text,/*ltä*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS, false) ||
                stem<string_typeT>::delete_if_is_in_r1(text,/*lle*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) ||
                stem<string_typeT>::delete_if_is_in_r1(text,/*ksi*/common_lang_constants::LOWER_K, common_lang_constants::UPPER_K, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) ||
                stem<string_typeT>::delete_if_is_in_r1(text,/*ine*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) ||
                stem<string_typeT>::delete_if_is_in_r1(text,/*na*/common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) ||
                stem<string_typeT>::delete_if_is_in_r1(text,/*nä*/common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS, false) )
                {
                m_step_3_successful = true;
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ta*/common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) ||
                stem<string_typeT>::delete_if_is_in_r1(text,/*tä*/common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS, false) )
                {
                m_step_3_successful = true;
                return;
                }
            //suffix followed by cv
            else if (text.length() >= 3 &&
                    (stem<string_typeT>::is_suffix_in_r1(text, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) || stem<string_typeT>::is_suffix_in_r1(text, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS) ) &&
                    !string_util::is_one_of(text[text.length()-3], FINNISH_VOWELS) &&
                    string_util::is_one_of(text[text.length()-2], FINNISH_VOWELS) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                m_step_3_successful = true;
                return;
                }
            //suffix followed by LV or ie
            else if (stem<string_typeT>::is_suffix_in_r1(text, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                if (text.length() >= 2 &&
                    ((string_util::is_one_of(text[text.length()-1], FINNISH_VOWELS_NO_Y) &&
                      string_util::tolower_western(text[text.length()-1]) == string_util::tolower_western(text[text.length()-2])) ||
                     stem<string_typeT>::is_suffix_in_r1(text,/*ie*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E)) )
                    {
                    text.erase(text.length()-1);
                    stem<string_typeT>::update_r_sections(text);
                    }
                m_step_3_successful = true;
                return;
                }
            }
        //---------------------------------------------
        void step_4(string_typeT& text)
            {
            if (stem<string_typeT>::delete_if_is_in_r2(text,/*impi*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_P, common_lang_constants::UPPER_P, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) ||
                stem<string_typeT>::delete_if_is_in_r2(text,/*impa*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_P, common_lang_constants::UPPER_P, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) ||
                stem<string_typeT>::delete_if_is_in_r2(text,/*impä*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_P, common_lang_constants::UPPER_P, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS, false) ||
                stem<string_typeT>::delete_if_is_in_r2(text,/*immi*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) ||
                stem<string_typeT>::delete_if_is_in_r2(text,/*imma*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) ||
                stem<string_typeT>::delete_if_is_in_r2(text,/*immä*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS, false) ||
                stem<string_typeT>::delete_if_is_in_r2(text,/*eja*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_J, common_lang_constants::UPPER_J, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) ||
                stem<string_typeT>::delete_if_is_in_r2(text,/*ejä*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_J, common_lang_constants::UPPER_J, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS, false) )
                {
                return;
                }
            else if (text.length() >= 5 &&
                    (stem<string_typeT>::is_suffix_in_r2(text,/*mpi*/common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_P, common_lang_constants::UPPER_P, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ||
                    stem<string_typeT>::is_suffix_in_r2(text,/*mpa*/common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_P, common_lang_constants::UPPER_P, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) ||
                    stem<string_typeT>::is_suffix_in_r2(text,/*mpä*/common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_P, common_lang_constants::UPPER_P, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS) ||
                    stem<string_typeT>::is_suffix_in_r2(text,/*mmi*/common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ||
                    stem<string_typeT>::is_suffix_in_r2(text,/*mma*/common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) ||
                    stem<string_typeT>::is_suffix_in_r2(text,/*mmä*/common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_A_UMLAUTS, common_lang_constants::UPPER_A_UMLAUTS) ) )
                {
                if (!(is_either<wchar_t>(text[text.length()-5], common_lang_constants::LOWER_P, common_lang_constants::UPPER_P) &&
                    is_either<wchar_t>(text[text.length()-4], common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) ) )
                    {
                    text.erase(text.length()-3);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            return;
            }
        //---------------------------------------------
        void step_5(string_typeT& text)
            {
            //if step 3 was successful in removing a suffix
            if (m_step_3_successful)
                {
                if (stem<string_typeT>::delete_if_is_in_r1(text, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ||
                    stem<string_typeT>::delete_if_is_in_r1(text, common_lang_constants::LOWER_J, common_lang_constants::UPPER_J) )
                    {
                    //NOOP
                    }
                }
            else
                {
                if (text.length() >= 2 &&
                    string_util::is_one_of(text[text.length()-2], FINNISH_VOWELS) )
                    {
                    if (stem<string_typeT>::delete_if_is_in_r1(text, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                        {
                        if (!stem<string_typeT>::delete_if_is_in_r2(text,/*imma*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                            {
                            if (text.length() >= 5 &&
                                stem<string_typeT>::is_suffix_in_r2(text,/*mma*/common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) &&
                                //isn't proceeded by "po"
                                !(is_either<wchar_t>(text[text.length()-5], common_lang_constants::LOWER_P, common_lang_constants::UPPER_P) &&
                                is_either<wchar_t>(text[text.length()-4], common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) ) )
                                {
                                text.erase(text.length()-3);
                                stem<string_typeT>::update_r_sections(text);
                                }
                            }
                        }
                    }
                }
            }
        //---------------------------------------------
        void step_6(string_typeT& text)
            {
            step_6a(text);
            step_6b(text);
            step_6c(text);
            step_6d(text);
            step_6e(text);
            }
        //---------------------------------------------
        void step_6a(string_typeT& text)
            {
            if (text.length() >= 2 &&
                stem<string_typeT>::get_r1() <= text.length()-2 &&
                string_util::is_one_of(text[text.length()-1], FINNISH_VOWELS_NO_Y) &&
                string_util::tolower_western(text[text.length()-1]) == string_util::tolower_western(text[text.length()-2]))
                {
                text.erase(text.end()-1);
                stem<string_typeT>::update_r_sections(text);
                }
            }
        //---------------------------------------------
        void step_6b(string_typeT& text)
            {
            if (text.length() >= 2 &&
                stem<string_typeT>::get_r1() <= text.length()-2 &&
                !string_util::is_one_of(text[text.length()-2], FINNISH_VOWELS) &&
                string_util::is_one_of(text[text.length()-1], FINNISH_VOWELS_SIMPLE) )
                {
                text.erase(text.end()-1);
                stem<string_typeT>::update_r_sections(text);
                }
            }
        //---------------------------------------------
        void step_6c(string_typeT& text)
            {
            if (stem<string_typeT>::is_suffix_in_r1(text,/*oj*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_J, common_lang_constants::UPPER_J) ||
                stem<string_typeT>::is_suffix_in_r1(text,/*uj*/common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_J, common_lang_constants::UPPER_J) )
                {
                text.erase(text.end()-1);
                stem<string_typeT>::update_r_sections(text);
                }
            }
        //---------------------------------------------
        void step_6d(string_typeT& text)
            {
            if (stem<string_typeT>::is_suffix_in_r1(text,/*jo*/common_lang_constants::LOWER_J, common_lang_constants::UPPER_J, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                text.erase(text.end()-1);
                stem<string_typeT>::update_r_sections(text);
                }
            }
        //---------------------------------------------
        void step_6e(string_typeT& text)
            {
            //find the last consonant
            size_t index = text.find_last_not_of(FINNISH_VOWELS);
            if (index == string_typeT::npos ||
                index < 1)
                { return; }
            if (string_util::tolower_western(text[index]) == string_util::tolower_western(text[index-1]))
                {
                text.erase(text.begin()+(index) );
                stem<string_typeT>::update_r_sections(text);
                }
            }
        //internal data specific to Finnish stemmer
        bool m_step_3_successful;
        };
    }

/** @}*/

#endif //__FINNISH_STEM_H__
