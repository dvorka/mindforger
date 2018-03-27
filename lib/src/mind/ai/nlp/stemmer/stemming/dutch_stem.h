/**@addtogroup Stemming
@brief Library for stemming words down to their root words.
@date 2003-2015
@copyright Oleander Software, Ltd.
@author Oleander Software, Ltd.
@details This program is free software; you can redistribute it and/or modify
it under the terms of the BSD License.
* @{*/

#ifndef __DUTCH_STEM_H__
#define __DUTCH_STEM_H__

#include "stemming.h"

namespace stemming
    {
    /**
    @brief Dutch stemmer.
    @date 2004
    @par Algorithm:

   Dutch includes the following accented forms:
    - ä ë ï ö ü á é í ó ú è

    <b>Step 1:</b>

    Search for the longest among the following suffixes, and perform the action indicated: 
            - heden 
                - Replace with heid if in R1.
            - en ene 
                - Delete if in R1 and preceded by a valid en-ending, and then undouble the ending.
            - s se 
                - Delete if in R1 and preceded by a valid s-ending.

    <b>Step 2:</b>

    Delete suffix e if in R1 and preceded by a non-vowel, and then undouble the ending.

    <b>Step 3a (heid):</b>

    Delete heid if in R2 and not preceded by c, and treat a preceding en as in step 1(b).

    <b>Step 3b (d-suffixes [*]):</b>
    
    Search for the longest among the following suffixes, and perform the action indicated. 
            - end ing
                - Delete if in R2.
                - If preceded by ig, delete if in R2 and not preceded by e, otherwise undouble the ending.
            - ig
                - Delete if in R2 and not preceded by e.
            - lijk
                - Delete if in R2, and then repeat step 2.
            - baar
                - Delete if in R2.
            - bar
                - Delete if in R2 and if step 2 actually removed an e.

    <b>Step 4 (undouble vowel):</b>
    
    If the words ends CVD, where C is a non-vowel, D is a non-vowel other than I,
    and V is double a, e, o, or u, then remove one of the vowels from V.
    
    (for example, maan -> man, brood -> brod).
    */
    //------------------------------------------------------
    template <typename string_typeT = std::wstring>
    class dutch_stem : public stem<string_typeT>
        {
    public:
        dutch_stem() : m_step_2_succeeded(false) {}
        //---------------------------------------------
        /**@param[in,out] text string to stem*/
        void operator()(string_typeT& text)
            {
            //First, remove all umlaut and acute accents
            stem<string_typeT>::remove_dutch_umlauts(text);
            stem<string_typeT>::remove_dutch_acutes(text);

            if (text.length() < 3)
                {
                return;
                }

            //reset internal data
            m_step_2_succeeded = false;
            stem<string_typeT>::reset_r_values();

            stem<string_typeT>::trim_western_punctuation(text);

            //Hash initial y, y after a vowel, and i between vowels
            stem<string_typeT>::hash_dutch_yi(text, DUTCH_VOWELS);

            stem<string_typeT>::find_r1(text, DUTCH_VOWELS);
            stem<string_typeT>::find_r2(text, DUTCH_VOWELS);
            //R1 must have at least 3 characters in front of it
            if (stem<string_typeT>::get_r1() < 3)
                { stem<string_typeT>::set_r1(3); }

            step_1(text);
            step_2(text);
            step_3a(text);
            step_3b(text);
            step_4(text);

            //unhash I and Y back into their original form 
            stem<string_typeT>::unhash_dutch_yi(text);
            }
    private:
        //---------------------------------------------
        void step_1(string_typeT& text)
            {
            if (stem<string_typeT>::is_suffix(text,/*heden*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-5)
                    {
                    text.erase(text.length()-1);
                    stem<string_typeT>::update_r_sections(text);
                    text[text.length()-2] = common_lang_constants::LOWER_I;
                    text[text.length()-1] = common_lang_constants::LOWER_D;
                    }
                }
            ///Define a valid en-ending as a non-vowel, and not gem.
            else if (stem<string_typeT>::is_suffix(text,/*ene*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E)    )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-3 &&
                    !string_util::is_one_of(text[text.length()-4], DUTCH_VOWELS) &&
                    (text.length() < 6 ||
                    //"gem" in front of "en" ending
                    !(is_either<wchar_t>(text[text.length()-6], common_lang_constants::LOWER_G, common_lang_constants::UPPER_G) &&
                    is_either<wchar_t>(text[text.length()-5], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[text.length()-4], common_lang_constants::LOWER_M, common_lang_constants::UPPER_M) ) ) )
                    {
                    text.erase(text.length()-3);
                    //undouble dd, kk, tt
                    if (string_util::is_one_of(text[text.length()-1], DUTCH_KDT) &&
                        string_util::tolower_western(text[text.length()-2]) == string_util::tolower_western(text[text.length()-1]))
                        { text.erase(text.length()-1); }
                    stem<string_typeT>::update_r_sections(text);
                    }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*en*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-2 &&
                    !string_util::is_one_of(text[text.length()-3], DUTCH_VOWELS) &&
                    (text.length() < 5 ||
                    !(is_either<wchar_t>(text[text.length()-5], common_lang_constants::LOWER_G, common_lang_constants::UPPER_G) &&
                    is_either<wchar_t>(text[text.length()-4], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[text.length()-3], common_lang_constants::LOWER_M, common_lang_constants::UPPER_M) ) ) )
                    {
                    text.erase(text.length()-2);
                    //undouble dd, kk, tt
                    if (string_util::is_one_of(text[text.length()-1], DUTCH_KDT) &&
                        string_util::tolower_western(text[text.length()-2]) == string_util::tolower_western(text[text.length()-1]) )
                        { text.erase(text.length()-1); }
                    stem<string_typeT>::update_r_sections(text);
                    }
                return;
                }
            else if (text.length() >= 3 &&
                stem<string_typeT>::is_suffix(text,/*se*/common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                !string_util::is_one_of(text[text.length()-3], DUTCH_S_ENDING))
                {
                if (stem<string_typeT>::get_r1() <= text.length()-2)
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    return;
                    }
                }
            ///Define a valid s-ending as a non-vowel other than j
            else if (text.length() >= 2 &&
                stem<string_typeT>::is_suffix(text, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) &&
                !string_util::is_one_of(text[text.length()-2], DUTCH_S_ENDING))
                {
                if (stem<string_typeT>::get_r1() <= text.length()-1)
                    {
                    text.erase(text.length()-1);
                    stem<string_typeT>::update_r_sections(text);
                    return;
                    }
                }
            }

        //---------------------------------------------
        void step_2(string_typeT& text)
            {
            if (stem<string_typeT>::is_suffix(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (text.length() >= 2 &&
                    stem<string_typeT>::get_r1() <= text.length()-1 &&
                    !string_util::is_one_of(text[text.length()-2], DUTCH_VOWELS) )
                    {
                    //watch out for vowel I/Y vowel
                    if (text.length() >= 3 &&
                        string_util::is_one_of(text[text.length()-2], L"iyIY") &&
                        string_util::is_one_of(text[text.length()-3], DUTCH_VOWELS) )
                        { return; }
                    else
                        {
                        text.erase(text.length()-1);
                        //undouble dd, kk, tt
                        if (string_util::is_one_of(text[text.length()-1], DUTCH_KDT) &&
                            string_util::tolower_western(text[text.length()-2]) == string_util::tolower_western(text[text.length()-1]))
                            { text.erase(text.length()-1); }
                        stem<string_typeT>::update_r_sections(text);
                        m_step_2_succeeded = true;
                        }
                    }
                }
            }
        //---------------------------------------------
        void step_3a(string_typeT& text)
            {
            if (stem<string_typeT>::is_suffix(text,/*heid*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) )
                {
                if (text.length() >= 5 &&
                    stem<string_typeT>::get_r2() <= text.length()-4 &&
                    is_neither<wchar_t>(text[text.length()-5], common_lang_constants::LOWER_C, common_lang_constants::UPPER_C) )
                    {
                    text.erase(text.length()-4);
                    stem<string_typeT>::update_r_sections(text);
                    if (stem<string_typeT>::get_r1() <= text.length()-2 &&
                        stem<string_typeT>::is_suffix(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) )
                        {
                        if ((!string_util::is_one_of(text[text.length()-3], DUTCH_VOWELS) ||
                            (string_util::is_one_of(text[text.length()-3], L"iyYI") &&
                            string_util::is_one_of(text[text.length()-4], DUTCH_VOWELS))) &&
                            (text.length() < 5 ||
                            !(is_either<wchar_t>(text[text.length()-5], common_lang_constants::LOWER_G, common_lang_constants::UPPER_G) &&
                            is_either<wchar_t>(text[text.length()-4], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                            is_either<wchar_t>(text[text.length()-3], common_lang_constants::LOWER_M, common_lang_constants::UPPER_M) ) ) )
                            {
                            text.erase(text.length()-2);
                            //undouble dd, kk, tt
                            if (string_util::is_one_of(text[text.length()-1], DUTCH_KDT) &&
                                string_util::tolower_western(text[text.length()-2]) == string_util::tolower_western(text[text.length()-1]))
                                { text.erase(text.length()-1); }
                            stem<string_typeT>::update_r_sections(text);
                            }
                        }
                    return;
                    }
                }
            }
        //---------------------------------------------
        void step_3b(string_typeT& text)
            {
            if (stem<string_typeT>::delete_if_is_in_r2(text,/*end*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) ||
                stem<string_typeT>::delete_if_is_in_r2(text,/*ing*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G) )
                {
                stem<string_typeT>::update_r_sections(text);
                if (text.length() > 3)
                    {
                    if (is_neither<wchar_t>(text[text.length()-3], common_lang_constants::LOWER_E , common_lang_constants::UPPER_E) &&
                        stem<string_typeT>::delete_if_is_in_r2(text,/*ig*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G) )
                        {
                        stem<string_typeT>::update_r_sections(text);
                        return;
                        }
                    else
                        {
                        if (string_util::is_one_of(text[text.length()-1], DUTCH_KDT) &&
                            string_util::tolower_western(text[text.length()-2]) == string_util::tolower_western(text[text.length()-1]))
                            {
                            text.erase(text.length()-1);
                            stem<string_typeT>::update_r_sections(text);
                            return;
                            }
                        }
                    }
                return;
                }
            else if (!(text.length() >= 3 && is_either<wchar_t>(text[text.length()-3], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ) &&
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ig*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G) )
                {
                stem<string_typeT>::update_r_sections(text);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*baar*/common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*lijk*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_J, common_lang_constants::UPPER_J, common_lang_constants::LOWER_K, common_lang_constants::UPPER_K) )
                {
                step_2(text);
                return;
                }
            else if (m_step_2_succeeded &&
                stem<string_typeT>::delete_if_is_in_r2(text,/*bar*/common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) )
                {
                return;
                }
            }
        //------------------------------------------------------
        void step_4(string_typeT& text)
            {
            //undouble consecutive (same) consonants
            if (text.length() >= 4 &&
                !string_util::is_one_of(text[text.length()-4], DUTCH_VOWELS) &&
                !string_util::is_one_of(text[text.length()-1], DUTCH_VOWELS) &&
                text[text.length()-1] != LOWER_I_HASH &&
                text[text.length()-1] != UPPER_I_HASH &&
                string_util::is_one_of(text[text.length()-2], L"aeouAEOU") &&
                string_util::tolower_western(text[text.length()-2]) == string_util::tolower_western(text[text.length()-3]) )
                {
                text.erase(text.end()-2, text.end()-1);
                stem<string_typeT>::update_r_sections(text);
                }
            }
        //internal data specific to Dutch stemmer
        bool m_step_2_succeeded;
        };
    }

/** @}*/

#endif //__DUTCH_STEM_H__
