/**@addtogroup Stemming
@brief Library for stemming words down to their root words.
@date 2003-2015
@copyright Oleander Software, Ltd.
@author Oleander Software, Ltd.
@details This program is free software; you can redistribute it and/or modify
it under the terms of the BSD License.
* @{*/

#ifndef __SWEDISH_STEM_H__
#define __SWEDISH_STEM_H__

#include "stemming.h"

namespace stemming
    {
    /**
    @brief Swedish stemmer.

    @par Algorithm:

    The Swedish alphabet includes the following additional letters,
        - ä   å   ö

    The following letters are vowels:
        - a   e   i   o   u   y   ä   å   ö

    R2 is not used: R1 is defined in the same way as in the German stemmer.

    Define a valid s-ending as one of:
        - b c d f g h j k l m n o p r t v y
    
    @par Algorithm:

    <b>Step 1:</b>

    Search for the longest among the following suffixes in R1, and perform the action indicated.
        - a   arna   erna   heterna   orna   ad   e   ade   ande   arne   are   aste   en   anden
              aren   heten   ern   ar   er   heter   or   as   arnas   ernas   ornas   es   ades
              andes   ens   arens   hetens   erns   at   andet   het   ast
            - Delete.
        - s
         - Delete if preceded by a valid s-ending.

    (Of course the letter of the valid s-ending is not necessarily in R1).

    <b>Step 2:</b>

    Search for one of the following suffixes in R1, and if found delete the last letter. 
        - dd   gd   nn   dt   gt   kt   tt 

    (For example, friskt -> frisk, fröknarnn -> fröknarn).
    
    <b>Step 2:</b>

    Search for the longest among the following suffixes in R1, and perform the action indicated.
        - lig   ig   els 
            - Delete 
        - löst 
            - Replace with lös 
        - fullt 
            - Replace with full
    */
    //------------------------------------------------------
    template <typename string_typeT = std::wstring>
    class swedish_stem : public stem<string_typeT>
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

            //reset internal data
            stem<string_typeT>::reset_r_values();

            stem<string_typeT>::trim_western_punctuation(text);

            //see where the R1 section begins
            //R1 is the first consonant after the first vowel
            stem<string_typeT>::find_r1(text, SWEDISH_VOWELS);
            if (stem<string_typeT>::get_r1() == text.length())
                {
                return;
                }

            //R1 must have at least 3 characters in front of it
            if (stem<string_typeT>::get_r1() < 3)
                {
                stem<string_typeT>::set_r1(3);    
                }
            //swedish does not use R2

            step_1(text);
            step_2(text);
            step_3(text);
            }
    private:
        //---------------------------------------------
        void step_1(string_typeT& text)
            {
            if (stem<string_typeT>::delete_if_is_in_r1(text,/*heterna*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*hetens*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*arna*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*erna*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*orna*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }    
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ande*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*arne*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*aste*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*anden*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*heten*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*heter*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*arnas*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ernas*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ornas*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*arens*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*andet*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*andes*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*aren*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text, /*erns*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ades*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*are*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ade*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
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
            else if (stem<string_typeT>::delete_if_is_in_r1(text, /*ens*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ern*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*at*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*es*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*or*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ad*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, false) )
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
                if (text.length() >= 2 &&
                    string_util::is_one_of(text[text.length()-2],
                    L"bcdfghjklmnoprtvyBCDFGHJKLMNOPRTVY") )
                    {
                    text.erase(text.length()-1);
                    stem<string_typeT>::update_r_sections(text);
                    }
                return;
                }
            }
        //---------------------------------------------
        void step_2(string_typeT& text)
            {
            if (stem<string_typeT>::is_suffix_in_r1(text,/*dd*/common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                }
            else if (stem<string_typeT>::is_suffix_in_r1(text,/*gd*/common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                }
            else if (stem<string_typeT>::is_suffix_in_r1(text,/*nn*/common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                }
            else if (stem<string_typeT>::is_suffix_in_r1(text,/*dt*/common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                }
            else if (stem<string_typeT>::is_suffix_in_r1(text,/*gt*/common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                }
            else if (stem<string_typeT>::is_suffix_in_r1(text,/*kt*/common_lang_constants::LOWER_K, common_lang_constants::UPPER_K, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                }
            else if (stem<string_typeT>::is_suffix_in_r1(text,/*tt*/common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                }
            }
        //---------------------------------------------
        void step_3(string_typeT& text)
            {
            if (stem<string_typeT>::is_suffix_in_r1(text,/*fullt*/common_lang_constants::LOWER_F, common_lang_constants::UPPER_F, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                }
            else if (stem<string_typeT>::is_suffix_in_r1(text,/*löst*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O_UMLAUTS, common_lang_constants::UPPER_O_UMLAUTS, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*lig*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*els*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
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

#endif //__SWEDISH_STEM_H__
