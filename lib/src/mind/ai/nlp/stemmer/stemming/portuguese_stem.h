/**@addtogroup Stemming
@brief Library for stemming words down to their root words.
@date 2003-2015
@copyright Oleander Software, Ltd.
@author Oleander Software, Ltd.
@details This program is free software; you can redistribute it and/or modify
it under the terms of the BSD License.
* @{*/

#ifndef __PORTUGUESE_STEM_H__
#define __PORTUGUESE_STEM_H__

#include "stemming.h"

namespace stemming
    {
    /**
    @brief Portuguese stemmer.
    @date 2004
    @par Algorithm:

    Letters in Portuguese include the following accented forms,
        - á é í ó ú â ê ô ç ã õ ü ñ

    The following letters are vowels:
        - a e i o u á é í ó ú â ê ô

    And the two nasalised vowel forms
     - ã õ

    should be treated as a vowel followed by a consonant. 

    ã and õ are therefore replaced by a~ and o~ in the word, where ~ is a separate character to be treated as a consonant.
    And then R2 and RV have the same definition as in the Spanish stemmer.

    @par Algorithm:

    <b>Step 1:</b>

    Search for the longest among the following suffixes, and perform the action indicated.
        - eza   ezas   ico   ica   icos   icas   ismo   ismos   ável   ível   ista   istas
         oso   osa   osos   osas   amento   amentos   imento   imentos   adora   ador   aça~o
         adoras   adores   aço~es   ante   antes   ância
            - Delete if in R2.
        - logia logias
            - Replace with log if in R2.
        - uça~o uço~es
            - Replace with u if in R2
        - ência ências
            - Replace with ente if in R2.
        - amente
            - Delete if in R1.
            - If preceded by iv, delete if in R2 (and if further preceded by at, delete if in R2), otherwise,
            - If preceded by os, ic or ad, delete if in R2.
        - mente
            - Delete if in R2.
            - If preceded by ante, avel or ível, delete if in R2.
        - idade idades
            - Delete if in R2.
            - If preceded by abil, ic or iv, delete if in R2.
        - iva ivo ivas ivos
            - Delete if in R2.
            - If preceded by at, delete if in R2.
        - ira iras
            - Replace with ir if in RV and preceded by e.

    Do step 2 if no ending was removed by step 1.

    <b>Step 2:</b>

    Search for the longest among the following suffixes in RV, and if found, delete. 

    - ada ida ia aria eria iria ará ara erá era irá ava asse esse isse aste este iste
    ei arei erei irei am iam ariam eriam iriam aram eram iram avam em arem erem irem
    assem essem issem ado ido ando endo indo ara~o era~o ira~o ar er ir as adas idas
    ias arias erias irias arás aras erás eras irás avas es ardes erdes irdes ares eres
    ires asses esses isses astes estes istes is ais eis íeis aríeis eríeis iríeis áreis
    areis éreis ereis íreis ireis ásseis ésseis ísseis áveis ados idos ámos amos íamos
    aríamos eríamos iríamos áramos éramos íramos ávamos emos aremos eremos iremos ássemos
    êssemos íssemos imos armos ermos irmos eu iu ou ira iras

    If the last step to be obeyed - either step 1 or 2 - altered the word, do step 3.

    <b>Step 3:</b>

    Delete suffix 'i' if in RV and preceded by 'c'.
    Alternatively, if neither steps 1 nor 2 altered the word, do step 4.

    <b>Step 4:</b>
    
    If the word ends with one of the suffixes 
        - os a i o á í ó 

    in RV, delete it.

    Always do step 5.

    <b>Step 5:</b>
    
    If the word ends with one of
        - e é ê 

    in RV, delete it, and if preceded by gu (or ci) with the u (or i) in RV, delete the u (or i). 
    Or if the word ends ç remove the cedilla.
    */
    //------------------------------------------------------
    template <typename string_typeT = std::wstring>
    class portuguese_stem : public stem<string_typeT>
        {
    public:
        portuguese_stem() : m_step1_step2_altered(false), m_altered_suffix_index(0)
            {}
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
            m_altered_suffix_index = 0;
            m_step1_step2_altered = false;
            stem<string_typeT>::reset_r_values();

            string_util::replace_all<string_typeT>(text, string_typeT(1, common_lang_constants::LOWER_A_TILDE), L"a~");
            string_util::replace_all<string_typeT>(text, string_typeT(1, common_lang_constants::UPPER_A_TILDE), L"A~");
            string_util::replace_all<string_typeT>(text, string_typeT(1, common_lang_constants::LOWER_O_TILDE), L"o~");
            string_util::replace_all<string_typeT>(text, string_typeT(1, common_lang_constants::UPPER_O_TILDE), L"O~");

            stem<string_typeT>::find_r1(text, PORTUGUESE_VOWELS);
            stem<string_typeT>::find_r2(text, PORTUGUESE_VOWELS);
            stem<string_typeT>::find_spanish_rv(text, PORTUGUESE_VOWELS);

            step_1(text);
            //intermediate steps handled by step 1
            if (!m_step1_step2_altered)
                {
                step_4(text);
                }
            step_5(text);

            //Turn a~, o~ back into ã, õ
            string_util::replace_all<string_typeT>(text, L"a~", string_typeT(1, common_lang_constants::LOWER_A_TILDE));
            string_util::replace_all<string_typeT>(text, L"A~", string_typeT(1, common_lang_constants::UPPER_A_TILDE));
            string_util::replace_all<string_typeT>(text, L"o~", string_typeT(1, common_lang_constants::LOWER_O_TILDE));
            string_util::replace_all<string_typeT>(text, L"O~", string_typeT(1, common_lang_constants::UPPER_O_TILDE));
            }
    private:
        //---------------------------------------------
        void step_1(string_typeT& text)
            {
            size_t original_length = text.length();
            if (stem<string_typeT>::delete_if_is_in_r2(text,/*amentos*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*imentos*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::is_suffix_in_r2(text,/*uço~es*/common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_C_CEDILLA, common_lang_constants::UPPER_C_CEDILLA, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::TILDE, common_lang_constants::TILDE, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                text.erase(text.length()-5);
                m_altered_suffix_index = text.length()-1;
                stem<string_typeT>::update_r_sections(text);
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*amento*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*imento*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*adoras*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*adores*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*aço~es*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_C_CEDILLA, common_lang_constants::UPPER_C_CEDILLA, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::TILDE, common_lang_constants::TILDE, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::is_suffix_in_r2(text,/*ências*/common_lang_constants::LOWER_E_CIRCUMFLEX, common_lang_constants::UPPER_E_CIRCUMFLEX, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                text.replace(text.end()-6, text.end(), L"ente");
                m_altered_suffix_index = text.length()-4;
                stem<string_typeT>::update_r_sections(text);
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::is_suffix_in_r2(text,/*logias*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                text.erase(text.length()-3);
                m_altered_suffix_index = text.length()-3;
                stem<string_typeT>::update_r_sections(text);
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*amente*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (stem<string_typeT>::delete_if_is_in_r2(text,/*iv*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, false) )
                    {
                    stem<string_typeT>::delete_if_is_in_r2(text,/*at*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, false);
                    }
                else
                    {
                    if (stem<string_typeT>::delete_if_is_in_r2(text,/*os*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) ||
                        stem<string_typeT>::delete_if_is_in_r2(text,/*ic*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C) ||
                        stem<string_typeT>::delete_if_is_in_r2(text,/*ad*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) )
                        {
                        //NOOP (fall through to branching statement)
                        }
                    }
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*idades*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (stem<string_typeT>::delete_if_is_in_r2(text,/*abil*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ic*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*iv*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V) )
                    {
                    //NOOP (fall through to branching statement)
                    }
                //NOOP (fall through to branching statement)
                }    
            else if (stem<string_typeT>::is_suffix_in_r2(text,/*logia*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                text.erase(text.length()-2);
                m_altered_suffix_index = text.length()-3;
                stem<string_typeT>::update_r_sections(text);
                //NOOP (fall through to branching statement)
                }    
            else if (stem<string_typeT>::is_suffix_in_r2(text,/*uça~o*/common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_C_CEDILLA, common_lang_constants::UPPER_C_CEDILLA, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::TILDE, common_lang_constants::TILDE, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                text.erase(text.length()-4);
                m_altered_suffix_index = text.length()-1;
                stem<string_typeT>::update_r_sections(text);
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::is_suffix_in_r2(text,/*ência*/common_lang_constants::LOWER_E_CIRCUMFLEX, common_lang_constants::UPPER_E_CIRCUMFLEX, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                text.replace(text.end()-5, text.end(), L"ente");
                m_altered_suffix_index = text.length()-4;
                stem<string_typeT>::update_r_sections(text);
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*mente*/common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (stem<string_typeT>::delete_if_is_in_r2(text,/*ante*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*avel*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ível*/common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) )
                    {
                    //NOOP (fall through to branching statement)
                    }
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*idade*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (stem<string_typeT>::delete_if_is_in_r2(text,/*abil*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ic*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*iv*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V) )
                    {
                    //NOOP (fall through to branching statement)
                    }
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::is_suffix(text,/*eiras*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (stem<string_typeT>::get_rv() <= text.length()-4)
                    {
                    text.erase(text.length()-2);
                    m_altered_suffix_index = text.length()-3;
                    stem<string_typeT>::update_r_sections(text);
                    }
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*antes*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ância*/common_lang_constants::LOWER_A_CIRCUMFLEX, common_lang_constants::UPPER_A_CIRCUMFLEX, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ismos*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*istas*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*adora*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*aça~o*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_C_CEDILLA, common_lang_constants::UPPER_C_CEDILLA, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::TILDE, common_lang_constants::TILDE, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ante*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ezas*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*icos*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*icas*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ismo*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ável*/common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ível*/common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ista*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*osos*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*osas*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ador*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ivas*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (stem<string_typeT>::delete_if_is_in_r2(text,/*at*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                    {
                    //NOOP (fall through to branching statement)
                    }
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ivos*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                if (stem<string_typeT>::delete_if_is_in_r2(text,/*at*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                    {
                    //NOOP (fall through to branching statement)
                    }
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::is_suffix(text,/*eira*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                if (stem<string_typeT>::get_rv() <= text.length()-3)
                    {
                    text.erase(text.length()-1);
                    m_altered_suffix_index = text.length()-3;
                    stem<string_typeT>::update_r_sections(text);
                    }
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*iva*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                if (stem<string_typeT>::delete_if_is_in_r2(text,/*at*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                    {
                    //NOOP (fall through to branching statement)
                    }
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ivo*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                if (stem<string_typeT>::delete_if_is_in_r2(text,/*at*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                    {
                    //NOOP (fall through to branching statement)
                    }
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*eza*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ico*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ica*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*oso*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*osa*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                //NOOP (fall through to branching statement)
                }
            //branch to the next appropriate step
            if (original_length == text.length() )
                {
                step_2(text);
                }
            else
                {
                m_step1_step2_altered = true;
                step_3(text);
                }
             }
        //---------------------------------------------
        void step_2(string_typeT& text)
            {
            size_t original_length = text.length();

            if (stem<string_typeT>::delete_if_is_in_rv(text,/*aríamos*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eríamos*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iríamos*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ássemos*/common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*êssemos*/common_lang_constants::LOWER_E_CIRCUMFLEX, common_lang_constants::UPPER_E_CIRCUMFLEX, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*íssemos*/common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*aríeis*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eríeis*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iríeis*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ásseis*/common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ésseis*/common_lang_constants::LOWER_E_ACUTE, common_lang_constants::UPPER_E_ACUTE, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ísseis*/common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*áramos*/common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*éramos*/common_lang_constants::LOWER_E_ACUTE, common_lang_constants::UPPER_E_ACUTE, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*íramos*/common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ávamos*/common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*aremos*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eremos*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iremos*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S ,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ariam*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eriam*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iriam*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*assem*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*essem*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*issem*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ara~o*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::TILDE, common_lang_constants::TILDE, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*era~o*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::TILDE, common_lang_constants::TILDE, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ira~o*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::TILDE, common_lang_constants::TILDE, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*arias*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erias*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*irias*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ardes*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erdes*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*irdes*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*asses*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*esses*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*isses*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*astes*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*estes*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*istes*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*áreis*/common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*areis*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*éreis*/common_lang_constants::LOWER_E_ACUTE, common_lang_constants::UPPER_E_ACUTE, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ereis*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*íreis*/common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ireis*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*áveis*/common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*íamos*/common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*armos*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ermos*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*irmos*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S,false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*aria*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eria*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iria*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*asse*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*esse*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*isse*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*aste*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*este*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iste*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*arei*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erei*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*irei*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*aram*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eram*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iram*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*avam*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*arem*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erem*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*irem*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ando*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*endo*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*indo*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*adas*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*idas*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*arás*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*aras*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erás*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eras*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*irás*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*avas*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ares*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eres*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ires*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*íeis*/common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ados*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*idos*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ámos*/common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*amos*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*emos*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*imos*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iras*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ada*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ida*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ará*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ara*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erá*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*era*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*irá*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ava*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iam*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ado*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ido*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ias*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ais*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eis*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ira*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ia*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ei*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*am*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*em*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ar*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*er*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ir*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*as*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*es*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*is*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eu*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iu*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, false) )
                {
                //NOOP (fall through to branching statement)
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ou*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, false) )
                {
                //NOOP (fall through to branching statement)
                }

            if (original_length != text.length() )
                {
                m_step1_step2_altered = true;
                step_3(text);
                }
            }
        //---------------------------------------------
        void step_3(string_typeT& text) 
            {
            if (stem<string_typeT>::is_suffix(text,/*ci*/common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) &&
                stem<string_typeT>::get_rv() <= text.length()-1)
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                }
            }
        //---------------------------------------------
        void step_4(string_typeT& text) 
            {
            if (stem<string_typeT>::delete_if_is_in_rv(text,/*os*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_A_ACUTE, common_lang_constants::UPPER_A_ACUTE) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_I_ACUTE, common_lang_constants::UPPER_I_ACUTE) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_O_ACUTE, common_lang_constants::UPPER_O_ACUTE) )
                {
                return;
                }
            }
        //---------------------------------------------
        void step_5(string_typeT& text) 
            {
            if (stem<string_typeT>::delete_if_is_in_rv(text, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                if (text.length() >= 1 &&
                    stem<string_typeT>::get_rv() <= text.length()-1 &&
                    (stem<string_typeT>::is_suffix(text,/*gu*/common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) ||
                    stem<string_typeT>::is_suffix(text,/*ci*/common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I)) )
                    {
                    text.erase(text.length()-1);
                    stem<string_typeT>::update_r_sections(text);
                    }
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*é*/common_lang_constants::LOWER_E_ACUTE, common_lang_constants::UPPER_E_ACUTE, false) )
                {
                if (text.length() >= 1 &&
                    stem<string_typeT>::get_rv() <= text.length()-1 &&
                    (stem<string_typeT>::is_suffix(text,/*gu*/common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) ||
                    stem<string_typeT>::is_suffix(text,/*ci*/common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I)) )
                    {
                    text.erase(text.length()-1);
                    stem<string_typeT>::update_r_sections(text);
                    }
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ê*/common_lang_constants::LOWER_E_CIRCUMFLEX, common_lang_constants::UPPER_E_CIRCUMFLEX, false) )
                {
                if (text.length() >= 1 &&
                    stem<string_typeT>::get_rv() <= text.length()-1 &&
                    (stem<string_typeT>::is_suffix(text,/*gu*/common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_U, common_lang_constants::UPPER_U) ||
                    stem<string_typeT>::is_suffix(text,/*ci*/common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I)) )
                    {
                    text.erase(text.length()-1);
                    stem<string_typeT>::update_r_sections(text);
                    }
                return;
                }
            else if (text.length() >= 1 &&
                text[text.length()-1] == common_lang_constants::UPPER_C_CEDILLA)
                {
                text[text.length()-1] = common_lang_constants::UPPER_C;
                }
            else if (text.length() >= 1 &&
                text[text.length()-1] == common_lang_constants::LOWER_C_CEDILLA)
                {
                text[text.length()-1] = common_lang_constants::LOWER_C;
                }
            }
        //internal data specific to Portuguese stemmer
        bool m_step1_step2_altered;
        size_t m_altered_suffix_index;
        };
    }

/** @}*/

#endif //__PORTUGUESE_STEM_H__
