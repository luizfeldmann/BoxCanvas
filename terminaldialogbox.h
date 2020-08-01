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

#ifndef _TERMINAL_DIALOG_BOX_H_
#define _TERMINAL_DIALOG_BOX_H_

#include <stdint.h>

typedef enum {
    DIALOG_BOX_STYLE_GREY =  0,
    DIALOG_BOX_STYLE_BLUE =  1,
    DIALOG_BOX_STYLE_RED   = 2,
} DialogBoxStyle;

uint8_t ShowMessageBox(const char *title, const char *text, uint8_t numOptions, char* options[], DialogBoxStyle styleSelector);
uint8_t ShowFileExplorer(char *out_filename, const char* filterextension, const char* title, uint8_t fileMustExist, DialogBoxStyle styleSelector);
float ShowSliderBox(const char *title, const char *text, float minValue, float curValue, float maxValue, float increment, DialogBoxStyle styleSelector);

#endif // _TERMINAL_DIALOG_BOX_H_
