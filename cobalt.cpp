//      _
//     (_)           _____
//      _ \ _   _   / ____|                                 _            _
//     (_) (_) (_) | (___  _   _ _ __   ___ _ __ ___   __ _| |_ ___  ___| |__
//  _ / _   _ /     \___ \| | | | '_ \ / _ \ '_ ` _ \ / _` | __/ _ \/ __| '_ \
// (_) (_) (_)      ____) | |_| | | | |  __/ | | | | | (_| | ||  __/ (__| | | |
//        \ _      |_____/ \__, |_| |_|\___|_| |_| |_|\__,_|\__\___|\___|_| |_|
//         (_)              __/ |
//                         |___/
//
//  Product:     RaqUp.com - Universal LINUX display driver
//  Version:     Number $Revision: 1.0.0 $ $Build: 48 $
//               of $Date: 2010/05/12 15:48:45 $
//               by $Author: Andreas Pfeil <Andreas.Pfeil@Synematech.com> $
//
//  Description: Controller board protokoll driver
//
//  Copyright (C) 2010 by Synematech.com
//
// -----------------------------------------------------------------------TAB=2

#include <string.h>
#include <stdio.h>
#include <time.h>

#include "uart.cpp"

class Cobalt : public UART {
  char  injectionBuffer;
public:
  Cobalt( const char* device ) : UART( device ) {
    injectionBuffer = 0;
  }
  
  char inject( const char c ) {
    injectionBuffer = c;
  }
  
  char getc( const bool blocking = true ) {
    if( injectionBuffer != 0 ) return injectionBuffer;
    
    char  buffer = UART::getc( blocking );
    
    if( buffer == '.' ) { // PING
      write( &buffer, 1 );
      buffer = 0;
    }

    return buffer;
  }
  
  
  void sleep( unsigned int seconds, unsigned int milliSeconds = 0, unsigned int nanoSeconds = 0 ) {
    struct timespec time;
    time.tv_sec  = seconds;
    time.tv_nsec = milliSeconds * 1000000L + nanoSeconds;
    nanosleep( &time, NULL );
  }
  
  void screen( const char *line1, const char *line2 ) { 
    showCursor( false );
    clear().home().print( line1 );
    locate( 0, 1 ).print( line2 );
  }

  Cobalt& home() {
    write( "h" );
    return *this;
  }
  Cobalt& clear() {
    write( "-" );
    return *this;
  }
  
  Cobalt& shutDown() {
    write( "#" );
    setTxRx( false );
    setCollision( false );
    setLink( false );
    set100MBit( false );
    return *this;
  }
  Cobalt& reBoot() {
    write( "~" );
    setTxRx( false );
    setCollision( false );
    setLink( false );
    set100MBit( false );
    return *this;
  }
  
  Cobalt& setLink( const bool value ) {
    if( value ) write( "L" );
    else        write( "l" );
    return *this;
  }
  Cobalt& setCollision( const bool value ) {
    if( value ) write( "C" );
    else        write( "c" );
    return *this;
  }
  Cobalt& set100MBit( const bool value ) {
    if( value ) write( "!" );
    else        write( "1" );
    return *this;
  }
  Cobalt& setTemp( const bool value ) {
    if( value ) write( "T" );
    else        write( "t" );
    return *this;
  }
  Cobalt& setWeb( const bool value ) { return setTemp( value ); }  
  
  Cobalt& setTxRx( const bool value ) {
    if( value ) write( "X" );
    else        write( "x" );
    return *this;
  }
  Cobalt& showCursor( const bool value ) {
    if( value ) write( "S" );
    else        write( "s" );
    return *this;
  }
  Cobalt& enableDisplay( const bool value ) {
    if( value ) write( "E" );
    else        write( "e" );
    return *this;
  }
  Cobalt& locate( const unsigned char x, const unsigned char y ) {
    char data[ 3 ] = "g ";
         data[ 1 ] = x + y * 16;
         write( data, 2 );
    return *this;
  }
  Cobalt& print( const char *line ) {
    char data[ 18 ] = ">",
         i;
    for( i = 0; i < 16 && line[ i ] != 0; i++ ) data[ i + 1 ] = line[ i ];
    data[ i + 1 ] = 0;
    write( data, i + 2 );
    return *this;
  }
  Cobalt& poke( const char c ) {
    char data[] = { '>', c, 0 };
    write( data, 3 );
    return *this;
  }  
};
