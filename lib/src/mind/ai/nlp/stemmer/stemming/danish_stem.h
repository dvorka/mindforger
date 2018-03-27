/**@addtogroup Stemming
@brief Library for stemming words down to their root words.
@date 2003-2015
@copyright Oleander Software, Ltd.
@author Oleander Software, Ltd.
@details This program is free software; you can redistribute it and/or modify
it under the terms of the BSD License.
* @{*/
#ifndef __DANISH_STEM_H__
#define __DANISH_STEM_H__

#include "stemming.h"

namespace stemming
    {
    /**
    @brief Danish stemmer.
    @date 2004
    @par Algorithm:

    The Danish alphabet includes the following additional letters:
    - æ å ø

    The following letters are vowels:
    - a e i o u y æ å ø

    A consonant is defined as a non-vowel.

    R2 is not used: R1 is defined in the same way as in the German stemmer.

    Define a valid s-ending as one of 
    - a b c d f g h j k l m n o p r t v y z å

    <b>Step 1:</b>

    Search for the longest among the following suffixes in R1, and perform the action indicated: 
            - hed ethed ered e erede ende erende ene erne ere en heden
               eren er heder erer heds es endes erendes enes ernes eres
               ens hedens erens ers ets erets et eret.
               - Delete.
            - s
               - Delete, if preceded by a valid s-ending.

    (Of course the letter of the valid s-ending is not necessarily in R1).

    <b>Step 2:</b>

    Search for one of the following suffixes in R1, and if found delete the last letter.:
            - gd dt gt kt

    (For example, friskt -> frisk).

    <b>Step 3:</b>

    If the word ends with igst, remove the final st. Search for the longest among the following suffixes in R1, and perform the action indicated:
            - ig lig elig els
                - Delete, and then repeat step 2.
            - løst
                - Replace with løs.

    <b>Step 4 (undouble):</b>

    If the word ends with double consonant in R1, then remove one of the consonants. 

    (For example, bestemmelse -> bestemmels (step 1) -> bestemm (step 3a) -> bestem in this step).

    @par Example:
    \code
    std::wstring word(L"ramningen");
    stemming::danish_stem<myString> StemDanish;
    StemDanish(word);
    \endcode
    */
    /** @} */
    //------------------------------------------------------
    template <typename string_typeT = std::wstring>
    class danish_stem : public stem<string_typeT>
        {
    public:
        //---------------------------------------------
        /**@param[in,out] text Danish string to stem.*/
        void operator()(string_typeT& text)
            {
            if (text.length() < 3)
                { return; }

            //reset internal data
            stem<string_typeT>::reset_r_values();

            stem<string_typeT>::trim_western_punctuation(text);

            //see where the R1 section begins
            //R1 is the first consonant after the first vowel
            stem<string_typeT>::find_r1(text, DANISH_VOWELS);
            if (stem<string_typeT>::get_r1() == text.length() )
                { return; }
            //R1 must have at least 3 characters in front of it
            if (stem<string_typeT>::get_r1() < 3)
                { stem<string_typeT>::set_r1(3); }
            //danish does not use R2

            step_1(text);
            step_2(text);
            step_3(text);
            step_4(text);
            }
    private:
        //---------------------------------------------
        void step_1(string_typeT& text)
            {
            if (stem<string_typeT>::delete_if_is_in_r1(text,/*erendes*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                { return; }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*erende*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                { return; }    
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*hedens*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ethed*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, false) )
                { return; }    
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*erede*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                { return; }    
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*heden*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*heder*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, false) )
                { return; }     
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*endes*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ernes*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*erens*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                { return; }    
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*erets*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                { return; }    
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*eres*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*enes*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*heds*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*erer*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, false) )
                { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*eren*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*erne*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
               { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ende*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ered*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, false) )
                { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*eret*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, false) )
                { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*hed*/common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, false) )
                { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ets*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ere*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ene*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ens*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ers*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*et*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, false) )
                { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*es*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*er*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, false) )
                { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*en*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                { return; } 
            else if (stem<string_typeT>::delete_if_is_in_r1(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                { return; } 
            else if (stem<string_typeT>::is_suffix_in_r1(text, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (text.length() >= 2 &&
                    string_util::is_one_of(text[text.length()-2], DANISH_ALPHABET) )
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
            if (stem<string_typeT>::is_suffix_in_r1(text,/*gd*/common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_r1(text,/*dt*/common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_r1(text,/*gt*/common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_r1(text,/*kt*/common_lang_constants::LOWER_K, common_lang_constants::UPPER_K, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                return;
                }
            }
        //---------------------------------------------
        void step_3(string_typeT& text)
            {
            //do this check separately
            if (stem<string_typeT>::is_suffix(text,/*igst*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                text.erase(text.length()-2);
                stem<string_typeT>::update_r_sections(text);
                }
            //now start looking for the longest suffix
            if (stem<string_typeT>::delete_if_is_in_r1(text,/*elig*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, false) )
                {
                step_2(text);
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_r1(text,/*løst*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O_STROKE, common_lang_constants::UPPER_O_STROKE, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*lig*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, false) )
                {
                step_2(text);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*els*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                step_2(text);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*ig*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, false) )
                {
                step_2(text);
                return;
                }
            }
        //---------------------------------------------
        void step_4(string_typeT& text)
            {
            //undouble consecutive (same) consonants if either are in R1 section
            if (text.length() >= 2 &&
                stem<string_typeT>::get_r1() <= text.length()-1 &&
                string_util::tolower_western(text[text.length()-2]) == string_util::tolower_western(text[text.length()-1]) )
                {
                if (!string_util::is_one_of(text[text.length()-2], DANISH_VOWELS) )
                    {
                    text.erase(text.length()-1);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            }
        };
    }

/** @}*/

#endif //__DANISH_STEM_H__
