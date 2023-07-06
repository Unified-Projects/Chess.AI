#pragma once

#include <string>

/* FOREGROUND */
#define RST  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define FRED(x) KRED + x + RST
#define FGRN(x) KGRN + x + RST
#define FYEL(x) KYEL + x + RST
#define FBLU(x) KBLU + x + RST
#define FMAG(x) KMAG + x + RST
#define FCYN(x) KCYN + x + RST
#define FWHT(x) KWHT + x + RST

#define BOLD(x) "\x1B[1m" x RST
#define UNDL(x) "\x1B[4m" x RST

enum LogType{
    ERR,
    WARN,
    SUCCESS,
    NORMAL,
    TIME
};

std::string FMT(std::string s, LogType t){
    switch (t)
    {
    case ERR:
        return FRED(s);
    case WARN:
        return FMAG(s);
    case SUCCESS:
        return FGRN(s);
    case NORMAL:
        return FBLU(s);
    case TIME:
        return FCYN(s);
    default:
        return "";
    }
}