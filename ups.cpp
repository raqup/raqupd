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
//  Description: UPS controller
//
//  Copyright (C) 2010 by Synematech.com
//
// -----------------------------------------------------------------------TAB=2

class UPS {
  char  _ip[ 32 ];
  bool  _isMaster,
        _isEnabled;
public:
  enum STATE { UNDEFINED, POWER_OK, POWER_RESTORED, ON_BATTERY };
  
  UPS() {
    strcpy( _ip, "000.000.000.000" );
    _isMaster   = false;
    _isEnabled  = false;
    
    load();
  }
  
  bool isEnabled() const { return _isEnabled; }
  bool isMaster() const { return _isMaster; }
  enum STATE getState() const { return _state; }
  
  void enable( const bool enabled ) {
    _isEnabled = enabled;
  }
  void setMaster( const bool isMaster ) { 
    _isMaster = isMaster;
  }
  void setServerIP( const char *ip ) {
    strncpy( _ip, ip, sizeof( _ip ) );
  }
  const char* getServerIP() const {
    return _ip;
  }
  
  bool load() {
    return true;
  }
  bool save() {
    return true;
  }

private:
  enum STATE _state;
};