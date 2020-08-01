// ===================================================================================  //
//    This program is free software: you can redistribute it and/or modify              //
//    it under the terms of the GNU General Public License as published by              //
//    the Free Software Foundation, either version 3 of the License, or                 //
//    (at your option) any later version.                                               //
//                                                                                      //
//    This program is distributed in the hope that it will be useful,                   //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of                    //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                     //
//    GNU General Public License for more details.                                      //
//                                                                                      //
//    You should have received a copy of the GNU General Public License                 //
//    along with this program.  If not, see <https://www.gnu.org/licenses/>5.           //
//                                                                                      //
//    Copyright: Luiz Gustavo Pfitscher e Feldmann, 2020                                //
// ===================================================================================  //

#ifndef _PORT_KBHIT_H_
#define _PORT_KBHIT_H_

    #define KEY_ENTER                '\n'
    #define KEY_RETURN               '\r'
    #define KEY_ESC                   27

    #define KEY_ARROW_UP              17    // ASCII "DEVICE CONTROL 0"
    #define KEY_ARROW_DOWN            18    // ASCII "DEVICE CONTROL 1"
    #define KEY_ARROW_LEFT            19    // ASCII "DEVICE CONTROL 2"
    #define KEY_ARROW_RIGHT           20    // ASCII "DEVICE CONTROL 3"
    #define KEY_PAGE_UP               -17
    #define KEY_PAGE_DOWN             -18

    char getchNavigation(void);

    #if (defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__))
        #include <conio.h> // functions kbhit(), getch() already defined here

        #define KEY_BACKSPACE                   '\b'
        #define WINDOWS_ARROW_ESCAPE_UP         72
        #define WINDOWS_ARROW_ESCAPE_DOWN       80
        #define WINDOWS_ARROW_ESCAPE_LEFT       75
        #define WINDOWS_ARROW_ESCAPE_RIGHT      77
        #define WINDOWS_PAGE_ESCAPE_UP          73
        #define WINDOWS_PAGE_ESCAPE_DOWN        81
        #define WINDOWS_ESCAPE                  -32

    #endif // WINDOWS

    #if defined(unix) || defined(__unix__) || defined(__unix)
        #define KEY_BACKSPACE                 127
        #define UNIX_ARROW_ESCAPE_UP          'A'
        #define UNIX_ARROW_ESCAPE_DOWN        'B'
        #define UNIX_ARROW_ESCAPE_LEFT        'D'
        #define UNIX_ARROW_ESCAPE_RIGHT       'C'

        char kbhit(void); // lets declare and implement ourselves
        char getch(void);
    #endif // UNIX

#endif
