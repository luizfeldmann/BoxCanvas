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
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.            //
//                                                                                      //
//    Copyright: Luiz Gustavo Pfitscher e Feldmann, 2020                                //
// ===================================================================================  //

#include "boxcanvas.h"
#include <stdio.h>
#include <stdlib.h>

void BoxCanvas_Create(BoxCanvas *canvas, uint8_t X, uint8_t Y, uint8_t W, uint8_t H)
{
    uint8_t WidthColumns, HeightRows;

    if (W == 0 || H == 0)
        Terminal_GetSize(&WidthColumns, &HeightRows);

    canvas->Top = Y;
    canvas->Left = X;
    canvas->Width = (W == 0) ? WidthColumns : W;
    canvas->Height = (H == 0) ? HeightRows : H;
    canvas->BackgroundStyle = CONSOLE_STYLE_BACKGROUND_GREY;
    canvas->FillStyle = CONSOLE_STYLE_TEXT_WHITE;

    canvas->BlockBuffer = (uint8_t**)malloc(canvas->Height * sizeof(uint8_t*));
    for (size_t row = 0; row < canvas->Height; row++)
    {
        canvas->BlockBuffer[row] = (uint8_t*)malloc(canvas->Width * sizeof(uint8_t));
        memset(canvas->BlockBuffer[row], 0, canvas->Width * sizeof(uint8_t));
    }
}

void BoxCanvas_Destroy(BoxCanvas *canvas)
{
    for (size_t row = 0; row < canvas->Height; row++)
        free((uint8_t *)canvas->BlockBuffer[row]); // free columns memory

    free((uint8_t **)canvas->BlockBuffer); // free rows memory
}

void BoxCanvas_GetCharacter(uint8_t boxcode, uint8_t *boxascii, uint32_t* boxunicode)
{
    uint8_t boxstrong = boxcode & BOX_FLAG_STRONG;

    if (boxcode == 0) // no code ...
    {
        *boxascii = ' '; // ... no box
        *boxunicode = ' ';
    }
    else if (boxcode & BOX_FLAG_FILL)
    {
        *boxascii = 219; // full painted box
        *boxunicode = 0x2588;
    }
    else switch (boxcode & BOX_MASK_BORDER)
    {
        case BOX_FLAG_UP:
        case BOX_FLAG_DOWN:
        case BOX_FLAG_UP | BOX_FLAG_DOWN: // vertical line |
            *boxascii = (boxstrong) ? 186 : 179;
            *boxunicode  = (boxstrong) ?  0x2551 : 0x2502 ;
            break;

        case BOX_FLAG_LEFT:
        case BOX_FLAG_RIGHT:
        case BOX_FLAG_LEFT | BOX_FLAG_RIGHT:  // horizontal line -
            *boxascii = (boxstrong) ? 205 : 196;
            *boxunicode  = (boxstrong) ?  0x2550 : 0x2500 ;
            break;

        case BOX_FLAG_UP | BOX_FLAG_DOWN | BOX_FLAG_LEFT | BOX_FLAG_RIGHT: // cross line +
            *boxascii = (boxstrong) ? 206 : 197;
            *boxunicode  = (boxstrong) ?  0x256C : 0x253C ;
            break;

        case BOX_FLAG_UP | BOX_FLAG_LEFT:  // _|
            *boxascii = (boxstrong) ? 188 : 217;
            *boxunicode  = (boxstrong) ?  0x255D : 0x2518 ;
            break;

        case BOX_FLAG_UP | BOX_FLAG_RIGHT: // |_
            *boxascii = (boxstrong) ? 200 : 192;
            *boxunicode  = (boxstrong) ?  0x255A : 0x2514 ;
            break;

        case BOX_FLAG_DOWN | BOX_FLAG_LEFT: // ¨|
            *boxascii = (boxstrong) ? 187 : 191;
            *boxunicode  = (boxstrong) ?  0x2557 : 0x2510 ;
            break;

        case BOX_FLAG_DOWN | BOX_FLAG_RIGHT: // |¨
            *boxascii = (boxstrong) ? 201 : 218;
            *boxunicode  = (boxstrong) ?  0x2554 : 0x250C ;
            break;

        case BOX_FLAG_UP | BOX_FLAG_LEFT | BOX_FLAG_RIGHT: // _|_
            *boxascii = (boxstrong) ? 202 : 193;
            *boxunicode  = (boxstrong) ?  0x2569 : 0x2534 ;
            break;

        case BOX_FLAG_DOWN | BOX_FLAG_LEFT | BOX_FLAG_RIGHT: // ¨|¨
            *boxascii = (boxstrong) ? 203 : 194;
            *boxunicode  = (boxstrong) ?  0x2566 : 0x252C ;
            break;

        case BOX_FLAG_UP | BOX_FLAG_DOWN | BOX_FLAG_LEFT: // -|
            *boxascii = (boxstrong) ? 185 : 180;
            *boxunicode  = (boxstrong) ?  0x2563 : 0x2524 ;
            break;

        case BOX_FLAG_UP | BOX_FLAG_DOWN | BOX_FLAG_RIGHT: // |-
            *boxascii = (boxstrong) ? 204 : 195;
            *boxunicode  = (boxstrong) ?  0x2560 : 0x251C ;
            break;

        default:
            if (boxcode & BOX_FLAG_DOTTED)
            {
                *boxascii = (boxstrong) ? 177 : 176;
                *boxunicode  = (boxstrong) ?  0x2592 : 0x2591 ;
            }
            else
            {
                *boxascii = ' ';
                *boxunicode  = ' ';
            }
        break;
    }
}

