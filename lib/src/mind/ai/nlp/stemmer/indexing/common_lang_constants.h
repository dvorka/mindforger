/**@addtogroup Indexing
@brief Library for stemming words down to their root words.
@date 2003-2016
@copyright Oleander Software, Ltd.
@author Oleander Software, Ltd.
@details This program is free software; you can redistribute it and/or modify
it under the terms of the BSD License.
* @{*/

#ifndef __COMMON_LANG_CONSTANTS_H__
#define __COMMON_LANG_CONSTANTS_H__

#include <cwctype>

namespace common_lang_constants
    {
    const wchar_t TAB = 0x09;
    const wchar_t SPACE = 0x20;
    const wchar_t COMMA = 0x2C;
    const wchar_t LESS_THAN = 60;
    const wchar_t GREATER_THAN = 62;
    const wchar_t POUND = 35;
    const wchar_t AMPERSAND = 0x26;
    const wchar_t SEMICOLON = 59;
    const wchar_t APOSTROPHE = 0x27;
    const wchar_t DOUBLE_QUOTE = 0x22;
    const wchar_t QUESTION_MARK = 0x3F;
    const wchar_t QUESTION_MARK_FULL_WIDTH = 0xFF1F;
    const wchar_t PERIOD = 0x2E;
    const wchar_t PERIOD_FULL_WIDTH = 0xFF0E;
    const wchar_t PERIOD_HALF_WIDTH = 0xFF61;
    const wchar_t EXCLAMATION_MARK = 0x21;
    const wchar_t EXCLAMATION_MARK_FULL_WIDTH = 0xFF01;
    const wchar_t COLON = 0x3A;
    const wchar_t FORWARD_SLASH = 0x2F;
    const wchar_t BACK_SLASH = 0x5C;
    const wchar_t DOLLAR_SIGN = 0x24;
    const wchar_t PERCENTAGE_SIGN = 0x25;
    const wchar_t HYPHEN = 0x2D;
    const wchar_t SOFT_HYPHEN = 0xAD;
    const wchar_t LEFT_PARENTHESIS = 0x28;
    const wchar_t LEFT_PARENTHESIS_FULL_WIDTH = 0xFF08;
    const wchar_t RIGHT_PARENTHESIS = 0x29;
    const wchar_t RIGHT_PARENTHESIS_FULL_WIDTH = 0xFF09;
    const wchar_t RIGHT_BRACKET = 0x5D;
    const wchar_t INTERROBANG = 0x203D;
    //numbers
    const wchar_t NUMBER_0 = 0x30;
    const wchar_t NUMBER_1 = 0x31;
    const wchar_t NUMBER_2 = 0x32;
    const wchar_t NUMBER_3 = 0x33;
    const wchar_t NUMBER_4 = 0x34;
    const wchar_t NUMBER_5 = 0x35;
    const wchar_t NUMBER_6 = 0x36;
    const wchar_t NUMBER_7 = 0x37;
    const wchar_t NUMBER_8 = 0x38;
    const wchar_t NUMBER_9 = 0x39;
    const wchar_t NUMBER_0_FULL_WIDTH = 0xFF10;
    const wchar_t NUMBER_1_FULL_WIDTH = 0xFF11;
    const wchar_t NUMBER_2_FULL_WIDTH = 0xFF12;
    const wchar_t NUMBER_3_FULL_WIDTH = 0xFF13;
    const wchar_t NUMBER_4_FULL_WIDTH = 0xFF14;
    const wchar_t NUMBER_5_FULL_WIDTH = 0xFF15;
    const wchar_t NUMBER_6_FULL_WIDTH = 0xFF16;
    const wchar_t NUMBER_7_FULL_WIDTH = 0xFF17;
    const wchar_t NUMBER_8_FULL_WIDTH = 0xFF18;
    const wchar_t NUMBER_9_FULL_WIDTH = 0xFF19;
    //constants for letters
    const wchar_t UPPER_A = 0x41;
    const wchar_t LOWER_A = 0x61;
    const wchar_t UPPER_B = 0x42;
    const wchar_t LOWER_B = 0x62;
    const wchar_t UPPER_C = 0x43;
    const wchar_t LOWER_C = 0x63;
    const wchar_t UPPER_D = 0x44;
    const wchar_t LOWER_D = 0x64;
    const wchar_t UPPER_E = 0x45;
    const wchar_t LOWER_E = 0x65;
    const wchar_t UPPER_F = 0x46;
    const wchar_t LOWER_F = 0x66;
    const wchar_t UPPER_G = 0x47;
    const wchar_t LOWER_G = 0x67;
    const wchar_t UPPER_H = 0x48;
    const wchar_t LOWER_H = 0x68;
    const wchar_t UPPER_I = 0x49;
    const wchar_t LOWER_I = 0x69;
    const wchar_t UPPER_J = 0x4A;
    const wchar_t LOWER_J = 0x6A;
    const wchar_t UPPER_K = 0x4B;
    const wchar_t LOWER_K = 0x6B;
    const wchar_t UPPER_L = 0x4C;
    const wchar_t LOWER_L = 0x6C;
    const wchar_t UPPER_M = 0x4D;
    const wchar_t LOWER_M = 0x6D;
    const wchar_t UPPER_N = 0x4E;
    const wchar_t LOWER_N = 0x6E;
    const wchar_t UPPER_O = 0x4F;
    const wchar_t LOWER_O = 0x6F;
    const wchar_t UPPER_P = 0x50;
    const wchar_t LOWER_P = 0x70;
    const wchar_t UPPER_Q = 0x51;
    const wchar_t LOWER_Q = 0x71;
    const wchar_t UPPER_R = 0x52;
    const wchar_t LOWER_R = 0x72;
    const wchar_t UPPER_S = 0x53;
    const wchar_t LOWER_S = 0x73;
    const wchar_t UPPER_T = 0x54;
    const wchar_t LOWER_T = 0x74;
    const wchar_t UPPER_U = 0x55;
    const wchar_t LOWER_U = 0x75;
    const wchar_t UPPER_V = 0x56;
    const wchar_t LOWER_V = 0x76;
    const wchar_t UPPER_W = 0x57;
    const wchar_t LOWER_W = 0x77;
    const wchar_t UPPER_X = 0x58;
    const wchar_t LOWER_X = 0x78;
    const wchar_t UPPER_Y = 0x59;
    const wchar_t LOWER_Y = 0x79;
    const wchar_t UPPER_Z = 0x5A;
    const wchar_t LOWER_Z = 0x7A;

    const wchar_t UPPER_A_ACUTE = 0xC1;
    const wchar_t LOWER_A_ACUTE = 0xE1;
    const wchar_t UPPER_E_ACUTE = 0xC9;
    const wchar_t LOWER_E_ACUTE = 0xE9;
    const wchar_t UPPER_I_ACUTE = 0xCD;
    const wchar_t LOWER_I_ACUTE = 0xED;
    const wchar_t UPPER_O_ACUTE = 0xD3;
    const wchar_t LOWER_O_ACUTE = 0xF3;
    const wchar_t LOWER_U_ACUTE = 0xFA;
    const wchar_t UPPER_U_ACUTE = 0xDA;
    const wchar_t UPPER_A_CIRCUMFLEX = 0xC2;
    const wchar_t LOWER_A_CIRCUMFLEX = 0xE2;
    const wchar_t UPPER_E_CIRCUMFLEX = 0xCA;
    const wchar_t LOWER_E_CIRCUMFLEX = 0xEA;
    const wchar_t UPPER_I_CIRCUMFLEX = 0xCE;
    const wchar_t LOWER_I_CIRCUMFLEX = 0xEE;
    const wchar_t UPPER_A_TILDE = 0xC3;
    const wchar_t LOWER_A_TILDE = 0xE3;
    const wchar_t UPPER_O_TILDE = 0xD5;
    const wchar_t LOWER_O_TILDE = 0xF5;
    const wchar_t UPPER_N_TILDE = 0xD1;
    const wchar_t LOWER_N_TILDE = 0xF1;
    const wchar_t UPPER_O_STROKE = 0xD8;
    const wchar_t LOWER_O_STROKE = 0xF8;
    const wchar_t UPPER_C_CEDILLA = 0xC7;
    const wchar_t LOWER_C_CEDILLA = 0xE7;
    const wchar_t UPPER_A_UMLAUTS = 0xC4;
    const wchar_t LOWER_A_UMLAUTS = 0xE4;
    const wchar_t UPPER_O_UMLAUTS = 0xD6;
    const wchar_t LOWER_O_UMLAUTS = 0xF6;
    const wchar_t UPPER_E_UMLAUTS = 0xCB;
    const wchar_t LOWER_E_UMLAUTS = 0xEB;
    const wchar_t UPPER_I_UMLAUTS = 0xCF;
    const wchar_t LOWER_I_UMLAUTS = 0xEF;
    const wchar_t UPPER_ETH = 0xD0;
    const wchar_t LOWER_ETH = 0xF0;
    const wchar_t UPPER_U_UMLAUTS = 0xDC;
    const wchar_t LOWER_U_UMLAUTS = 0xFC;
    const wchar_t TILDE = 0x7E;
    const wchar_t UPPER_A_GRAVE = 0xC0;
    const wchar_t LOWER_A_GRAVE = 0xE0;
    const wchar_t UPPER_E_GRAVE = 0xC8;
    const wchar_t LOWER_E_GRAVE = 0xE8;
    const wchar_t UPPER_I_GRAVE = 0xCC;
    const wchar_t LOWER_I_GRAVE = 0xEC;
    const wchar_t UPPER_O_GRAVE = 0xD2;
    const wchar_t LOWER_O_GRAVE = 0xF2;
    const wchar_t UPPER_Y_ACUTE = 0xDD;
    const wchar_t LOWER_Y_ACUTE = 0xFD;
    const wchar_t ESZETT = 0xDF; //a.k.a. "sharp s"
    const wchar_t Y_UMLAUT = 0xFF;
    const wchar_t ELLIPSE = 0x2026;
    const wchar_t COMPOUND_WORD_SEPARATORS[5] = { HYPHEN, SOFT_HYPHEN, FORWARD_SLASH, BACK_SLASH, 0 };
    const wchar_t NUMBERS_AND_DOT[22] = {
        NUMBER_0, NUMBER_1, NUMBER_2, NUMBER_3, NUMBER_4,
        NUMBER_5, NUMBER_6, NUMBER_7, NUMBER_8, NUMBER_9,
        NUMBER_0_FULL_WIDTH, NUMBER_1_FULL_WIDTH, NUMBER_2_FULL_WIDTH, NUMBER_3_FULL_WIDTH, NUMBER_4_FULL_WIDTH,
        NUMBER_5_FULL_WIDTH, NUMBER_6_FULL_WIDTH, NUMBER_7_FULL_WIDTH, NUMBER_8_FULL_WIDTH, NUMBER_9_FULL_WIDTH,
        PERIOD, 0 };
    }

/** @}*/

#endif //__COMMON_LANG_CONSTANTS_H__
