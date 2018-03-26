/**@addtogroup Stemming
@brief Library for stemming words down to their root words.
@date 2003-2015
@copyright Oleander Software, Ltd.
@author Oleander Software, Ltd.
@details This program is free software; you can redistribute it and/or modify
it under the terms of the BSD License.
* @{*/

#ifndef __SPANISH_STEM_H__
#define __SPANISH_STEM_H__

#include "stemming.h"

namespace stemming
    {
    /**
    @brief Spanish stemmer.

    @par Algorithm:

    Letters in Spanish include the following accented forms:
        - á é í ó ú ü ñ
    
    The following letters are vowels:
        - a e i o u á é í ó ú ü
    
    R2 is defined in the usual way - see the note on R1 and R2.

    RV is defined as follows (and this is not the same as the French stemmer definition):

    If the second letter is a consonant, RV is the region after the next following vowel,
    or if the first two letters are vowels, RV is the region after the next consonant,
    and otherwise (consonant-vowel case) RV is the region after the third letter.
    But RV is the end of the word if these positions cannot be found.

    For example,

        m a c h o     o l i v a     t r a b a j o     á u r e o
             |...|         |...|         |.......|         |...|

    @par Algorithm:

    Always do steps 0 and 1.

    <b>Step 0:</b>

    Search for the longest among the following suffixes
        - me se sela selo selas selos la le lo las les los nos
            - Delete it, if comes after one of 
                - iéndo ándo ár ér ír
                - ando iendo ar er ir
                - yendo, following u

    in RV. In the case of (c), yendo must lie in RV, but the preceding u can be outside it.

    In the case of (a), deletion is followed by removing the acute accent
    (for example, haciéndola -> haciendo).

    <b>Step 1:</b>

    Search for the longest among the following suffixes, and perform the action indicated.
        - anza anzas ico ica icos icas ismo ismos able ables ible ibles ista istas oso osa osos osas amiento amientos imiento imientos
            - Delete if in R2.
        - adora ador ación adoras adores aciones ante antes ancia ancias
            - Delete if in R2.
            - If preceded by ic, delete if in R2
        - logía logías
            - Replace with log if in R2.
        - ución uciones
            - Replace with u if in R2.
        - encia encias
            - Replace with ente if in R2.
        - amente
            - Delete if in R1.
            - If preceded by iv, delete if in R2 (and if further preceded by at, delete if in R2), otherwise,
            - If preceded by os, ic or ad, delete if in R2.
        - mente
            - Delete if in R2.
            - If preceded by ante, able or ible, delete if in R2.
        - idad idades
            - Delete if in R2
            - If preceded by abil, ic or iv, delete if in R2.
        - iva ivo ivas ivos
            - Delete if in R2.
            - If preceded by at, delete if in R2.

    Do step 2(a) if no ending was removed by step 1.

    <b>Step 2(a):</b>

    Search for the longest among the following suffixes in RV, and if found, delete if preceded by u.
        - ya ye yan yen yeron yendo yo yó yas yes yais yamos

    (Note that the preceding u need not be in RV).

    <b>Step 2(b):</b>

    Do Step 2(b) if step 2(a) was done, but failed to remove a suffix.

    Search for the longest among the following suffixes in RV, and perform the action indicated.
            - en es éis emos
                - Delete, and if preceded by gu delete the u (the gu need not be in RV).
            - arían arías arán arás aríais aría aréis aríamos aremos ará aré erían erías
            erán erás eríais ería eréis eríamos eremos erá eré irían irías irán irás iríais
            iría iréis iríamos iremos irá iré aba ada ida ía ara iera ad ed id ase iese aste
            iste an aban ían aran ieran asen iesen aron ieron ado ido ando iendo ió ar er ir
            as abas adas idas ías aras ieras ases ieses ís áis abais íais arais ierais
            aseis ieseis asteis isteis ados idos amos ábamos íamos imos áramos iéramos iésemos ásemos
                - Delete.

    Always do step 3.

    <b>Step 3:</b>

    Search for the longest among the following suffixes in RV, and perform the action indicated.
            - os a o á í ó
                - Delete if in RV.
            - e é
                - Delete if in RV, and if preceded by gu with the u in RV delete the u.
    */
    //------------------------------------------------------
    template <typename string_typeT = std::wstring>
    class spanish_stem : public stem<string_typeT>
        {
    public:
        //---------------------------------------------
        /**@param[in,out] text string to stem*/
        void operator()(string_typeT& text)
            {
            if (text.length() < 3)
                {
                stem<string_typeT>::remove_spanish_acutes(text);
                return;
                }

            //reset internal data
            stem<string_typeT>::reset_r_values();

            stem<string_typeT>::trim_western_punctuation(text);

            stem<string_typeT>::find_r1(text, SPANISH_VOWELS);
            stem<string_typeT>::find_r2(text, SPANISH_VOWELS);
            stem<string_typeT>::find_spanish_rv(text, SPANISH_VOWELS);

            step_0(text);
            step_1(text);
            ///steps 2a and 2b and only called from step1
            step_3(text);

            stem<string_typeT>::remove_spanish_acutes(text);
            }
    private:
        //---------------------------------------------
        void step_0(string_typeT& text)
            {
            if (stem<string_typeT>::is_suffix_in_rv(text,/*selos*/common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S))
                {
                if (step_0a(text, 5))
                    { return; }
                if (step_0b(text, 5))
                    { return; }
                if (step_0c(text, 5))
                    { return; }
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*selas*/common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S))
                {
                if (step_0a(text, 5))
                    { return; }
                if (step_0b(text, 5))
                    { return; }
                if (step_0c(text, 5))
                    { return; }
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*sela*/common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A))
                {
                if (step_0a(text, 4))
                    { return; }
                if (step_0b(text, 4))
                    { return; }
                if (step_0c(text, 4))
                    { return; }
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*selo*/common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O))
                {
                if (step_0a(text, 4))
                    { return; }
                if (step_0b(text, 4))
                    { return; }
                if (step_0c(text, 4))
                    { return; }
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*las*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S))
                {
                if (step_0a(text, 3))
                    { return; }
                if (step_0b(text, 3))
                    { return; }
                if (step_0c(text, 3))
                    { return; }
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*les*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S))
                {
                if (step_0a(text, 3))
                    { return; }
                if (step_0b(text, 3))
                    { return; }
                if (step_0c(text, 3))
                    { return; }
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*los*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S))
                {
                if (step_0a(text, 3))
                    { return; }
                if (step_0b(text, 3))
                    { return; }
                if (step_0c(text, 3))
                    { return; }
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*nos*/common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S))
                {
                if (step_0a(text, 3))
                    { return; }
                if (step_0b(text, 3))
                    { return; }
                if (step_0c(text, 3))
                    { return; }
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*la*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A))
                {
                if (step_0a(text, 2))
                    { return; }
                if (step_0b(text, 2))
                    { return; }
                if (step_0c(text, 2))
                    { return; }
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*le*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E))
                {
                if (step_0a(text, 2))
                    { return; }
                if (step_0b(text, 2))
                    { return; }
                if (step_0c(text, 2))
                    { return; }
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*lo*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O))
                {
                if (step_0a(text, 2))
                    { return; }
                if (step_0b(text, 2))
                    { return; }
                if (step_0c(text, 2))
                    { return; }
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*me*/common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E))
                {
                if (step_0a(text, 2))
                    { return; }
                if (step_0b(text, 2))
                    { return; }
                if (step_0c(text, 2))
                    { return; }
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*se*/common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E))
                {
                if (step_0a(text, 2))
                    { return; }
                if (step_0b(text, 2))
                    { return; }
                if (step_0c(text, 2))
                    { return; }
                return;
                }
            }
        //---------------------------------------------
        bool step_0a(string_typeT& text, size_t suffix_length)
            {
            if ((text.length() >= suffix_length+5) &&
                stem<string_typeT>::get_rv() <= (text.length()-(5+suffix_length)) &&
                /*iéndo*/
                (is_either<wchar_t>(text[text.length()-(5+suffix_length)], common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) &&
                    is_either<wchar_t>(text[text.length()-(4+suffix_length)], common_lang_constants::LOWER_E_ACUTE, common_lang_constants::UPPER_E_ACUTE) &&
                    is_either<wchar_t>(text[text.length()-(3+suffix_length)], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[text.length()-(2+suffix_length)], common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) &&
                    is_either<wchar_t>(text[text.length()-(1+suffix_length)], common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) ) )
                {
                text.erase(text.length()-suffix_length);
                text[text.length()-4] = common_lang_constants::LOWER_E;
                stem<string_typeT>::update_r_sections(text);
                return true;
                }
            else if ((text.length() >= suffix_length+4) &&
                stem<string_typeT>::get_rv() <= (text.length()-(4+suffix_length)) &&
                /*ándo*/
                (is_either<wchar_t>(text[text.length()-(4+suffix_length)], common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE) &&
                    is_either<wchar_t>(text[text.length()-(3+suffix_length)], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[text.length()-(2+suffix_length)], common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) &&
                    is_either<wchar_t>(text[text.length()-(1+suffix_length)], common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) ) )
                {
                text.erase(text.length()-suffix_length);
                text[text.length()-4] = common_lang_constants::LOWER_A;
                stem<string_typeT>::update_r_sections(text);
                return true;
                }
            else if ((text.length() >= suffix_length+2) &&
                stem<string_typeT>::get_rv() <= (text.length()-(2+suffix_length)) &&
                /*ár*/
                (is_either<wchar_t>(text[text.length()-(2+suffix_length)], common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE) &&
                    is_either<wchar_t>(text[text.length()-(1+suffix_length)], common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) ) )
                {
                text.erase(text.length()-suffix_length);
                text[text.length()-2] = common_lang_constants::LOWER_A;
                stem<string_typeT>::update_r_sections(text);
                return true;
                }
            else if ((text.length() >= suffix_length+2) &&
                stem<string_typeT>::get_rv() <= (text.length()-(2+suffix_length)) &&
                /*ér*/
                (is_either<wchar_t>(text[text.length()-(2+suffix_length)], common_lang_constants::LOWER_E_ACUTE, common_lang_constants::UPPER_E_ACUTE) &&
                    is_either<wchar_t>(text[text.length()-(1+suffix_length)], common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) ) )
                {
                text.erase(text.length()-suffix_length);
                text[text.length()-2] = common_lang_constants::LOWER_E;
                stem<string_typeT>::update_r_sections(text);
                return true;
                }
            else if ((text.length() >= suffix_length + 2) &&
                stem<string_typeT>::get_rv() <= (text.length()-(2+suffix_length)) &&
                /*ír*/
                (is_either<wchar_t>(text[text.length()-(2+suffix_length)], common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE) &&
                    is_either<wchar_t>(text[text.length()-(1+suffix_length)], common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) ) )
                {
                text.erase(text.length()-suffix_length);
                text[text.length()-2] = common_lang_constants::LOWER_I;
                stem<string_typeT>::update_r_sections(text);
                return true;
                }
            return false;
            }

        //---------------------------------------------
        bool step_0b(string_typeT& text, size_t suffix_length)
            {
            if ((text.length() >= suffix_length+5) &&
                stem<string_typeT>::get_rv() <= (text.length()-(5+suffix_length)) &&
                /*iendo*/
                (is_either<wchar_t>(text[text.length()-(5+suffix_length)], common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) &&
                    is_either<wchar_t>(text[text.length()-(4+suffix_length)], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[text.length()-(3+suffix_length)], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[text.length()-(2+suffix_length)], common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) &&
                    is_either<wchar_t>(text[text.length()-(1+suffix_length)], common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) ) )
                {
                text.erase(text.length()-suffix_length);
                stem<string_typeT>::update_r_sections(text);
                return true;
                }
            else if ((text.length() >= suffix_length+4) &&
                stem<string_typeT>::get_rv() <= (text.length()-(4+suffix_length)) &&
                /*ando*/
                (is_either<wchar_t>(text[text.length()-(4+suffix_length)], common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) &&
                    is_either<wchar_t>(text[text.length()-(3+suffix_length)], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[text.length()-(2+suffix_length)], common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) &&
                    is_either<wchar_t>(text[text.length()-(1+suffix_length)], common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) ) )
                {
                text.erase(text.length()-suffix_length);
                stem<string_typeT>::update_r_sections(text);
                return true;
                }
            else if ((text.length() >= suffix_length+2) &&
                stem<string_typeT>::get_rv() <= (text.length()-(2+suffix_length)) &&
                /*ar*/
                (is_either<wchar_t>(text[text.length()-(2+suffix_length)], common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) &&
                    is_either<wchar_t>(text[text.length()-(1+suffix_length)], common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) ) )
                {
                text.erase(text.length()-suffix_length);
                stem<string_typeT>::update_r_sections(text);
                return true;
                }
            else if ((text.length() >= suffix_length+2) &&
                stem<string_typeT>::get_rv() <= (text.length()-(2+suffix_length)) &&
                /*er*/
                (is_either<wchar_t>(text[text.length()-(2+suffix_length)], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[text.length()-(1+suffix_length)], common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) ) )
                {
                text.erase(text.length()-suffix_length);
                stem<string_typeT>::update_r_sections(text);
                return true;
                }
            else if ((text.length() >= suffix_length+2) &&
                stem<string_typeT>::get_rv() <= (text.length()-(2+suffix_length)) &&
                /*ir*/
                (is_either<wchar_t>(text[text.length()-(2+suffix_length)], common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) &&
                    is_either<wchar_t>(text[text.length()-(1+suffix_length)], common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) ) )
                {
                text.erase(text.length()-suffix_length);
                stem<string_typeT>::update_r_sections(text);
                return true;
                }
            return false;
            }

        //---------------------------------------------
        bool step_0c(string_typeT& text, size_t suffix_length)
            {
            if ((text.length() >= suffix_length+6) &&
                stem<string_typeT>::get_rv() <= text.length()-(suffix_length+5) &&
                /*uyendo*/
                (is_either<wchar_t>(text[text.length()-(6+suffix_length)], common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) &&
                    is_either<wchar_t>(text[text.length()-(5+suffix_length)], common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y) &&
                    is_either<wchar_t>(text[text.length()-(4+suffix_length)], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                    is_either<wchar_t>(text[text.length()-(3+suffix_length)], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                    is_either<wchar_t>(text[text.length()-(2+suffix_length)], common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) &&
                    is_either<wchar_t>(text[text.length()-(1+suffix_length)], common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) ) )
                {
                text.erase(text.length()-suffix_length);
                stem<string_typeT>::update_r_sections(text);
                return true;
                }
            return false;
            }
        //---------------------------------------------
        void step_1(string_typeT& text)
            {
            size_t original_length = text.length();
            if (stem<string_typeT>::delete_if_is_in_r2(text,/*imientos*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (original_length > text.length() )
                    {
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*amientos*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (original_length > text.length() )
                    {
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::is_suffix_in_r2(text,/*uciones*/common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                text.erase(text.length()-6);
                stem<string_typeT>::update_r_sections(text);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*amiento*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                if (original_length > text.length() )
                    {
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*imiento*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                if (original_length > text.length() )
                    {
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*aciones*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (original_length > text.length() )
                    {
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ic*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C);
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::is_suffix_in_r2(text,/*logías*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                text.erase(text.length()-3);
                stem<string_typeT>::update_r_sections(text);
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_r2(text,/*encias*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                text.erase(text.length()-2);
                text[text.length()-2] = common_lang_constants::LOWER_T;
                text[text.length()-1] = common_lang_constants::LOWER_E;
                stem<string_typeT>::update_r_sections(text);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*idades*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (original_length > text.length() )
                    {
                    if (stem<string_typeT>::delete_if_is_in_r2(text,/*abil*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) ||
                        stem<string_typeT>::delete_if_is_in_r2(text,/*ic*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C) ||
                        stem<string_typeT>::delete_if_is_in_r2(text,/*iv*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V) )
                        {
                        return;
                        }
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*amente*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (original_length > text.length() )
                    {
                    if (stem<string_typeT>::delete_if_is_in_r2(text,/*iv*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V) )
                        {
                        stem<string_typeT>::delete_if_is_in_r2(text,/*at*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T);
                        return;
                        }
                    else
                        {
                        if (stem<string_typeT>::delete_if_is_in_r2(text,/*os*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) ||
                            stem<string_typeT>::delete_if_is_in_r2(text,/*ic*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C) ||
                            stem<string_typeT>::delete_if_is_in_r2(text,/*ad*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) )
                            {
                            return;
                            }
                        }
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*adores*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) ||
                     stem<string_typeT>::delete_if_is_in_r2(text,/*adoras*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) ||
                     stem<string_typeT>::delete_if_is_in_r2(text,/*ancias*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S))
                {
                if (original_length > text.length() )
                    {
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ic*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C);
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*adora*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) ||
                     stem<string_typeT>::delete_if_is_in_r2(text,/*ación*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O_ACUTE, common_lang_constants::UPPER_O_ACUTE, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) ||
                     stem<string_typeT>::delete_if_is_in_r2(text,/*antes*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) ||
                     stem<string_typeT>::delete_if_is_in_r2(text,/*ancia*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                if (original_length > text.length() )
                    {
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ic*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C);
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ibles*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (original_length > text.length() )
                    {
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*istas*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (original_length > text.length() )
                    {
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ables*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (original_length > text.length() )
                    {
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ismos*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (original_length > text.length() )
                    {
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*anzas*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (original_length > text.length() )
                    {
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::is_suffix_in_r2(text,/*logía*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                text.erase(text.length()-2);
                stem<string_typeT>::update_r_sections(text);
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_r2(text,/*ución*/common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O_ACUTE, common_lang_constants::UPPER_O_ACUTE, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) )
                {
                text.erase(text.length()-4);
                stem<string_typeT>::update_r_sections(text);
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_r2(text,/*encia*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                text.erase(text.length()-1);
                text[text.length()-2] = common_lang_constants::LOWER_T;
                text[text.length()-1] = common_lang_constants::LOWER_E;
                stem<string_typeT>::update_r_sections(text);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*mente*/common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (original_length > text.length() )
                    {
                    if (stem<string_typeT>::delete_if_is_in_r2(text,/*ante*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ||
                        stem<string_typeT>::delete_if_is_in_r2(text,/*able*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ||
                        stem<string_typeT>::delete_if_is_in_r2(text,/*ible*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                        {
                        return;
                        }
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*anza*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                if (original_length > text.length() )
                    {
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*icos*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (original_length > text.length() )
                    {
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*icas*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (original_length > text.length() )
                    {
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ismo*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                if (original_length > text.length() )
                    {
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*able*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (original_length > text.length() )
                    {
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ible*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (original_length > text.length() )
                    {
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ista*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                if (original_length > text.length() )
                    {
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*osos*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (original_length > text.length() )
                    {
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*osas*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (original_length > text.length() )
                    {
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ivas*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (original_length > text.length() )
                    {
                    stem<string_typeT>::delete_if_is_in_r2(text,/*at*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T);
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ivos*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (original_length > text.length() )
                    {
                    stem<string_typeT>::delete_if_is_in_r2(text,/*at*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T);
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ador*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ante*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (original_length > text.length() )
                    {
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ic*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C);
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*idad*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) )
                {
                if (original_length > text.length() )
                    {
                    if (stem<string_typeT>::delete_if_is_in_r2(text,/*abil*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) ||
                        stem<string_typeT>::delete_if_is_in_r2(text,/*ic*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C) ||
                        stem<string_typeT>::delete_if_is_in_r2(text,/*iv*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V) )
                        {
                        return;
                        }
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ico*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                if (original_length > text.length() )
                    {
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ica*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                if (original_length > text.length() )
                    {
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*oso*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                if (original_length > text.length() )
                    {
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*osa*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                if (original_length > text.length() )
                    {
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*iva*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                if (original_length > text.length() )
                    {
                    stem<string_typeT>::delete_if_is_in_r2(text,/*at*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T);
                    return;
                    }
                step_2a(text);
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ivo*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                if (original_length > text.length() )
                    {
                    stem<string_typeT>::delete_if_is_in_r2(text,/*at*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T);
                    return;
                    }
                step_2a(text);
                }    
            //this should only be called from here if nothing was removed in step 1
            if (text.length() == original_length)
                {
                step_2a(text);
                }
            }
        //---------------------------------------------
        void step_2a(string_typeT& text)
            {
            size_t original_length = text.length();
            if (stem<string_typeT>::is_suffix_in_rv(text,/*yeron*/common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N))
                {
                if (is_either<wchar_t>(text[text.length()-6], common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) )
                    {
                    text.erase(text.length()-5);
                    stem<string_typeT>::update_r_sections(text);
                    return;
                    }
                step_2b(text);
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*yendo*/common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O))
                {
                if (text.length() >= 6 &&
                    is_either<wchar_t>(text[text.length()-6], common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) )
                    {
                    text.erase(text.length()-5);
                    stem<string_typeT>::update_r_sections(text);
                    return;
                    }
                step_2b(text);
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*yamos*/common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S))
                {
                if (text.length() >= 6 &&
                    is_either<wchar_t>(text[text.length()-6], common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) )
                    {
                    text.erase(text.length()-5);
                    stem<string_typeT>::update_r_sections(text);
                    return;
                    }
                step_2b(text);
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*yais*/common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S))
                {
                if (text.length() >= 5 &&
                    is_either<wchar_t>(text[text.length()-5], common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) )
                    {
                    text.erase(text.length()-4);
                    stem<string_typeT>::update_r_sections(text);
                    return;
                    }
                step_2b(text);
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*yan*/common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N))
                {
                if (text.length() >= 4 &&
                    is_either<wchar_t>(text[text.length()-4], common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) )
                    {
                    text.erase(text.length()-3);
                    stem<string_typeT>::update_r_sections(text);
                    return;
                    }
                step_2b(text);
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*yen*/common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N))
                {
                if (text.length() >= 4 &&
                    is_either<wchar_t>(text[text.length()-4], common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) )
                    {
                    text.erase(text.length()-3);
                    stem<string_typeT>::update_r_sections(text);
                    return;
                    }
                step_2b(text);
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*yas*/common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S))
                {
                if (text.length() >= 4 &&
                    is_either<wchar_t>(text[text.length()-4], common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) )
                    {
                    text.erase(text.length()-3);
                    stem<string_typeT>::update_r_sections(text);
                    return;
                    }
                step_2b(text);
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*yes*/common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S))
                {
                if (text.length() >= 4 &&
                    is_either<wchar_t>(text[text.length()-4], common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) )
                    {
                    text.erase(text.length()-3);
                    stem<string_typeT>::update_r_sections(text);
                    return;
                    }
                step_2b(text);
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*ya*/common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A))
                {
                if (text.length() >= 3 &&
                    is_either<wchar_t>(text[text.length()-3], common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) )
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    return;
                    }
                step_2b(text);
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*ye*/common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E))
                {
                if (text.length() >= 3 &&
                    is_either<wchar_t>(text[text.length()-3], common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) )
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    return;
                    }
                step_2b(text);
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*yo*/common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O))
                {
                if (text.length() >= 3 &&
                    is_either<wchar_t>(text[text.length()-3], common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) )
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    return;
                    }
                step_2b(text);
                }
            else if (stem<string_typeT>::is_suffix_in_rv(text,/*yó*/common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y, common_lang_constants::LOWER_O_ACUTE, common_lang_constants::UPPER_O_ACUTE))
                {
                if (text.length() >= 3 &&
                    is_either<wchar_t>(text[text.length()-3], common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) )
                    {
                    text.erase(text.length()-2);
                    stem<string_typeT>::update_r_sections(text);
                    return;
                    }
                step_2b(text);
                }
            //only called if 2a fails to remove a suffix
            if (text.length() == original_length)
                {
                step_2b(text);
                }
            }
        //---------------------------------------------
        void step_2b(string_typeT& text) 
            {
            if (stem<string_typeT>::delete_if_is_in_rv(text,/*aríamos*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eríamos*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iríamos*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iéramos*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E_ACUTE, common_lang_constants::UPPER_E_ACUTE, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iésemos*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E_ACUTE, common_lang_constants::UPPER_E_ACUTE, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*aríais*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*aremos*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eríais*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eremos*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iríais*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iremos*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ierais*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ieseis*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*asteis*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*isteis*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ábamos*/common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*áramos*/common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ásemos*/common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*arían*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*arías*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*aréis*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E_ACUTE, common_lang_constants::UPPER_E_ACUTE, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erían*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erías*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eréis*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E_ACUTE, common_lang_constants::UPPER_E_ACUTE, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*irían*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*irías*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iréis*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E_ACUTE, common_lang_constants::UPPER_E_ACUTE, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ieran*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iesen*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ieron*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iendo*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ieras*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ieses*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*abais*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*arais*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*aseis*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*íamos*/common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*emos*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                if (stem<string_typeT>::is_suffix(text,/*gu*/common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) )
                    {
                    text.erase(text.length()-1);
                    stem<string_typeT>::update_r_sections(text);
                    }
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*arán*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*arás*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*aría*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erán*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erás*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ería*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*irán*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*irás*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iría*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iera*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iese*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*aste*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iste*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*aban*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*aran*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*asen*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*aron*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ando*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*abas*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*adas*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*idas*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*aras*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ases*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*íais*/common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ados*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*idos*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*amos*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*imos*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ará*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*aré*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E_ACUTE, common_lang_constants::UPPER_E_ACUTE, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erá*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eré*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E_ACUTE, common_lang_constants::UPPER_E_ACUTE, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*irá*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iré*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E_ACUTE, common_lang_constants::UPPER_E_ACUTE, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*aba*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ada*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ida*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ara*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ase*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ían*/common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ado*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ido*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ías*/common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*áis*/common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*éis*/common_lang_constants::LOWER_E_ACUTE, common_lang_constants::UPPER_E_ACUTE, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                if (stem<string_typeT>::is_suffix(text,/*gu*/common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) )
                    {
                    text.erase(text.length()-1);
                    stem<string_typeT>::update_r_sections(text);
                    }
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ía*/common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ad*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ed*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*id*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*an*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ió*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O_ACUTE, common_lang_constants::UPPER_O_ACUTE, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ar*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*er*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ir*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*as*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ís*/common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*en*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, false) )
                {
                if (stem<string_typeT>::is_suffix(text,/*gu*/common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) )
                    {
                    text.erase(text.length()-1);
                    stem<string_typeT>::update_r_sections(text);
                    }
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*es*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                if (stem<string_typeT>::is_suffix(text,/*gu*/common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) )
                    {
                    text.erase(text.length()-1);
                    stem<string_typeT>::update_r_sections(text);
                    }
                return;
                }
            }
         //---------------------------------------------
        void step_3(string_typeT& text) 
            {
            if (stem<string_typeT>::delete_if_is_in_rv(text,/*os*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*a*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*o*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*á*/common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*í*/common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ó*/common_lang_constants::LOWER_O_ACUTE, common_lang_constants::UPPER_O_ACUTE) )
                {
                return;
                }

            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*é*/common_lang_constants::LOWER_E_ACUTE, common_lang_constants::UPPER_E_ACUTE) ||
                    stem<string_typeT>::delete_if_is_in_rv(text,/*e*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (stem<string_typeT>::is_suffix_in_rv(text,/*u*/common_lang_constants::LOWER_U, common_lang_constants::UPPER_U))
                    {
                    if (text.length() >= 2 &&
                        is_either<wchar_t>(text[text.length()-2], common_lang_constants::LOWER_G, common_lang_constants::UPPER_G) )
                        {
                        text.erase(text.length()-1);
                        }
                    }
                return;
                }
            }
        };
    }

/** @}*/

#endif //__SPANISH_STEM_H__
