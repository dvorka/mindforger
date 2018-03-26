/**@addtogroup Stemming
@brief Library for stemming words down to their root words.
@date 2003-2015
@copyright Oleander Software, Ltd.
@author Oleander Software, Ltd.
@details This program is free software; you can redistribute it and/or modify
it under the terms of the BSD License.
* @{*/

#ifndef __ITALIAN_STEM_H__
#define __ITALIAN_STEM_H__

#include "stemming.h"

namespace stemming
    {
    /**
    @brief Italian stemmer.
    @date 2004
    @par Algorithm:

    Italian can include the following accented forms:
        - á é í ó ú à è ì ò ù

    First, replace all acute accents by grave accents.
    And, as in French, put u after q, and u, i between vowels into upper case. The vowels are then:
        - a e i o u à è ì ò ù

    R2 and RV have the same definition as in the Spanish stemmer.

    @par Algorithm:

    <b>Step 0:</b>

    Search for the longest among the following suffixes
        - ci gli la le li lo mi ne si ti vi sene gliela gliele glieli glielo gliene mela
        mele meli melo mene tela tele teli telo tene cela cele celi celo cene vela vele veli velo vene 

    following one of:
        - ando endo
        - ar er ir 

    in RV. In case of (a) the suffix is deleted, in case (b) it is replace by e
    (guardandogli -> guardando, accomodarci -> accomodare).

    <b>Step 1:</b>

    Search for the longest among the following suffixes, and perform the action indicated.
        - anza   anze   ico   ici   ica   ice   iche   ichi   ismo   ismi   abile   abili   ibile
         ibili   ista   iste   isti   istà   istè   istì   oso   osi   osa   ose   mente   atrice
         atrici  ante   anti
            - Delete if in R2.
        - azione   azioni   atore   atori delete if in R2
            - If preceded by ic, delete if in R2.
        - logia   logie
            - Replace with log if in R2.
        - uzione   uzioni   usione   usioni
            - Replace with u if in R2.
        - enza   enze
            - Replace with ente if in R2.
        - amento   amenti   imento   imenti
            - Delete if in RV.
        - amente
            - Delete if in R1.
            - If preceded by iv, delete if in R2 (and if further preceded by at, delete if in R2), otherwise,
            - If preceded by os, ic or abil, delete if in R2.
        - ità
            - Delete if in R2.
            - If preceded by abil, ic or iv, delete if in R2.
        - ivo   ivi   iva   ive
            - Delete if in R2.
            - If preceded by at, delete if in R2 (and if further preceded by ic, delete if in R2).

    Do step 2 if no ending was removed by step 1.

    <b>Step 2:</b>
    Search for the longest among the following suffixes in RV, and if found, delete.
        - ammo ando ano are arono asse assero assi assimo ata ate ati ato
        ava avamo avano avate avi avo emmo enda ende endi endo erà erai
        eranno ere erebbe erebbero erei eremmo eremo ereste eresti erete
        erò erono essero ete eva evamo evano evate evi evo Yamo iamo immo 
        irà irai iranno ire irebbe irebbero irei iremmo iremo ireste iresti
        irete irò irono isca iscano isce isci isco iscono issero ita ite iti
        ito iva ivamo ivano ivate ivi ivo ono uta ute uti uto ar ir

    Always do steps 3a and 3b.

    <b>Step 3(a):</b>

    Delete a final a, e, i, o, à, è, ì, or ò if it is in RV, and a preceding 'i'
    if it is in RV (crocchi -> crocch, crocchio -> crocch).

    <b>Step 3(b):</b>

    Replace final ch (or gh) with c (or g) if in RV (crocch -> crocc).
    */
    //------------------------------------------------------
    template <typename string_typeT = std::wstring>
    class italian_stem : public stem<string_typeT>
        {
    public:
        //---------------------------------------------
        ///@param[in,out] text string to stem
        void operator()(string_typeT& text)
            {
            if (text.length() < 3)
                {
                stem<string_typeT>::italian_acutes_to_graves(text);
                return;
                }

            //reset internal data
            stem<string_typeT>::reset_r_values();

            stem<string_typeT>::trim_western_punctuation(text);
            stem<string_typeT>::italian_acutes_to_graves(text);
            stem<string_typeT>::hash_italian_ui(text, ITALIAN_VOWELS);

            stem<string_typeT>::find_r1(text, ITALIAN_VOWELS);
            stem<string_typeT>::find_r2(text, ITALIAN_VOWELS);
            stem<string_typeT>::find_spanish_rv(text, ITALIAN_VOWELS);

            //step 0:
            step_0(text);
            //step 1:
            size_t text_length = text.length();
            step_1(text);

            //step 2 is called only if step 1 did not remove a suffix
            if (text_length == text.length() )
                {
                step_2(text);
                }

            //step 3:
            step_3a(text);
            step_3b(text);

            stem<string_typeT>::unhash_italian_ui(text);
            }
    private:
        //---------------------------------------------
        void step_0(string_typeT& text)
            {
            if (stem<string_typeT>::is_suffix(text,/*gliela*/common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                if (step_0a(text, 6) || step_0b(text, 6) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*gliele*/common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (step_0a(text, 6) || step_0b(text, 6) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*glieli*/common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                if (step_0a(text, 6) || step_0b(text, 6) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*glielo*/common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                if (step_0a(text, 6) || step_0b(text, 6) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*gliene*/common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (step_0a(text, 6) || step_0b(text, 6) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*sene*/common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (step_0a(text, 4) || step_0b(text, 4) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*mela*/common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                if (step_0a(text, 4) || step_0b(text, 4) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*mele*/common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (step_0a(text, 4) || step_0b(text, 4) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*meli*/common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                if (step_0a(text, 4) || step_0b(text, 4) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*melo*/common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                if (step_0a(text, 4) || step_0b(text, 4) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*mene*/common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (step_0a(text, 4) || step_0b(text, 4) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*tela*/common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                if (step_0a(text, 4) || step_0b(text, 4) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*tele*/common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (step_0a(text, 4) || step_0b(text, 4) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*teli*/common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                if (step_0a(text, 4) || step_0b(text, 4) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*telo*/common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                if (step_0a(text, 4) || step_0b(text, 4) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*tene*/common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (step_0a(text, 4) || step_0b(text, 4) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*cela*/common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                if (step_0a(text, 4) || step_0b(text, 4) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*cela*/common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                if (step_0a(text, 4) || step_0b(text, 4) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*celi*/common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                if (step_0a(text, 4) || step_0b(text, 4) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*celo*/common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                if (step_0a(text, 4) || step_0b(text, 4) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*cene*/common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (step_0a(text, 4) || step_0b(text, 4) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*vela*/common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                if (step_0a(text, 4) || step_0b(text, 4) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*vele*/common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (step_0a(text, 4) || step_0b(text, 4) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*veli*/common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                if (step_0a(text, 4) || step_0b(text, 4) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*velo*/common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                if (step_0a(text, 4) || step_0b(text, 4) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*vene*/common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (step_0a(text, 4) || step_0b(text, 4) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*gli*/common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                if (step_0a(text, 3) || step_0b(text, 3) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*ci*/common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                if (step_0a(text, 2) || step_0b(text, 2) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*la*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                if (step_0a(text, 2) || step_0b(text, 2) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*le*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (step_0a(text, 2) || step_0b(text, 2) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*li*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                if (step_0a(text, 2) || step_0b(text, 2) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*lo*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                if (step_0a(text, 2) || step_0b(text, 2) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*mi*/common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                if (step_0a(text, 2) || step_0b(text, 2) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*ne*/common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (step_0a(text, 2) || step_0b(text, 2) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*si*/common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                if (step_0a(text, 2) || step_0b(text, 2) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*ti*/common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                if (step_0a(text, 2) || step_0b(text, 2) ) { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::is_suffix(text,/*vi*/common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                if (step_0a(text, 2) || step_0b(text, 2) ) { /*NOOP*/ }
                return;
                }
            }
        //---------------------------------------------
        bool step_0a(string_typeT& text, size_t suffix_length)
            {
            if (text.length() >= (suffix_length+4) &&
                stem<string_typeT>::get_rv() <= text.length()-(4+suffix_length) &&
                (/*ando*/(is_either<wchar_t>(text[text.length()-(4+suffix_length)], common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) &&
                        is_either<wchar_t>(text[text.length()-(3+suffix_length)], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                        is_either<wchar_t>(text[text.length()-(2+suffix_length)], common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) &&
                        is_either<wchar_t>(text[text.length()-(1+suffix_length)], common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) ) ||
                /*endo*/(is_either<wchar_t>(text[text.length()-(4+suffix_length)], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) &&
                        is_either<wchar_t>(text[text.length()-(3+suffix_length)], common_lang_constants::LOWER_N, common_lang_constants::UPPER_N) &&
                        is_either<wchar_t>(text[text.length()-(2+suffix_length)], common_lang_constants::LOWER_D, common_lang_constants::UPPER_D) &&
                        is_either<wchar_t>(text[text.length()-(1+suffix_length)], common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) ) ) )
                {
                text.erase(text.length()-suffix_length);
                stem<string_typeT>::update_r_sections(text);
                return true;
                }
            return false;
            }
        //---------------------------------------------
        bool step_0b(string_typeT& text, size_t suffix_length)
            {
            if ((text.length() >= suffix_length+2) &&
                stem<string_typeT>::get_rv() <= (text.length()-(2+suffix_length)) &&
                (
                /*ar*/(is_either<wchar_t>(text[text.length()-(2+suffix_length)], common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) && is_either<wchar_t>(text[text.length()-1-suffix_length], common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) ) ||
                /*er*/(is_either<wchar_t>(text[text.length()-(2+suffix_length)], common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) && is_either<wchar_t>(text[text.length()-1-suffix_length], common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) ) ||
                /*or*/(is_either<wchar_t>(text[text.length()-(2+suffix_length)], common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) && is_either<wchar_t>(text[text.length()-1-suffix_length], common_lang_constants::LOWER_R, common_lang_constants::UPPER_R) )
                ) )
                {
                text.replace(text.end()-suffix_length, text.end(), L"e");
                stem<string_typeT>::update_r_sections(text);
                return true;
                }
            return false;
            }
        //---------------------------------------------
        void step_1(string_typeT& text)
            {
            if (stem<string_typeT>::delete_if_is_in_rv(text,/*amento*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) ||
                stem<string_typeT>::delete_if_is_in_rv(text,/*amenti*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ||
                stem<string_typeT>::delete_if_is_in_rv(text,/*imento*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) ||
                stem<string_typeT>::delete_if_is_in_rv(text,/*imenti*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*azione*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                stem<string_typeT>::delete_if_is_in_r2(text,/*ic*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*azioni*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                stem<string_typeT>::delete_if_is_in_r2(text,/*ic*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C);
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_r2(text,/*uzione*/common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ||
                stem<string_typeT>::is_suffix_in_r2(text,/*uzioni*/common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ||
                stem<string_typeT>::is_suffix_in_r2(text,/*usione*/common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ||
                stem<string_typeT>::is_suffix_in_r2(text,/*usioni*/common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                text.erase(text.length()-5);
                stem<string_typeT>::update_r_sections(text);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r1(text,/*amente*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (stem<string_typeT>::delete_if_is_in_r2(text,/*iv*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V) )
                    {
                    stem<string_typeT>::delete_if_is_in_r2(text,/*at*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T);
                    }
                else if (stem<string_typeT>::delete_if_is_in_r2(text,/*abil*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) ||
                        stem<string_typeT>::delete_if_is_in_r2(text,/*ic*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C) ||
                        stem<string_typeT>::delete_if_is_in_r2(text,/*os*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S) )
                    { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*atrice*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*atrici*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*abile*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*abili*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ibile*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ibili*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*mente*/common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*atore*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                stem<string_typeT>::delete_if_is_in_r2(text,/*ic*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*atori*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                stem<string_typeT>::delete_if_is_in_r2(text,/*ic*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C);
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_r2(text,/*logia*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) ||
                stem<string_typeT>::is_suffix_in_r2(text,/*logie*/common_lang_constants::LOWER_L, common_lang_constants::UPPER_L, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                text.erase(text.length()-2);
                stem<string_typeT>::update_r_sections(text);
                return;
                }
            else if (stem<string_typeT>::is_suffix_in_r2(text,/*enza*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) ||
                stem<string_typeT>::is_suffix_in_r2(text,/*enze*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                text.replace(text.end()-2, text.end(), L"te");
                stem<string_typeT>::update_r_sections(text);
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ante*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*anti*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*anza*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*anze*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_Z, common_lang_constants::UPPER_Z, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*iche*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ichi*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_H, common_lang_constants::UPPER_H, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ismo*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ismi*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ista*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*iste*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*isti*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*istà*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A_GRAVE, common_lang_constants::UPPER_A_GRAVE) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*istè*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E_GRAVE, common_lang_constants::UPPER_E_GRAVE) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*istì*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I_GRAVE, common_lang_constants::UPPER_I_GRAVE) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ico*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ici*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ica*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ice*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*oso*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*osi*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*osa*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ose*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ità*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A_GRAVE, common_lang_constants::UPPER_A_GRAVE) )
                {
                if (stem<string_typeT>::delete_if_is_in_r2(text,/*abil*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_L, common_lang_constants::UPPER_L) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ic*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C) ||
                    stem<string_typeT>::delete_if_is_in_r2(text,/*iv*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V) )
                    { /*NOOP*/ }
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_r2(text,/*ivo*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O) ||
                stem<string_typeT>::delete_if_is_in_r2(text,/*ivi*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) ||
                stem<string_typeT>::delete_if_is_in_r2(text,/*iva*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A) ||
                stem<string_typeT>::delete_if_is_in_r2(text,/*ive*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E) )
                {
                if (stem<string_typeT>::delete_if_is_in_r2(text,/*at*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T) )
                    {
                    stem<string_typeT>::delete_if_is_in_r2(text,/*ic*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C);
                    }
                return;
                }
            }
        //---------------------------------------------
        void step_2(string_typeT& text)
            {
            if (stem<string_typeT>::delete_if_is_in_rv(text,/*erebbero*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*irebbero*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*assero*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*assimo*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eranno*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erebbe*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eremmo*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ereste*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eresti*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*essero*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iranno*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*irebbe*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_B, common_lang_constants::UPPER_B, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iremmo*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ireste*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iresti*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iscano*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iscono*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*issero*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*arono*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*avamo*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*avano*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*avate*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eremo*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erete*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erono*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*evamo*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*evano*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*evate*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iremo*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*irete*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*irono*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ivamo*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ivano*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ivate*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ammo*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ando*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text, /*asse*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*assi*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*emmo*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*enda*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ende*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*endi*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*endo*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_D, common_lang_constants::UPPER_D, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erai*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erei*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*yamo*/common_lang_constants::LOWER_Y, common_lang_constants::UPPER_Y, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iamo*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*immo*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_M, common_lang_constants::UPPER_M, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*irai*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*irei*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*isca*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*isce*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*isci*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*isco*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_S, common_lang_constants::UPPER_S, common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ano*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*are*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ata*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ate*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ati*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ato*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ava*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*avi*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*avo*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erà*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A_GRAVE, common_lang_constants::UPPER_A_GRAVE, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ere*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*erò*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_O_GRAVE, common_lang_constants::UPPER_O_GRAVE, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ete*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*eva*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*evi*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*evo*/common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*irà*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_A_GRAVE, common_lang_constants::UPPER_A_GRAVE, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ire*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*irò*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, common_lang_constants::LOWER_O_GRAVE, common_lang_constants::UPPER_O_GRAVE, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ita*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ite*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iti*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ito*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*iva*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ivi*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ivo*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_V, common_lang_constants::UPPER_V, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ono*/common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, common_lang_constants::LOWER_N, common_lang_constants::UPPER_N, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*uta*/common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ute*/common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_E, common_lang_constants::UPPER_E, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*uti*/common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*uto*/common_lang_constants::LOWER_U, common_lang_constants::UPPER_U, common_lang_constants::LOWER_T, common_lang_constants::UPPER_T, common_lang_constants::LOWER_O, common_lang_constants::UPPER_O, false) )
                {
                return;
                }
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ar*/common_lang_constants::LOWER_A, common_lang_constants::UPPER_A, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, false) )
                {
                return;
                }
            ///'ir' not in original specification, but used in general implementation
            else if (stem<string_typeT>::delete_if_is_in_rv(text,/*ir*/common_lang_constants::LOWER_I, common_lang_constants::UPPER_I, common_lang_constants::LOWER_R, common_lang_constants::UPPER_R, false) )
                {
                return;
                }
            /**deletion or 'er' from rv is considered problematic,
            but part of the standard*/
            }
        //---------------------------------------------
        void step_3a(string_typeT& text)
            {
            if (text.length() >= 1 &&
                stem<string_typeT>::get_rv() <= text.length()-1 &&
                string_util::is_one_of(text[text.length()-1], ITALIAN_VOWELS_SIMPLE) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                if (stem<string_typeT>::get_rv() <= text.length()-1 &&                    
                    is_either<wchar_t>(text[text.length()-1], common_lang_constants::LOWER_I, common_lang_constants::UPPER_I) )
                    {
                    text.erase(text.length()-1);
                    stem<string_typeT>::update_r_sections(text);
                    }
                }
            }
        //---------------------------------------------
        void step_3b(string_typeT& text)
            {
            if (stem<string_typeT>::is_suffix_in_rv(text,/*ch*/common_lang_constants::LOWER_C, common_lang_constants::UPPER_C, common_lang_constants::LOWER_H, common_lang_constants::UPPER_H) ||
                stem<string_typeT>::is_suffix_in_rv(text,/*gh*/common_lang_constants::LOWER_G, common_lang_constants::UPPER_G, common_lang_constants::LOWER_H, common_lang_constants::UPPER_H) )
                {
                text.erase(text.length()-1);
                stem<string_typeT>::update_r_sections(text);
                }
            }
        };
    }

/** @}*/

#endif //__ITALIAN_STEM_H__
