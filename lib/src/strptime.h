#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#if defined(_WIN32)
const char * strp_weekdays[] =
    { "sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday"};
const char * strp_monthnames[] =
    { "january", "february", "march", "april", "may", "june", "july", "august", "september", "october", "november", "december"};
bool strp_atoi(const char * & s, int & result, int low, int high, int offset)
    {
    bool worked = false;
    char * end;
    unsigned long num = strtoul(s, & end, 10);
    if (num >= (unsigned long)low && num <= (unsigned long)high)
        {
        result = (int)(num + offset);
        s = end;
        worked = true;
        }
    return worked;
    }
char * strptime(const char *s, const char *format, struct tm *tm)
    {
    bool working = true;
    while (working && *format && *s)
        {
        switch (*format)
            {
        case '%':
            {
            ++format;
            switch (*format)
                {
            case 'a':
            case 'A': // weekday name
                tm->tm_wday = -1;
                working = false;
                for (size_t i = 0; i < 7; ++ i)
                    {
                    size_t len = strlen(strp_weekdays[i]);
                    if (!strnicmp(strp_weekdays[i], s, len))
                        {
                        tm->tm_wday = i;
                        s += len;
                        working = true;
                        break;
                        }
                    else if (!strnicmp(strp_weekdays[i], s, 3))
                        {
                        tm->tm_wday = i;
                        s += 3;
                        working = true;
                        break;
                        }
                    }
                break;
            case 'b':
            case 'B':
            case 'h': // month name
                tm->tm_mon = -1;
                working = false;
                for (size_t i = 0; i < 12; ++ i)
                    {
                    size_t len = strlen(strp_monthnames[i]);
                    if (!strnicmp(strp_monthnames[i], s, len))
                        {
                        tm->tm_mon = i;
                        s += len;
                        working = true;
                        break;
                        }
                    else if (!strnicmp(strp_monthnames[i], s, 3))
                        {
                        tm->tm_mon = i;
                        s += 3;
                        working = true;
                        break;
                        }
                    }
                break;
            case 'd':
            case 'e': // day of month number
                working = strp_atoi(s, tm->tm_mday, 1, 31, 0);
                break;
            case 'D': // %m/%d/%y
                {
                const char * s_save = s;
                working = strp_atoi(s, tm->tm_mon, 1, 12, -1);
                if (working && *s == '/')
                    {
                    ++ s;
                    working = strp_atoi(s, tm->tm_mday, 1, 31, 0);
                    if (working && *s == '/')
                        {
                        ++ s;
                        working = strp_atoi(s, tm->tm_year, 0, 99, 0);
                        if (working && tm->tm_year < 69)
                            tm->tm_year += 100;
                        }
                    }
                if (!working)
                    s = s_save;
                }
                break;
            case 'H': // hour
                working = strp_atoi(s, tm->tm_hour, 0, 23, 0);
                break;
            case 'I': // hour 12-hour clock
                working = strp_atoi(s, tm->tm_hour, 1, 12, 0);
                break;
            case 'j': // day number of year
                working = strp_atoi(s, tm->tm_yday, 1, 366, -1);
                break;
            case 'm': // month number
                working = strp_atoi(s, tm->tm_mon, 1, 12, -1);
                break;
            case 'M': // minute
                working = strp_atoi(s, tm->tm_min, 0, 59, 0);
                break;
            case 'n': // arbitrary whitespace
            case 't':
                while (isspace((int)*s))
                    ++s;
                break;
            case 'p': // am / pm
                if (!strnicmp(s, "am", 2))
                    { // the hour will be 1 -> 12 maps to 12 am, 1 am .. 11 am, 12 noon 12 pm .. 11 pm
                    if (tm->tm_hour == 12) // 12 am == 00 hours
                        tm->tm_hour = 0;
                    s += 2;
                    }
                else if (!strnicmp(s, "pm", 2))
                    {
                    if (tm->tm_hour < 12) // 12 pm == 12 hours
                        tm->tm_hour += 12; // 1 pm -> 13 hours, 11 pm -> 23 hours
                    s += 2;
                    }
                else
                    working = false;
                break;
            case 'r': // 12 hour clock %I:%M:%S %p
                {
                const char * s_save = s;
                working = strp_atoi(s, tm->tm_hour, 1, 12, 0);
                if (working && *s == ':')
                    {
                    ++ s;
                    working = strp_atoi(s, tm->tm_min, 0, 59, 0);
                    if (working && *s == ':')
                        {
                        ++ s;
                        working = strp_atoi(s, tm->tm_sec, 0, 60, 0);
                        if (working && isspace((int)*s))
                            {
                            ++ s;
                            while (isspace((int)*s))
                                ++s;
                            if (!strnicmp(s, "am", 2))
                                { // the hour will be 1 -> 12 maps to 12 am, 1 am .. 11 am, 12 noon 12 pm .. 11 pm
                                if (tm->tm_hour == 12) // 12 am == 00 hours
                                    tm->tm_hour = 0;
                                }
                            else if (!strnicmp(s, "pm", 2))
                                {
                                if (tm->tm_hour < 12) // 12 pm == 12 hours
                                    tm->tm_hour += 12; // 1 pm -> 13 hours, 11 pm -> 23 hours
                                }
                            else
                                working = false;
                            }
                        }
                    }
                if (!working)
                    s = s_save;
                }
                break;
            case 'R': // %H:%M
                {
                const char * s_save = s;
                working = strp_atoi(s, tm->tm_hour, 0, 23, 0);
                if (working && *s == ':')
                    {
                    ++ s;
                    working = strp_atoi(s, tm->tm_min, 0, 59, 0);
                    }
                if (!working)
                    s = s_save;
                }
                break;
            case 'S': // seconds
                working = strp_atoi(s, tm->tm_sec, 0, 60, 0);
                break;
            case 'T': // %H:%M:%S
                {
                const char * s_save = s;
                working = strp_atoi(s, tm->tm_hour, 0, 23, 0);
                if (working && *s == ':')
                    {
                    ++ s;
                    working = strp_atoi(s, tm->tm_min, 0, 59, 0);
                    if (working && *s == ':')
                        {
                        ++ s;
                        working = strp_atoi(s, tm->tm_sec, 0, 60, 0);
                        }
                    }
                if (!working)
                    s = s_save;
                }
                break;
            case 'w': // weekday number 0->6 sunday->saturday
                working = strp_atoi(s, tm->tm_wday, 0, 6, 0);
                break;
            case 'Y': // year
                working = strp_atoi(s, tm->tm_year, 1900, 65535, -1900);
                break;
            case 'y': // 2-digit year
                working = strp_atoi(s, tm->tm_year, 0, 99, 0);
                if (working && tm->tm_year < 69)
                    tm->tm_year += 100;
                break;
            case '%': // escaped
                if (*s != '%')
                    working = false;
                ++s;
                break;
            default:
                working = false;
                }
            }
            break;
        case ' ':
        case '\t':
        case '\r':
        case '\n':
        case '\f':
        case '\v':
            // zero or more whitespaces:
            while (isspace((int)*s))
                ++ s;
            break;
        default:
            // match character
            if (*s != *format)
                working = false;
            else
                ++s;
            break;
            }
        ++format;
        }
    return (working?(char *)s:0);
    }
#endif
