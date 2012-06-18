#pragma once
#include <cstdio>
#include <stdarg.h>
#include <QString>

class StarlabException : public std::exception{
protected:  
    QString errmsg;
    
public:
    void init(){
        errmsg = "error not specified";
    }
    StarlabException(){ init(); }
    StarlabException(const QString& text){ 
        init();
        errmsg = text; 
    }
       
    /// This throws an exeption with a format similar to a printf
    /// Make sure you are passing actual char* or it will terminate badly!!
    StarlabException(const char *format, ...){
        char buffer[256];
        va_list args;
        va_start (args, format);

#ifdef WIN32
        vsprintf_s (buffer,format, args);
#else
        vsprintf (buffer,format, args);
#endif

        va_end (args);
        errmsg = QString(buffer);
    }
    
    ~StarlabException() throw() {}

    /// For std::exception
    virtual inline const char* what() const throw() {
        return qPrintable(errmsg);
    }
     
    /// Used in the title of the messagebox
    virtual QString type(){ return "Starlab Exception"; }
    virtual QString message(){ return errmsg; }
};
