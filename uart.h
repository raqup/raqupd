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
//  Description: UART Interface controller
//
//  Copyright (C) 2010 by Synematech.com
//
// -----------------------------------------------------------------------TAB=2

#ifndef _UART
#define _UART

// http://tldp.org/HOWTO/Serial-Programming-HOWTO/x115.html

// http://www.vanemery.com/Linux/Serial/serial-console.html
// http://www.linusakesson.net/programming/tty/index.php

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>   // bzero
#include <stdlib.h>   // 
#include <unistd.h>   // close

class UART {
  int             fd;
  fd_set          readset,
                  writeset;
  struct  termios parameter, parameter_backup;
 
  // flag=file status flags to turn on  
  void    set_fl( int fd, int flags );
  void    clr_fl( int fd, int flags );
  
public:
          UART( const char* device );
         ~UART();
  
  bool    hasData();
  char    getc( const bool blocking = true );
  int     read( char *buffer, unsigned int maxLength );
  void    write( const char *buffer, const int length = -1 );
};

#endif