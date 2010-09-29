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

#include "cobalt.h"

Cobalt::Cobalt( const char* device ) : UART( device ) {
  injectionBuffer = 0;
}
  
char Cobalt::inject( const char c ) {
  injectionBuffer = c;
}
  
char Cobalt::getc( const bool blocking ) {
  if( injectionBuffer != 0 ) return injectionBuffer;
  
  char  buffer = UART::getc( blocking );
  
  if( buffer == '.' ) { // PING
    write( &buffer, 1 );
    buffer = 0;
  }

  return buffer;
}
  
  
void Cobalt::sleep( unsigned int seconds, unsigned int milliSeconds, unsigned int nanoSeconds ) {
  struct timespec time;
  time.tv_sec  = seconds;
  time.tv_nsec = milliSeconds * 1000000L + nanoSeconds;
  nanosleep( &time, NULL );
}
  
void Cobalt::screen( const char *line1, const char *line2 ) { 
  showCursor( false );
  clear().home().print( line1 );
  locate( 0, 1 ).print( line2 );
}

Cobalt& Cobalt::home() {
  write( "h" );
  return *this;
}

Cobalt& Cobalt::clear() {
  write( "-" );
  return *this;
}
  
Cobalt& Cobalt::shutDown() {
  write( "#" );
  setTxRx( false );
  setCollision( false );
  setLink( false );
  set100MBit( false );
  return *this;
}

Cobalt& Cobalt::reBoot() {
  write( "~" );
  setTxRx( false );
  setCollision( false );
  setLink( false );
  set100MBit( false );
  return *this;
}
  
Cobalt& Cobalt::setLink( const bool value ) {
  if( value ) write( "L" );
  else        write( "l" );
  return *this;
}
  
Cobalt& Cobalt::setCollision( const bool value ) {
  if( value ) write( "C" );
  else        write( "c" );
  return *this;
}

Cobalt& Cobalt::set100MBit( const bool value ) {
  if( value ) write( "!" );
  else        write( "1" );
  return *this;
}
  
Cobalt& Cobalt::setTemp( const bool value ) {
  if( value ) write( "T" );
  else        write( "t" );
  return *this;
}

Cobalt& Cobalt::setWeb( const bool value ) { 
  return setTemp( value ); 
}  
  
Cobalt& Cobalt::setTxRx( const bool value ) {
  if( value ) write( "X" );
  else        write( "x" );
  return *this;
}
  
Cobalt& Cobalt::showCursor( const bool value ) {
  if( value ) write( "S" );
  else        write( "s" );
  return *this;
}
  
Cobalt& Cobalt::enableDisplay( const bool value ) {
  if( value ) write( "E" );
  else        write( "e" );
  return *this;
}

Cobalt& Cobalt::locate( const unsigned char x, const unsigned char y ) {
  char data[ 3 ] = "g ";
       data[ 1 ] = x + y * 16;
       write( data, 2 );
  return *this;
}

Cobalt& Cobalt::print( const char *line ) {
  char data[ 18 ] = ">",
       i;
  for( i = 0; i < 16 && line[ i ] != 0; i++ ) data[ i + 1 ] = line[ i ];
  data[ i + 1 ] = 0;
  write( data, i + 2 );
  return *this;
}
  
Cobalt& Cobalt::poke( const char c ) {
  char data[] = { '>', c, 0 };
  write( data, 3 );
  return *this;
}  
