/**@addtogroup Stemming
@brief Library for stemming words down to their root words.
@date 2003-2015
@copyright Oleander Software, Ltd.
@author Oleander Software, Ltd.
@details This program is free software; you can redistribute it and/or modify
it under the terms of the BSD License.
* @{*/

#ifndef __NORWEGIAN_STEM_H__
#define __NORWEGIAN_STEM_H__

#include "stemming.h"

namespace stemming
    {
    /**
    @brief Norwegian stemmer.
    @date 2004
    @par Algorithm:

    The Norwegian alphabet includes the following additional letters:
        - æ   å   ø

    The following letters are vowels:
        - a   e   i   o   u   y   æ   å   ø

    R2 is not used: R1 is defined in the same way as in the German stemmer.

    Define a valid s-ending as one of
        - b   c   d   f   g   h   j   l   m   n   o   p   r   t   v   y   z,
        - or 'k', not preceded by a vowel.
    
    @par Algorithm:

    <b>Step 1:</b>

    Search for the longest among the following suffixes in R1, and perform the action indicated: 
        - a e ede ande ende ane ene hetene en heten ar er heter as es edes
          endes enes hetenes ens hetens ers ets et het ast 
            - Delete.
        - s 
            - Delete if preceded by a valid s-ending.

        - erte   ert 
            - Replace with "er".

        (Of course the letter of the valid s-ending is not necessarily in R1).

    <b>Step 2:</b>

    If the word ends dt or vt in R1, then delete the 't'.
    (For example, meldt -> meld, operativt -> operativ).

    <b>Step 3:</b>

    Search for the longest among the following suffixes in R1, and if found, delete:
        - leg eleg ig eig lig elig els lov elov slov hetslov
    */
    //------------------------------------------------------
    template <typename string_typeT = std::wstring>
    class norwegian_stem : public stem<string_typeT>
        {
    public:
        //---------------------------------------------
        /**@param[in,out] text string to stem*/
        void operator()(string_typeT& text)
            {
            if (text.length() < 3)
                {
                return;
                }
            stem<string_typeT>::trim_western_punctuation(text);

            //reset internal data
            stem<string_typeT>::reset_r_values();

            stem<string_typeT>::find_r1(text, NORWEGIAN_VOWELS);
            if (stem<string_typeT>::get_r1() == text.length() )
                {
                return;
                }
            //R1 must have at least 3 characters in front of it
            if (stem<string_typeT>::get_r1() < 3)
                {
                stem<string_typeT>::set_r1(3);
                }
            //norwegian does not use R2

            step_1(text);
            step_2(text);
            step_3(text);
            }
    private:
        //---------------------------------------------
        void step_1(string_typeT& text)
            {
            if (stem<string_typeT>::delete_if_is_in_r1(text,/*hetenes*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*hetene*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*hetens*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*heter*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*heten*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*endes*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ande*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ende*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*edes*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*enes*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            if (stem<string_typeT>::is_suffix_in_r1(text,/*erte*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                text.erase(text.length()-2);
                stem<string_typeT>::update_r_sections(text);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ers*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ets*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*het*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ast*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ens*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ene*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ane*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ede*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_r1(text,/*ert*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*et*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*es*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*as*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ar*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*er*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*en*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_r1(text, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-1)
                    {
                    //only delete if a valid "s" ending
                    if (text.length() >= 2 &&
                        string_util::is_one_of(text[text.length()-2],
                        L"bcdfghjlmnoprtvyzBCDFGHJLMNOPRTVYZ") )
                        {
                        text.erase(text.length()-1);
                        stem<string_typeT>::update_r_sections(text);
                        return;
                        }
                    else if (text.length() >= 3 &&
                        is_either<wchar_t>(text[text.length()-2], common_lang_constants::LOWER_K, common_lang_constants::UPPER_K) &&
                        !string_util::is_one_of(text[text.length()-3], NORWEGIAN_VOWELS))
                        {
                        text.erase(text.length()-1);
                        stem<string_typeT>::update_r_sections(text);
                        return;
                        }
                    }
                }
            }

        //---------------------------------------------
        void step_2(string_typeT& text)
            {
            if (stem<string_typeT>::is_suffix_in_r1(text,/*dt*/common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_r1(text,/*vt*/common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                return;
                }
            }
        //---------------------------------------------
        void step_3(string_typeT& text)
            {
            if (stem<string_typeT>::delete_if_is_in_r1(text,/*hetslov*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*slov*/common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*elov*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*elig*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*eleg*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*lov*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*els*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*lig*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*eig*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*leg*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ig*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, false) )
                {
                return;
                }
            }
        };
    }

/** @}*/

#endif //__NORWEGIAN_STEM_H__
