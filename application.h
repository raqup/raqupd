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
//  Description: Application controller
//
//  Copyright (C) 2010 by Synematech.com
//
// -----------------------------------------------------------------------TAB=2

#ifndef _APPLICATION
#define _APPLICATION

// http://beej.us/guide/bgnet/output/html/multipage/inet_ntopman.html

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>

#include "cobalt.h"
#include "service.h"
#include "interface.h"

class Application {
  long          _user, _nice, _system, _idle, _iowait, _irq, _softirq, dummy;
  long          _recive_packets, _recive_errors,
                _transmit_packets, _transmit_errors, _transmit_collisions;
  unsigned int  delay, externDelay;
  FILE          *mii_tool;
  bool          _rxtx, _collision;

  char          externLine1[ 16 ],
                externLine2[ 16 ];

  Cobalt        cobalt;
  Interface     interface;
  Service       service;

  enum Screen { INFO, CPU, MEM, LOAD, UPTIME,

                EXTERN,

                SETUP_NETWORK, NETWORK,
                SETUP_SERVICE, SERVICE,
                REBOOT, REBOOT_DO,
                SHUTDOWN, SHUTDOWN_DO,
                RESET_PASSWORD,

                SKIP, SELECT
              } last, next;

  void        formatNumber( char *buffer, float value );

public:
              Application( const char* device, const unsigned int delay = 100 );
             ~Application();

  void        print( const char *line1, const char *line2, const unsigned int delay = 10 );

  int         run();

  void        updateLEDs();

  enum Screen sysInfo( enum Screen previous, enum Screen next );
  enum Screen cpuInfo( enum Screen previous, enum Screen next );
  enum Screen memInfo( enum Screen previous, enum Screen next );
  enum Screen loadInfo( enum Screen previous, enum Screen next );
  enum Screen uptimeInfo( enum Screen previous, enum Screen next );
  enum Screen menuItem( const char *line1, const char *line2, enum Screen next, enum Screen select );
  int         request( const char *question, const char *option1, const char *option2, const int defaultOption = 0 );
  const char* editIP( const char *line1, const char *address );

  void        configServices();
  void        resetPassword();
};

#endif