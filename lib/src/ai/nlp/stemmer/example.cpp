#include "stemming/english_stem.h"
#include "stemming/french_stem.h"
#include "stemming/german_stem.h"
#include "stemming/finnish_stem.h"
#include "stemming/swedish_stem.h"
#include "stemming/spanish_stem.h"
#include "stemming/dutch_stem.h"
#include "stemming/danish_stem.h"
#include "stemming/italian_stem.h"
#include "stemming/norwegian_stem.h"
#include "stemming/portuguese_stem.h"
#include "stemming/russian_stem.h"
#include <string>
#include <iostream>

int stemmer_class_foundation()
    {
    //the word to be stemmed
    std::wstring word(L"transportation");
    /*create an instance of a "english_stem" class. The template argument for the
    stemmers are the type of std::basic_string that you are trying to stem, by default 
    std::wstring (Unicode strings). As long as the char type of your basic_string is wchar_t,
    then you can use any type of basic_string. This is to say, if your basic_string has a custom
    char_traits or allocator, then just specify it in your template argument to the stemmer. For example:

    typedef std::basic_string<wchar_t, myTraits, myAllocator> myString;
    myString word(L"documentation");
    stemming::english_stem<myString> StemEnglish;
    StemEnglish(word);*/
    stemming::english_stem<> StemEnglish;
    std::wcout << L"(English) Original text:\t" << word.c_str() << std::endl;
    //the "english_stem" has its operator() overloaded, so you can
    //treat your class instance like it's a function.  In this case,
    //pass in the std::wstring to be stemmed.  Note that this alters
    //the original std::wstring, so when the call is done the string will
    //be stemmed.
    StemEnglish(word);
    //now the variable "word" should equal "document"
    std::wcout << L"(English) Stemmed text:\t" << word.c_str() << std::endl;
    //try a similar word that should have the same stem
    word = L"documenting";
    std::wcout << L"(English) Original text:\t" << word.c_str() << std::endl;
    StemEnglish(word);
    //now the variable "word" should equal "document"
    std::wcout << L"(English) Stemmed text:\t" << word.c_str() << std::endl;

    //Now try a French word
    stemming::french_stem<> StemFrench;
    word = L"continuellement";
    std::wcout << L"\n(French) Original text:\t" << word.c_str() << std::endl;
    StemFrench(word);
    //now the variable "word" should equal "continuel"
    std::wcout << L"(French) Stemmed text:\t" << word.c_str() << std::endl;

    //Many other stemmers are also available
    stemming::german_stem<> StemGerman;
    stemming::finnish_stem<> StemFinnish;
    stemming::swedish_stem<> StemSwedish;
    stemming::dutch_stem<> StemDutch;
    stemming::spanish_stem<> StemSpanish;
    stemming::italian_stem<> StemItalian;
    stemming::norwegian_stem<> StemNorwgian;
    stemming::danish_stem<> StemDanish;
    stemming::portuguese_stem<> StemPortuguese;

    /*if you are using std::string (ANSI strings) then convert it to a temporary
    wchar_t buffer, assign that to a std::wstring, and then stem that.*/
    std::string ANSIWord("documentation");
    wchar_t* UnicodeTextBuffer = new wchar_t[ANSIWord.length()+1];
    std::wmemset(UnicodeTextBuffer, 0, ANSIWord.length()+1);
    std::mbstowcs(UnicodeTextBuffer, ANSIWord.c_str(), ANSIWord.length());
    word = UnicodeTextBuffer;
    StemEnglish(word);
    //now the variable "word" should equal "document"
    std::wcout << L"\nDemonstrating the stemming of an ANSI string:\n";
    std::wcout << L"(English) Original text:\t" << ANSIWord.c_str() << std::endl;
    std::wcout << L"(English) Stemmed text:\t" << word.c_str() << std::endl;

    return 0;
    }
