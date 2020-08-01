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

#include "port_kbhit.h"

#if defined(unix) || defined(__unix__) || defined(__unix)

#include <fcntl.h>
//#include <sys/types.h>
//#include <sys/time.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

char initialized = 0;
char savedMode = 0;

struct termios termOriginal, termModified;

void SetMode(char mode)
{
    if (!initialized)
    {
        tcgetattr(0, &termOriginal);

        termModified = termOriginal;
        termModified.c_lflag &= ~ICANON;
        termModified.c_lflag &= ~ECHO;
    }

    if (mode == savedMode) // avoid unnecessary system-calls
        return;

    if (mode == 0)
        tcsetattr(0, TCSANOW, &termOriginal); // echo on
    else
        tcsetattr(0, TCSANOW, &termModified); // echo off

    savedMode = mode;
}

char kbhit(void)
{
    SetMode(1); // echo off

    int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK); // getchar blocking

    char ch = getchar();

    fcntl(STDIN_FILENO, F_SETFL, oldf); // getchar non-blocking

    if(ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
    /*SetMode(1);

    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting); // check for bytes waiting

    return (byteswaiting > 0);*/
}

char getch(void)
{
    SetMode(1);
    char ch = getchar();
    SetMode(0);

    return ch;
}

char getchNavigation(void)
{
    char ch = getch();
    if (ch != 27) // start escape sequence
        return ch;

    ch = getch();
    if (ch != '[')
        return ch;

    ch = getch();
    if (ch == '1')
    {
        char ch = getch();
        if (ch != ';')
            return ch;

        ch = getch();
        if (ch != '2')
            return ch;

        ch = getch();
        switch (ch)
        {
            case UNIX_ARROW_ESCAPE_UP:      return KEY_PAGE_UP; break;
            case UNIX_ARROW_ESCAPE_DOWN:    return KEY_PAGE_DOWN; break;
            default: return ch; break;
        }
    }
    else
    {
        switch (ch)
        {
            case UNIX_ARROW_ESCAPE_UP:      return KEY_ARROW_UP;    break;
            case UNIX_ARROW_ESCAPE_DOWN:    return KEY_ARROW_DOWN;  break;
            case UNIX_ARROW_ESCAPE_LEFT:    return KEY_ARROW_LEFT;  break;
            case UNIX_ARROW_ESCAPE_RIGHT:   return KEY_ARROW_RIGHT; break;

            default: return ch;
        }
    }
}

#endif

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__))
char getchNavigation(void)
{
    char ch = getch();
    if (ch != WINDOWS_ESCAPE)
        return ch;

    ch = getch(); // get the new, actual command

    switch (ch)
    {
        case WINDOWS_ARROW_ESCAPE_UP:       return KEY_ARROW_UP;    break;
        case WINDOWS_ARROW_ESCAPE_DOWN:     return KEY_ARROW_DOWN;  break;
        case WINDOWS_ARROW_ESCAPE_LEFT:     return KEY_ARROW_LEFT;  break;
        case WINDOWS_ARROW_ESCAPE_RIGHT:    return KEY_ARROW_RIGHT; break;
        case WINDOWS_PAGE_ESCAPE_UP:        return KEY_PAGE_UP; break;
        case WINDOWS_PAGE_ESCAPE_DOWN:      return KEY_PAGE_DOWN; break;
        default: return ch; break;
    }
}
#endif
