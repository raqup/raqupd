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

#ifndef _COBALT
#define _COBALT 

#include <string.h>
#include <stdio.h>
#include <time.h>

#include "uart.h"

class Cobalt : public UART {
  char    injectionBuffer;
  
public:
          Cobalt( const char* device );
  
  char    inject( const char c );
  
  char    getc( const bool blocking = true );
  
  
  void    sleep( unsigned int seconds, unsigned int milliSeconds = 0, unsigned int nanoSeconds = 0 );
  
  void    screen( const char *line1, const char *line2 );

  Cobalt& home();
  Cobalt& clear();
  
  Cobalt& shutDown();
  Cobalt& reBoot();
  
  Cobalt& setLink( const bool value );
  Cobalt& setCollision( const bool value );
  Cobalt& set100MBit( const bool value );
  Cobalt& setTemp( const bool value );
  Cobalt& setWeb( const bool value );
  
  Cobalt& setTxRx( const bool value );
  Cobalt& showCursor( const bool value );
  Cobalt& enableDisplay( const bool value );
  Cobalt& locate( const unsigned char x, const unsigned char y );
  Cobalt& print( const char *line );
  Cobalt& poke( const char c );
};

#endif