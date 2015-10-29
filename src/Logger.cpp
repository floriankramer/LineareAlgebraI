/*
 * Logger.cpp
 *
 *  Created on: Dec 2, 2014
 *      Author: dwarf
 */

#include "Logger.h"
#include <iostream>
#include "os.h"
#ifdef windows
#include <windows.h>
#endif
#ifdef unix
#include <execinfo.h>
#endif


Logger::Logger(std::string name) : name(name) {

}

Logger::~Logger() {
}

void Logger::logCurrentStackTrace(){
#ifdef unix
    int j, nptrs;
    void *buffer[100];
    char **strings;

    nptrs = backtrace(buffer, 100);
    printf("backtrace() returned %d addresses\n", nptrs);

    /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
       would produce similar output to the following: */

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    for (j = 0; j < nptrs; j++)
        printf("%s\n", strings[j]);

    free(strings);
#endif
#ifdef windows
    /*unsigned int   i; //TODO reimplement stack trace printing under windows, if thought necessary
    void         * stack[ 100 ];
    unsigned short frames;
    SYMBOL_INFO  * symbol;
    HANDLE         process;

    process = GetCurrentProcess();
    SymInitialize( process, NULL, TRUE );
    frames               = CaptureStackBackTrace( 0, 100, stack, NULL );
    symbol               = ( SYMBOL_INFO * )calloc( sizeof( SYMBOL_INFO ) + 256 * sizeof( char ), 1 );
    symbol->MaxNameLen   = 255;
    symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

    for( i = 0; i < frames; i++ )
    {
        SymFromAddr( process, ( DWORD64 )( stack[ i ] ), 0, symbol );

        printf( "%i: %s - 0x%0X\n", frames - i - 1, symbol->Name, symbol->Address );
    }

    free( symbol );*/
#endif

}

void Logger::setName(std::string n){
	name = n;
}

std::string Logger::getName(){
 return name;
}

