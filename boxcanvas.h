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
#ifndef _BOX_CANVAS_H_
#define _BOX_CANVAS_H_

#include <stdint.h>
#include "../BrailleCanvas/BrailleCanvas/terminal.h" // -- get this file (and the matching .c file too) in the repo "BrailleCanvas" at: https://github.com/luizfeldmann/BrailleCanvas

// box flags <UP> <DOWN> <LEFT> <RIGHT> <?> <STRONG> <DOTTED> <FILL>
#define BOX_FLAG_UP 0b10000000
#define BOX_FLAG_DOWN 0b01000000
#define BOX_FLAG_LEFT 0b00100000
#define BOX_FLAG_RIGHT 0b00010000
#define BOX_FLAG_FILL 0b00000001
#define BOX_FLAG_DOTTED 0b00000010
#define BOX_FLAG_STRONG 0b0000100
#define BOX_MASK_BORDER 0b11110000

typedef enum { // <> <> <> <> <> <STRONG> <DOTTED/SHADOW> <FILL>
    BOX_STYLE_FILL = BOX_FLAG_FILL,
    BOX_STYLE_NOFILL = 0b00000000,
    BOX_STYLE_SHADOW = BOX_FLAG_DOTTED,
    BOX_STYLE_NOSHADOW = 0b00000000,
    BOX_STYLE_STRONG = BOX_FLAG_STRONG,
    BOX_STYLE_WEAK = 0b00000000,
} BoxDrawStyle;

typedef struct _BoxCanvas
{
    uint8_t Top;
    uint8_t Left;
    uint8_t Width;
    uint8_t Height;

    ConsoleStyleText FillStyle;
    ConsoleStyleBackground BackgroundStyle;

    uint8_t **BlockBuffer;
} BoxCanvas;

void BoxCanvas_Create(BoxCanvas *canvas, uint8_t X, uint8_t Y, uint8_t W, uint8_t H);
void BoxCanvas_Destroy(BoxCanvas *canvas);
void BoxCanvas_Render(BoxCanvas *canvas);
void BoxCanvas_Box(BoxCanvas *canvas, uint8_t X, uint8_t Y, uint8_t W, uint8_t H, BoxDrawStyle style);

#endif // _BOX_CANVAS_H_