void BoxCanvas_Render(BoxCanvas *canvas)
{
    Terminal_SaveCursorPosition(); // let's save the current state before we do anything

    Terminal_SetStyle(canvas->FillStyle, canvas->BackgroundStyle); // set the style
    Terminal_ClearArea(canvas->Left, canvas->Top, canvas->Width, canvas->Height);

    #if (defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__))
        uint8_t use_utf8 = (GetConsoleOutputCP() == CP_UTF8);
    #else
        uint8_t use_utf8 = 1; // always use utf-8 under UNIX
    #endif

    char print_line_buffer[canvas->Width*5];
    for (int16_t row = 0; row < canvas->Height; row++) // iterate over the rows and print along the lines (natural printing left to right)
    {
        memset(print_line_buffer, 0, sizeof(print_line_buffer)); // erase garbage from previous iteration with '\0'
        for (int16_t col = 0; col < canvas->Width; col++)
        {
            uint8_t ascii;
            uint32_t unicode;
            char utf8[5];

            BoxCanvas_GetCharacter(canvas->BlockBuffer[row][col], &ascii, &unicode);

            if (use_utf8)
            {
                utf8_encode(utf8, unicode);
                strcat(print_line_buffer, utf8);
            }
            else
                print_line_buffer[strlen(print_line_buffer)] = ascii;
        }

        Terminal_SetCursorPosition(canvas->Left, canvas->Top + row);
        fwrite(print_line_buffer, sizeof(char)*strlen(print_line_buffer), 1, stdout);
        //printf("%s", print_line_buffer);
    }

    Terminal_RestoreCursorSavedPosition();
}

void BoxCanvas_Box(BoxCanvas *canvas, uint8_t X, uint8_t Y, uint8_t W, uint8_t H, BoxDrawStyle style)
{
    uint8_t maxX = min(X + W, canvas->Width);
    uint8_t maxY = min(Y + H, canvas->Height);

    uint8_t shadow = style & BOX_STYLE_SHADOW;
    uint8_t fill = style & BOX_STYLE_FILL;
    uint8_t strong = style & BOX_STYLE_STRONG;

    for (uint8_t currX = X; currX < maxX + (shadow && maxX < canvas->Width) ? 1 : 0; currX++)
    {
        for (uint8_t currY = Y; currY < maxY + (shadow && maxY < canvas->Height); currY++)
        {
            uint8_t code = 0;

            if (currX == X && currY == Y) // top left corner
                code |= BOX_FLAG_DOWN | BOX_FLAG_RIGHT;
            else if (currX == maxX-1 && currY == Y) // top right corner
                code |= BOX_FLAG_DOWN | BOX_FLAG_LEFT;
            else if (currX == X && currY == maxY-1) // bottom left corner
                code |= BOX_FLAG_UP | BOX_FLAG_RIGHT;
            else if (currX == maxX-1 && currY == maxY-1) // bottom right corner
                code |= BOX_FLAG_UP | BOX_FLAG_LEFT;
            else if (currX == maxX || currY == maxY) // shadow
            {
                if ((currX > X + 2) && (currY > Y + 1))
                    code |= BOX_FLAG_DOTTED;
            }
            else if (currX == X || currX == maxX-1) // left or right side
                code |= BOX_FLAG_UP | BOX_FLAG_DOWN;
            else if (currY == Y || currY == maxY-1) // top or bottom
                code |= BOX_FLAG_LEFT | BOX_FLAG_RIGHT;
            else (void)0; // interior

            if (code != 0 && strong) code |= BOX_FLAG_STRONG;

            if (fill)
                canvas->BlockBuffer[currY][currX] = code;
            else
                canvas->BlockBuffer[currY][currX] |= code;
        }
    }
}
