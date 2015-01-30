/* Debugging utilities */


#ifndef PHOTOBOOTH_DEBUG_H_INCLUDED
#define PHOTOBOOTH_DEBUG_H_INCLUDED

#ifdef DEBUG
    #define debugPrint(str) \
       Serial.print(millis()); \
       Serial.print(": "); \
       Serial.print(__PRETTY_FUNCTION__); \
       Serial.print(':'); \
       Serial.print(__LINE__); \
       Serial.print(' '); \
       Serial.print(str); \
       Serial.print('\n');
#else
    #define debugPrint(str) {}
#endif

#endif
