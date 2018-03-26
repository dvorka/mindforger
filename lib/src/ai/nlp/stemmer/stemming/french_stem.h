/**@addtogroup Stemming
@brief Library for stemming words down to their root words.
@date 2003-2015
@copyright Oleander Software, Ltd.
@author Oleander Software, Ltd.
@details This program is free software; you can redistribute it and/or modify
it under the terms of the BSD License.
* @{*/

#ifndef __FRENCH_STEM_H__
#define __FRENCH_STEM_H__

#include "stemming.h"

namespace stemming
    {
    /**
    @brief French stemmer.
    @date 2004
    @par Algorithm:

    Letters in French include the following accented forms:
        - â à ç ë é ê è ï î ô û ù 

    The following letters are vowels:
        - a e i o u y â à ë é ê è ï î ô û ù

    @par Algorithm:

    <b>Step 1:</b>

    Search for the longest among the following suffixes, and perform the action indicated.
        - ance iqUe isme able iste eux ances iqUes ismes ables istes
            - Delete if in R2.
        - atrice ateur ation atrices ateurs ations
            - Delete if in R2.
            - If preceded by ic, delete if in R2, else replace by iqU.
        - logie logies
            - Replace with log if in R2.
        - usion ution usions utions
            - Replace with u if in R2.
        -ence ences
            - Replace with ent if in R2.
        - ement ements
            - Delete if in RV
            - If preceded by iv, delete if in R2 (and if further preceded by at, delete if in R2), otherwise,
            - If preceded by eus, delete if in R2, else replace by eux if in R1, otherwise,
            - If preceded by abl or iqU, delete if in R2, otherwise,
        - If preceded by ièr or Ièr, replace by i if in RV
        - ité ités
            - Delete if in R2
            - If preceded by abil, delete if in R2, else replace by abl, otherwise,
            - If preceded by ic, delete if in R2, else replace by iqU, otherwise,
            - If preceded by iv, delete if in R2
            - If ive ifs ives, delete if in R2
            - If preceded by "at", delete if in R2 (and if further preceded by ic, delete if in R2, else replace by iqU).
        - eaux
            - Replace with eau.
        - aux
            - Replace with al if in R1.
        - euse euses
            - Delete if in R2, else replace by eux if in R1.
        - issement issements
            - Delete if in R1 and preceded by a non-vowel.
        - amment
            - Replace with ant if in RV.
        - emment
            - Replace with ent if in RV.
        -ment ments
            - Delete if preceded by a vowel in RV.

    In steps 2(a) and 2(b) all tests are confined to the RV region.

    Do step 2(a) if either no ending was removed by step 1, or if one of endings amment, emment, ment, ments was found.

    <b>Step 2(a):</b>

    Search for the longest among the following suffixes and if found, delete if preceded by a non-vowel:

        - îmes ît îtes i ie ies ir ira irai iraIent irais irait iras irent irez iriez irions irons iront is issaIent
        issais issait issant issante issantes issants isse issent isses issez issiez issions issons it

    (Note that the non-vowel itself must also be in RV).

    Do step 2b if step 2a was done, but failed to remove a suffix.

    <b>Step 2(b):</b>

    Search for the longest among the following suffixes, and perform the action indicated. 
        - ions
            - Delete if in R2.
        - é ée ées és èrent er era erai eraIent erais erait eras erez eriez erions erons eront ez iez
            - Delete.
        - âmes ât âtes a ai aIent ais ait ant ante antes ants as asse assent asses assiez assions
            - Delete.
            - If preceded by e, delete.

    (Note that the e that may be deleted in this last step must also be in RV).

    If the last step to be obeyed - either step 1, 2a or 2b - altered the word, do step 3.

    <b>Step 3:</b>

    Replace final Y with i or final ç with c.

    Alternatively, if the last step to be obeyed did not alter the word, do step 4.

    <b>Step 4:</b>

    If the word ends s, not preceded by a, i, o, u, è or s, delete it. 

    In the rest of step 4, all tests are confined to the RV region.

    Search for the longest among the following suffixes, and perform the action indicated:
        - ion
            - Delete if in R2 and preceded by s or t.
        - ier ière Ier Ière
            - Replace with i.
        - e
            - Delete.
        - ë 
            - If preceded by gu, delete.

    (So note that ion is removed only when it is in R2 - as well as being in RV -
    and preceded by s or t which must be in RV).

    <b>Step 5:</b>

    If the word ends enn, onn, ett, ell or eill, then delete the last letter.

    <b>Step 6:</b>

    If the words ends with é or è followed by at least one non-vowel, then remove the accent from the 'e'.
    */
    //------------------------------------------------------
    template <typename string_typeT = std::wstring>
    class french_stem : public stem<string_typeT>
        {
    public:
        french_stem() : m_step_1_successful(false) {}
        //---------------------------------------------
        /**@param[in,out] text string to stem*/
        void operator()(string_typeT& text)
            {
            if (text.length() < 2)
                {
                return;
                }

            //reset internal data
            m_step_1_successful = false;
            stem<string_typeT>::reset_r_values();

            stem<string_typeT>::trim_western_punctuation(text);
            stem<string_typeT>::hash_french_yui(text, FRENCH_VOWELS);

            stem<string_typeT>::find_r1(text, FRENCH_VOWELS);
            stem<string_typeT>::find_r2(text, FRENCH_VOWELS);
            stem<string_typeT>::find_french_rv(text, FRENCH_VOWELS);

            size_t length = text.length();
            step_1(text);
            if (!m_step_1_successful)
                {
                step_2a(text);
                }
            if (length != text.length() )
                {
                step_3(text);
                }
            else
                {
                step_4(text);
                }
            step_5(text);
            step_6(text);

            stem<string_typeT>::unhash_french_yui(text);
            }
    private:
        bool ic_to_iqu(string_typeT& text)
            {
            if (stem<string_typeT>::is_suffix(text,/*ic*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C) )
                {
                if (stem<string_typeT>::get_r2() <= text.length()-2)
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    return true;
                    }
                else
                    {
                    text.erase(text.length()-2);
                    text += L"iq";
                    text += LOWER_U_HASH;
                    //need to search for r2 again because the 'iq' added here may change that
                    stem<string_typeT>::find_r2(text, FRENCH_VOWELS);
                    stem<string_typeT>::find_french_rv(text, FRENCH_VOWELS);
                    return true;
                    }
                }
            return false;
            }
        //---------------------------------------------
        void step_1(string_typeT& text)
            {
            size_t length = text.length();
            if (stem<string_typeT>::is_suffix(text,/*issements*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (text.length() >= 10 &&
                    stem<string_typeT>::get_r1() <= (text.length()-9) &&
                    !string_util::is_one_of(text[text.length()-10], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-9);
                    m_step_1_successful = true;
                    }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*issement*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                if (text.length() >= 9 &&
                    stem<string_typeT>::get_r1() <= (text.length()-8) &&
                    !string_util::is_one_of(text[text.length()-9], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-8);
                    m_step_1_successful = true;
                    }
                return;
                }
            //7
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*atrices*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                if (length != text.length() )
                    {
                    ic_to_iqu(text);
                    m_step_1_successful = true;
                    }
                return;
                }
            //6
            else if (stem<string_typeT>::is_suffix(text,/*amment*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                if (stem<string_typeT>::get_rv() <= (text.length()-6) )
                    {
                    text.replace(text.end()-5, text.end(), L"nt");
                    stem<string_typeT>::update_r_sections(text);
                    }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*emment*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                if (stem<string_typeT>::get_rv() <= (text.length()-6) )
                    {
                    text.replace(text.end()-5, text.end(), L"nt");
                    stem<string_typeT>::update_r_sections(text);
                    }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*logies*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (stem<string_typeT>::get_r2() <= (text.length()-6) )
                    {
                    text.erase(text.length()-3);
                    stem<string_typeT>::update_r_sections(text);
                    m_step_1_successful = true;
                    }
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*atrice*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ateurs*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ations*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                if (length != text.length() )
                    {
                    ic_to_iqu(text);
                    m_step_1_successful = true;
                    }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*usions*/common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) ||
                    stem<string_typeT>::is_suffix(text,/*utions*/common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (stem<string_typeT>::get_r2() <= (text.length()-6) )
                    {
                    text.erase(text.length()-5);
                    stem<string_typeT>::update_r_sections(text);
                    m_step_1_successful = true;
                    }
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ements*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                if (stem<string_typeT>::delete_if_is_in_r2(text,/*iv*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V) )
                    {
                    stem<string_typeT>::delete_if_is_in_r2(text,/*at*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T);
                    }
                else if (stem<string_typeT>::is_suffix(text,/*eus*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                    {
                    if (stem<string_typeT>::get_r2() <= text.length()-3)
                        {
                        text.erase(text.length()-3);
                        stem<string_typeT>::update_r_sections(text);
                        }
                    else if (stem<string_typeT>::get_r1() <= text.length()-3)
                        {
                        text[text.length()-1] = common_lang_constants::LOWER_X;
                        }
                    }
                else if (stem<string_typeT>::delete_if_is_in_r2(text,/*abl*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) )
                    {
                    //NOOP
                    }
                else if (text.length() >= 3 &&
                    (text[text.length()-3] == common_lang_constants::LOWER_I || text[text.length()-3] == common_lang_constants::UPPER_I) &&
                    (text[text.length()-2] == common_lang_constants::LOWER_Q || text[text.length()-2] == common_lang_constants::UPPER_Q) &&
                    is_either<wchar_t>(text[text.length()-1], LOWER_U_HASH, UPPER_U_HASH) )
                    {
                    if (stem<string_typeT>::get_r2() <= text.length()-3)
                        {
                        text.erase(text.length()-3);
                        stem<string_typeT>::update_r_sections(text);
                        }
                    }
                else if (stem<string_typeT>::is_suffix_in_rv(text,/*ièr*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E_GRAVE, common_lang_constants::UPPER_E_GRAVE, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) )
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    }
                else if (text.length() >= 3 &&
                    stem<string_typeT>::get_rv() <= (text.length()-3) &&
                    (text[text.length()-2] == common_lang_constants::LOWER_E_GRAVE || text[text.length()-2] == common_lang_constants::UPPER_E_GRAVE) &&
                    (text[text.length()-1] == common_lang_constants::LOWER_R || text[text.length()-1] == common_lang_constants::UPPER_R) &&
                    is_either<wchar_t>(text[text.length()-3], LOWER_I_HASH, UPPER_I_HASH) )
                    {
                    text.replace(text.end()-3, text.end(), L"i");
                    stem<string_typeT>::update_r_sections(text);
                    }
                if (length != text.length() )
                    {
                    m_step_1_successful = true;
                    }
                return;
                }
            //5
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ateur*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, false) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ation*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                if (length != text.length() )
                    {
                    ic_to_iqu(text);
                    m_step_1_successful = true;
                    }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*usion*/common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) ||
                    stem<string_typeT>::is_suffix(text,/*ution*/common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) )
                {
                if (stem<string_typeT>::get_r2() <= (text.length()-5) )
                    {
                    text.erase(text.length()-4);
                    stem<string_typeT>::update_r_sections(text);
                    m_step_1_successful = true;
                    }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*ences*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (stem<string_typeT>::get_r2() <= (text.length()-5) )
                    {
                    text.replace(text.end()-3, text.end(), L"t");
                    stem<string_typeT>::update_r_sections(text);
                    m_step_1_successful = true;
                    }
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ables*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*istes*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ismes*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ances*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                if (length != text.length() )
                    {
                    m_step_1_successful = true;
                    }
                return;
                }
            else if (text.length() >= 5 &&
                    (text[text.length()-5] == common_lang_constants::LOWER_I || text[text.length()-5] == common_lang_constants::UPPER_I) &&
                    (text[text.length()-4] == common_lang_constants::LOWER_Q || text[text.length()-4] == common_lang_constants::UPPER_Q) &&
                    (text[text.length()-2] == common_lang_constants::LOWER_E || text[text.length()-2] == common_lang_constants::UPPER_E) &&
                    (text[text.length()-1] == common_lang_constants::LOWER_S || text[text.length()-1] == common_lang_constants::UPPER_S) &&
                    is_either<wchar_t>(text[text.length()-3], LOWER_U_HASH, UPPER_U_HASH) )
                    {
                    if (stem<string_typeT>::get_r2() <= text.length()-5)
                        {
                        text.erase(text.length()-5);
                        stem<string_typeT>::update_r_sections(text);
                        }
                    if (length != text.length() )
                        {
                        m_step_1_successful = true;
                        }
                    return;
                    }
            else if (stem<string_typeT>::is_suffix(text,/*logie*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (stem<string_typeT>::get_r2() <= (text.length()-5) )
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    m_step_1_successful = true;
                    }
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ement*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, false) )
                {
                if (stem<string_typeT>::delete_if_is_in_r2(text,/*iv*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, false) )
                    {
                    stem<string_typeT>::delete_if_is_in_r2(text, /*at*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T);
                    }
                else if (stem<string_typeT>::is_suffix(text,/*eus*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                    {
                    if (text.length() >= 3 &&
                        stem<string_typeT>::get_r2() <= text.length()-3)
                        {
                        text.erase(text.length()-3);
                        stem<string_typeT>::update_r_sections(text);
                        }
                    else if (text.length() >= 3 &&
                        stem<string_typeT>::get_r1() <= text.length()-3)
                        {
                        text[text.length()-1] = common_lang_constants::LOWER_X;
                        }
                    }
                else if (stem<string_typeT>::delete_if_is_in_r2(text,/*abl*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) )
                    {
                    //NOOP
                    }
                else if (text.length() >= 3 &&
                    (text[text.length()-3] == common_lang_constants::LOWER_I || text[text.length()-3] == common_lang_constants::UPPER_I) &&
                    (text[text.length()-2] == common_lang_constants::LOWER_Q || text[text.length()-2] == common_lang_constants::UPPER_Q) &&
                    is_either<wchar_t>(text[text.length()-1], LOWER_U_HASH, UPPER_U_HASH) )
                    {
                    if (stem<string_typeT>::get_r2() <= text.length()-3)
                        {
                        text.erase(text.length()-3);
                        stem<string_typeT>::update_r_sections(text);
                        }
                    }
                else if (stem<string_typeT>::is_suffix_in_rv(text,/*ièr*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E_GRAVE, common_lang_constants::UPPER_E_GRAVE, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) )
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    }
                else if (text.length() >= 3 &&
                    stem<string_typeT>::get_rv() <= (text.length()-3) &&
                    (text[text.length()-2] == common_lang_constants::LOWER_E_GRAVE || text[text.length()-2] == common_lang_constants::UPPER_E_GRAVE) &&
                    (text[text.length()-1] == common_lang_constants::LOWER_R || text[text.length()-1] == common_lang_constants::UPPER_R) &&
                    is_either<wchar_t>(text[text.length()-3], LOWER_I_HASH, UPPER_I_HASH) )
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    }
                if (length != text.length() )
                    {
                    m_step_1_successful = true;
                    }
                }
            else if (stem<string_typeT>::is_suffix(text,/*ments*/common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                //the proceeding vowel must also be n RV
                if (text.length() >= 6 &&
                    stem<string_typeT>::get_rv() <= text.length()-6 &&
                    string_util::is_one_of(text[text.length()-6], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-5);
                    stem<string_typeT>::update_r_sections(text);
                    }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*euses*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (stem<string_typeT>::get_r2() <= text.length()-5)
                    {
                    text.erase(text.length()-5);
                    stem<string_typeT>::update_r_sections(text);
                    }
                else if (stem<string_typeT>::get_r1() <= text.length()-5)
                    {
                    text.replace(text.end()-3, text.end(), L"x");
                    stem<string_typeT>::update_r_sections(text);
                    }
                m_step_1_successful = true;
                }
            //4
            else if (stem<string_typeT>::is_suffix(text,/*euse*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (stem<string_typeT>::get_r2() <= text.length()-4)
                    {
                    text.erase(text.length()-4);
                    stem<string_typeT>::update_r_sections(text);
                    }
                else if (text.length() >= 4 &&
                    stem<string_typeT>::get_r1() <= text.length()-4)
                    {
                    text.replace(text.end()-2, text.end(), L"x");
                    stem<string_typeT>::update_r_sections(text);
                    }
                m_step_1_successful = true;
                }
            else if (stem<string_typeT>::is_suffix(text,/*ment*/common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                //the proceeding vowel must also be n RV
                if (text.length() >= 5 &&
                    stem<string_typeT>::get_rv() <= text.length()-5 &&
                    string_util::is_one_of(text[text.length()-5], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-4);
                    stem<string_typeT>::update_r_sections(text);
                    }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*ence*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (stem<string_typeT>::get_r2() <= (text.length()-4) )
                    {
                    text.replace(text.end()-2, text.end(), L"t");
                    stem<string_typeT>::update_r_sections(text);
                    m_step_1_successful = true;
                    }
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ance*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*isme*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*able*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*iste*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                if (length != text.length() )
                    {
                    m_step_1_successful = true;
                    }
                return;
                }
            else if (text.length() >= 4 &&
                    (text[text.length()-4] == common_lang_constants::LOWER_I || text[text.length()-4] == common_lang_constants::UPPER_I) &&
                    (text[text.length()-3] == common_lang_constants::LOWER_Q || text[text.length()-3] == common_lang_constants::UPPER_Q) &&
                    (text[text.length()-1] == common_lang_constants::LOWER_E || text[text.length()-1] == common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[text.length()-2], LOWER_U_HASH, UPPER_U_HASH) )
                    {
                    if (stem<string_typeT>::get_r2() <= text.length()-4)
                        {
                        text.erase(text.length()-4);
                        stem<string_typeT>::update_r_sections(text);
                        }
                    if (length != text.length() )
                        {
                        m_step_1_successful = true;
                        }
                    return;
                    }
            else if (stem<string_typeT>::is_suffix(text,/*eaux*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_X, common_lang_constants::UPPER_X) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                m_step_1_successful = true;
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ités*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E_ACUTE, common_lang_constants::UPPER_E_ACUTE, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                if (stem<string_typeT>::is_suffix(text,/*abil*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) )
                    {
                    if (stem<string_typeT>::get_r2() <= text.length()-4)
                        {
                        text.erase(text.length()-4);
                        stem<string_typeT>::update_r_sections(text);
                        }
                    else
                        {
                        text.replace(text.end()-2, text.end(), L"l");
                        }
                    }
                else if (stem<string_typeT>::is_suffix(text,/*ic*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C) )
                    {
                    if (stem<string_typeT>::get_r2() <= text.length()-2)
                        {
                        text.erase(text.length()-2);
                        stem<string_typeT>::update_r_sections(text);
                        }
                    else
                        {
                        text.erase(text.length()-2);
                        text += L"iq";
                        text += LOWER_U_HASH;
                        //need to search for r2 again because the 'iq' added here may change that
                        stem<string_typeT>::find_r2(text, FRENCH_VOWELS);
                        stem<string_typeT>::find_french_rv(text, FRENCH_VOWELS);
                        }
                    }
                else
                    {
                    stem<string_typeT>::delete_if_is_in_r2(text,/*iv*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V);
                    }
                if (length != text.length() )
                    {
                    m_step_1_successful = true;
                    }
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ives*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                if (stem<string_typeT>::delete_if_is_in_r2(text,/*at*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                    {
                    ic_to_iqu(text);
                    }
                if (length != text.length() )
                    {
                    m_step_1_successful = true;
                    }
                return;
                }
            //3
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ité*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E_ACUTE, common_lang_constants::UPPER_E_ACUTE, false) )
                {
                if (stem<string_typeT>::is_suffix(text,/*abil*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) )
                    {
                    if (stem<string_typeT>::get_r2() <= text.length()-4)
                        {
                        text.erase(text.length()-4);
                        stem<string_typeT>::update_r_sections(text);
                        }
                    else
                        {
                        text.replace(text.end()-2, text.end(), L"l");
                        }
                    }
                else if (stem<string_typeT>::is_suffix(text,/*ic*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C) )
                    {
                    if (stem<string_typeT>::get_r2() <= text.length()-2)
                        {
                        text.erase(text.length()-2);
                        stem<string_typeT>::update_r_sections(text);
                        }
                    else
                        {
                        text.erase(text.length()-2);
                        text += L"iq";
                        text += LOWER_U_HASH;
                        //need to search for r2 again because the 'iq' added here may change that
                        stem<string_typeT>::find_r2(text, FRENCH_VOWELS);
                        stem<string_typeT>::find_french_rv(text, FRENCH_VOWELS);
                        }
                    }
                else
                    {
                    stem<string_typeT>::delete_if_is_in_r2(text,/*iv*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V);
                    }
                if (length != text.length() )
                    {
                    m_step_1_successful = true;
                    }
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*eux*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_X, common_lang_constants::UPPER_X, false) )
                {
                if (length != text.length() )
                    {
                    m_step_1_successful = true;
                    }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*aux*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_X, common_lang_constants::UPPER_X) )
                {
                if (stem<string_typeT>::get_r1() <= (text.length()-3) )
                    {
                    text.replace(text.end()-2, text.end(), L"l");
                    stem<string_typeT>::update_r_sections(text);
                    m_step_1_successful = true;
                    }
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ive*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ifs*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_F, common_lang_constants::UPPER_F, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                if (stem<string_typeT>::delete_if_is_in_r2(text,/*at*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, false) )
                    {
                    ic_to_iqu(text);
                    }
                if (length != text.length() )
                    {
                    m_step_1_successful = true;
                    }
                return;
                }
            //2
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*if*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_F, common_lang_constants::UPPER_F, false) )
                {
                if (stem<string_typeT>::delete_if_is_in_r2(text,/*at*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, false) )
                    {
                    ic_to_iqu(text);
                    }
                if (length != text.length() )
                    {
                    m_step_1_successful = true;
                    }
                return;
                }
            }
         //---------------------------------------------
        void step_2a(string_typeT& text)
            {
            if (text.length() >= 9 &&
                stem<string_typeT>::get_rv() <= (text.length()-8) &&
                (text[text.length()-8] == common_lang_constants::LOWER_I || text[text.length()-8] == common_lang_constants::UPPER_I) &&
                (text[text.length()-7] == common_lang_constants::LOWER_S || text[text.length()-7] == common_lang_constants::UPPER_S) &&
                (text[text.length()-6] == common_lang_constants::LOWER_S || text[text.length()-6] == common_lang_constants::UPPER_S) &&
                (text[text.length()-5] == common_lang_constants::LOWER_A || text[text.length()-5] == common_lang_constants::UPPER_A) &&
                (text[text.length()-3] == common_lang_constants::LOWER_E || text[text.length()-3] == common_lang_constants::UPPER_E) &&
                (text[text.length()-2] == common_lang_constants::LOWER_N || text[text.length()-2] == common_lang_constants::UPPER_N) &&
                (text[text.length()-1] == common_lang_constants::LOWER_T || text[text.length()-1] == common_lang_constants::UPPER_T) &&
                is_either<wchar_t>(text[text.length()-4], LOWER_I_HASH, UPPER_I_HASH) )
                {
                if (stem<string_typeT>::get_rv() <= (text.length()-7) &&
                    !string_util::is_one_of(text[text.length()-9], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-8);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*issantes*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (text.length() >= 9 &&
                    stem<string_typeT>::get_rv() <= (text.length()-9) &&
                    !string_util::is_one_of(text[text.length()-9], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-8);
                    return;
                    }
                }
            else if (text.length() >= 8 &&
                stem<string_typeT>::get_rv() <= (text.length()-7) &&
                (text[text.length()-7] == common_lang_constants::LOWER_I || text[text.length()-7] == common_lang_constants::UPPER_I) &&
                (text[text.length()-6] == common_lang_constants::LOWER_R || text[text.length()-6] == common_lang_constants::UPPER_R) &&
                (text[text.length()-5] == common_lang_constants::LOWER_A || text[text.length()-5] == common_lang_constants::UPPER_A) &&
                (text[text.length()-3] == common_lang_constants::LOWER_E || text[text.length()-3] == common_lang_constants::UPPER_E) &&
                (text[text.length()-2] == common_lang_constants::LOWER_N || text[text.length()-2] == common_lang_constants::UPPER_N) &&
                (text[text.length()-1] == common_lang_constants::LOWER_T || text[text.length()-1] == common_lang_constants::UPPER_T) &&
                is_either<wchar_t>(text[text.length()-4], LOWER_I_HASH, UPPER_I_HASH) )
                {
                if (stem<string_typeT>::get_rv() <= (text.length()-8) &&
                    !string_util::is_one_of(text[text.length()-8], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-7);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*issante*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (text.length() >= 8 &&
                    stem<string_typeT>::get_rv() <= (text.length()-8) &&
                    !string_util::is_one_of(text[text.length()-8], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-7);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*issants*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (text.length() >= 8 &&
                    stem<string_typeT>::get_rv() <= (text.length()-8) &&
                    !string_util::is_one_of(text[text.length()-8], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-7);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*issions*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (text.length() >= 8 &&
                    stem<string_typeT>::get_rv() <= (text.length()-8) &&
                    !string_util::is_one_of(text[text.length()-8], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-7);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*irions*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (text.length() >= 7 &&
                    stem<string_typeT>::get_rv() <= (text.length()-7) &&
                    !string_util::is_one_of(text[text.length()-7], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-6);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*issais*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (text.length() >= 7 &&
                    stem<string_typeT>::get_rv() <= (text.length()-7) &&
                    !string_util::is_one_of(text[text.length()-7], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-6);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*issait*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                if (text.length() >= 7 &&
                    stem<string_typeT>::get_rv() <= (text.length()-7) &&
                    !string_util::is_one_of(text[text.length()-7], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-6);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*issant*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                if (text.length() >= 7 &&
                    stem<string_typeT>::get_rv() <= (text.length()-7) &&
                    !string_util::is_one_of(text[text.length()-7], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-6);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*issent*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                if (text.length() >= 7 &&
                    stem<string_typeT>::get_rv() <= (text.length()-7) &&
                    !string_util::is_one_of(text[text.length()-7], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-6);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*issiez*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z) )
                {
                if (text.length() >= 7 &&
                    stem<string_typeT>::get_rv() <= (text.length()-7) &&
                    !string_util::is_one_of(text[text.length()-7], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-6);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*issons*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (text.length() >= 7 &&
                    stem<string_typeT>::get_rv() <= (text.length()-7) &&
                    !string_util::is_one_of(text[text.length()-7], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-6);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*irais*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (text.length() >= 6 &&
                    stem<string_typeT>::get_rv() <= (text.length()-6) &&
                    !string_util::is_one_of(text[text.length()-6], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-5);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*irait*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                if (text.length() >= 6 &&
                    stem<string_typeT>::get_rv() <= (text.length()-6) &&
                    !string_util::is_one_of(text[text.length()-6], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-5);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*irent*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                if (text.length() >= 6 &&
                    stem<string_typeT>::get_rv() <= (text.length()-6) &&
                    !string_util::is_one_of(text[text.length()-6], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-5);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*iriez*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z) )
                {
                if (text.length() >= 6 &&
                    stem<string_typeT>::get_rv() <= (text.length()-6) &&
                    !string_util::is_one_of(text[text.length()-6], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-5);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*irons*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (text.length() >= 6 &&
                    stem<string_typeT>::get_rv() <= (text.length()-6) &&
                    !string_util::is_one_of(text[text.length()-6], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-5);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*iront*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                if (text.length() >= 6 &&
                    stem<string_typeT>::get_rv() <= (text.length()-6) &&
                    !string_util::is_one_of(text[text.length()-6], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-5);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*isses*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (text.length() >= 6 &&
                    stem<string_typeT>::get_rv() <= (text.length()-6) &&
                    !string_util::is_one_of(text[text.length()-6], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-5);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*issez*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z) )
                {
                if (text.length() >= 6 &&
                    stem<string_typeT>::get_rv() <= (text.length()-6)&&
                    !string_util::is_one_of(text[text.length()-6], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-5);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*îmes*/common_lang_constants::LOWER_I_CIRCUMFLEX, common_lang_constants::UPPER_I_CIRCUMFLEX, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (text.length() >= 5 &&
                    stem<string_typeT>::get_rv() <= (text.length()-5) &&
                    !string_util::is_one_of(text[text.length()-5], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-4);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*îtes*/common_lang_constants::LOWER_I_CIRCUMFLEX, common_lang_constants::UPPER_I_CIRCUMFLEX, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (text.length() >= 5 &&
                    stem<string_typeT>::get_rv() <= (text.length()-5) &&
                    !string_util::is_one_of(text[text.length()-5], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-4);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*irai*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                if (text.length() >= 5 &&
                    stem<string_typeT>::get_rv() <= (text.length()-5) &&
                    !string_util::is_one_of(text[text.length()-5], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-4);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*iras*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (text.length() >= 5 &&
                    stem<string_typeT>::get_rv() <= (text.length()-5) &&
                    !string_util::is_one_of(text[text.length()-5], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-4);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*irez*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z) )
                {
                if (text.length() >= 5 &&
                    stem<string_typeT>::get_rv() <= (text.length()-5) &&
                    !string_util::is_one_of(text[text.length()-5], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-4);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*isse*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (text.length() >= 5 &&
                    stem<string_typeT>::get_rv() <= (text.length()-5) &&
                    !string_util::is_one_of(text[text.length()-5], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-4);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*ies*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (text.length() >= 4 &&
                    stem<string_typeT>::get_rv() <= (text.length()-4) &&
                    !string_util::is_one_of(text[text.length()-4], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-3);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*ira*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                if (text.length() >= 4 &&
                    stem<string_typeT>::get_rv() <= (text.length()-4) &&
                    !string_util::is_one_of(text[text.length()-4], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-3);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text, common_lang_constants::LOWER_I_CIRCUMFLEX, common_lang_constants::UPPER_I_CIRCUMFLEX, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                if (text.length() >= 3 &&
                    stem<string_typeT>::get_rv() <= text.length()-3 &&
                    !string_util::is_one_of(text[text.length()-3], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-2);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*ie*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (text.length() >= 3 &&
                    stem<string_typeT>::get_rv() <= text.length()-3 &&
                    !string_util::is_one_of(text[text.length()-3], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-2);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*ir*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) )
                {
                if (text.length() >= 3 &&
                    stem<string_typeT>::get_rv() <= text.length()-3 &&
                    !string_util::is_one_of(text[text.length()-3], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-2);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*is*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (text.length() >= 3 &&
                    stem<string_typeT>::get_rv() <= (text.length()-3) &&
                    !string_util::is_one_of(text[text.length()-3], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-2);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*it*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                if (text.length() >= 3 &&
                    stem<string_typeT>::get_rv() <= (text.length()-3) &&
                    !string_util::is_one_of(text[text.length()-3], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-2);
                    return;
                    }
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                if (text.length() >= 2 &&
                    stem<string_typeT>::get_rv() <= (text.length()-2) &&
                    !string_util::is_one_of(text[text.length()-2], FRENCH_VOWELS) )
                    {
                    text.erase(text.length()-1);
                    return;
                    }
                }
            //only called if 2a fails to remove a suffix
            step_2b(text);
            }
        //---------------------------------------------
        void step_2b(string_typeT& text)
            {
            if (stem<string_typeT>::delete_if_is_in_rv(text,/*assions*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*assent*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, false) )
                {
                stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*assiez*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z, false) )
                {
                stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E);
                return;
                }
            else if (text.length() >= 7 &&
                stem<string_typeT>::get_rv() <= (text.length()-7) &&
                (text[text.length()-7] == common_lang_constants::LOWER_E || text[text.length()-7] == common_lang_constants::UPPER_E) &&
                (text[text.length()-6] == common_lang_constants::LOWER_R || text[text.length()-6] == common_lang_constants::UPPER_R) &&
                (text[text.length()-5] == common_lang_constants::LOWER_A || text[text.length()-5] == common_lang_constants::UPPER_A) &&
                (text[text.length()-3] == common_lang_constants::LOWER_E || text[text.length()-3] == common_lang_constants::UPPER_E) &&
                (text[text.length()-2] == common_lang_constants::LOWER_N || text[text.length()-2] == common_lang_constants::UPPER_N) &&
                (text[text.length()-1] == common_lang_constants::LOWER_T || text[text.length()-1] == common_lang_constants::UPPER_T) &&
                is_either<wchar_t>(text[text.length()-4], LOWER_I_HASH, UPPER_I_HASH) )
                {
                text.erase(text.length()-7);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erions*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (text.length() >= 5 &&
                stem<string_typeT>::get_rv() <= (text.length()-5) &&
                (text[text.length()-5] == common_lang_constants::LOWER_A || text[text.length()-5] == common_lang_constants::UPPER_A) &&
                (text[text.length()-3] == common_lang_constants::LOWER_E || text[text.length()-3] == common_lang_constants::UPPER_E) &&
                (text[text.length()-2] == common_lang_constants::LOWER_N || text[text.length()-2] == common_lang_constants::UPPER_N) &&
                (text[text.length()-1] == common_lang_constants::LOWER_T || text[text.length()-1] == common_lang_constants::UPPER_T) &&
                is_either<wchar_t>(text[text.length()-4], LOWER_I_HASH, UPPER_I_HASH) )
                {
                text.erase(text.length()-5);
                stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*antes*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*asses*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*èrent*/common_lang_constants::LOWER_E_GRAVE, common_lang_constants::UPPER_E_GRAVE, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erais*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erait*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eriez*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erons*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eront*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, false) )
                {
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_r1(text,/*ions*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) &&
                stem<string_typeT>::delete_if_is_in_r2(text,/*ions*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*âmes*/common_lang_constants::LOWER_A_CIRCUMFLEX, common_lang_constants::UPPER_A_CIRCUMFLEX, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*âtes*/common_lang_constants::LOWER_A_CIRCUMFLEX, common_lang_constants::UPPER_A_CIRCUMFLEX, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ante*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ants*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*asse*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erai*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eras*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erez*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ais*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ait*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, false) )
                {
                stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ant*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, false) )
                {
                stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ées*/common_lang_constants::LOWER_E_ACUTE, common_lang_constants::UPPER_E_ACUTE, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*era*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iez*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ât*/common_lang_constants::LOWER_A_CIRCUMFLEX, common_lang_constants::UPPER_A_CIRCUMFLEX, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, false) )
                {
                stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ai*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) )
                {
                stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*as*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ée*/common_lang_constants::LOWER_E_ACUTE, common_lang_constants::UPPER_E_ACUTE, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*és*/common_lang_constants::LOWER_E_ACUTE, common_lang_constants::UPPER_E_ACUTE, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*er*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ez*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*é*/common_lang_constants::LOWER_E_ACUTE, common_lang_constants::UPPER_E_ACUTE, false) )
                {
                return;
                }
            }
        //---------------------------------------------
        void step_3(string_typeT& text)
            {
            if (text.length() == 0)
                { return; }
            if (text[text.length()-1] == LOWER_Y_HASH)
                {
                text[text.length()-1] = common_lang_constants::LOWER_I;
                }
            else if (text[text.length()-1] == UPPER_Y_HASH)
                {
                text[text.length()-1] = common_lang_constants::UPPER_I;
                }
            else if (text[text.length()-1] == common_lang_constants::UPPER_C_CEDILLA)
                {
                text[text.length()-1] = common_lang_constants::UPPER_C;
                }
            else if (text[text.length()-1] == common_lang_constants::LOWER_C_CEDILLA)
                {
                text[text.length()-1] = common_lang_constants::LOWER_C;
                }
            }
        //---------------------------------------------
        void step_4(string_typeT& text)
            {
            if (text.length() >= 2 &&
                (text[text.length()-1] == common_lang_constants::LOWER_S || text[text.length()-1] == common_lang_constants::UPPER_S) &&
                !string_util::is_one_of(text[text.length()-2], FRENCH_AIOUES) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                }

            if (stem<string_typeT>::is_suffix_in_rv(text,/*ière*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E_GRAVE, common_lang_constants::UPPER_E_GRAVE, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                text.replace(text.end()-4, text.end(), L"i");
                stem<string_typeT>::update_r_sections(text);
                return;
                }
            else if (text.length() >= 4 &&
                stem<string_typeT>::get_rv() <= (text.length()-4) &&
                (text[text.length()-3] == common_lang_constants::LOWER_E_GRAVE || text[text.length()-3] == common_lang_constants::UPPER_E_GRAVE) &&
                (text[text.length()-2] == common_lang_constants::LOWER_R || text[text.length()-2] == common_lang_constants::UPPER_R) &&
                (text[text.length()-1] == common_lang_constants::LOWER_E || text[text.length()-1] == common_lang_constants::UPPER_E) &&
                is_either<wchar_t>(text[text.length()-4], LOWER_I_HASH, UPPER_I_HASH) )
                {
                text.replace(text.end()-4, text.end(), L"i");
                stem<string_typeT>::update_r_sections(text);
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*ier*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) )
                {
                text.replace(text.end()-3, text.end(), L"i");
                stem<string_typeT>::update_r_sections(text);
                return;
                }
            else if (text.length() >= 3 &&
                stem<string_typeT>::get_rv() <= (text.length()-3) &&
                (text[text.length()-2] == common_lang_constants::LOWER_E || text[text.length()-2] == common_lang_constants::UPPER_E) &&
                (text[text.length()-1] == common_lang_constants::LOWER_R || text[text.length()-1] == common_lang_constants::UPPER_R) &&
                is_either<wchar_t>(text[text.length()-3], LOWER_I_HASH, UPPER_I_HASH) )
                {
                text.replace(text.end()-3, text.end(), L"i");
                stem<string_typeT>::update_r_sections(text);
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*sion*/common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) ||
                stem<string_typeT>::is_suffix_in_rv(text,/*tion*/common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) )
                {
                if (text.length() >= 3 &&
                    stem<string_typeT>::get_r2() <= text.length()-3)
                    {
                    text.erase(text.length()-3);
                    stem<string_typeT>::update_r_sections(text);
                    }
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*ë*/common_lang_constants::LOWER_E_UMLAUTS, common_lang_constants::UPPER_E_UMLAUTS) )
                {
                if (text.length() >= 3 &&
                    (is_either<wchar_t>(text[text.length()-3], common_lang_constants::LOWER_G, common_lang_constants::UPPER_G) &&
                    is_either<wchar_t>(text[text.length()-2], common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) ) )
                    {
                    text.erase(text.length()-1);
                    }
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                return;
                }
            }
        //---------------------------------------------
        void step_5(string_typeT& text)
            {
            if (stem<string_typeT>::is_suffix(text,/*enn*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) ||
                stem<string_typeT>::is_suffix(text,/*onn*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) ||
                stem<string_typeT>::is_suffix(text,/*ett*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) ||
                stem<string_typeT>::is_suffix(text,/*ell*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) ||
                stem<string_typeT>::is_suffix(text,/*eill*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                }
            }
        //---------------------------------------------
        void step_6(string_typeT& text)
            {
            size_t last_vowel = text.find_last_of(FRENCH_VOWELS);
            if (last_vowel == string_typeT::npos ||
                last_vowel == text.length()-1)
                {
                return;
                }
            else if (is_either<wchar_t>(text[last_vowel], 0xE9, 0xE8) ||
                    is_either<wchar_t>(text[last_vowel], 0xC9, 0xC8) )
                {
                text[last_vowel] = common_lang_constants::LOWER_E;
                }
            }

        //internal data specific to French stemmer
        bool m_step_1_successful;
        };
    }

/** @}*/

#endif //__FRENCH_STEM_H__
