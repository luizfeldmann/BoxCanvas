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

#include "boxcanvas.h"
#include "terminaldialogbox.h"
#include <stdio.h>

void demo_Boxes()
{
    BoxCanvas canvas;
    BoxCanvas_Create(&canvas, 0, 0, 0, 0); // o width or height means "fullscreen"
    canvas.FillStyle = CONSOLE_STYLE_TEXT_WHITE;
    canvas.BackgroundStyle = CONSOLE_STYLE_BACKGROUND_GREY;

    BoxCanvas_Box(&canvas, 10, 10, 10, 10, BOX_STYLE_STRONG);
    BoxCanvas_Box(&canvas, 25, 10, 10, 10, BOX_STYLE_WEAK);
    BoxCanvas_Box(&canvas, 40, 10, 10, 10, BOX_STYLE_SHADOW);

    BoxCanvas_Box(&canvas, 10, 25, 50, 10, BOX_STYLE_STRONG | BOX_STYLE_SHADOW);
    BoxCanvas_Box(&canvas, 10, 25, 25, 8, BOX_STYLE_WEAK);
    BoxCanvas_Box(&canvas, 30, 27, 10, 4, BOX_STYLE_SHADOW | BOX_STYLE_FILL);

    BoxCanvas_Render(&canvas);

    fflush(stdin);
    getc(stdin);

    Terminal_SetStyle(CONSOLE_STYLE_TEXT_WHITE, CONSOLE_STYLE_BACKGROUND_BLACK);
}

void demo_Msg()
{
    int opt = ShowMessageBox("TITLE HERE", "This is the text inside the message box.", 3, (char *[]){"OPTION 0", "OPTION 1", "OPTION 2"}, DIALOG_BOX_STYLE_RED);

    Terminal_SetStyle(CONSOLE_STYLE_TEXT_WHITE, CONSOLE_STYLE_BACKGROUND_BLACK);
    Terminal_Clear();
    Terminal_RestoreCursorSavedPosition();

    printf("\nYou chose option %d\n", opt);
}

void demo_Explore()
{
    char path[256] = "";
    int result = ShowFileExplorer(path, "c", "Please pick a .c file", 1, DIALOG_BOX_STYLE_BLUE);

    Terminal_SetStyle(CONSOLE_STYLE_TEXT_WHITE, CONSOLE_STYLE_BACKGROUND_BLACK);
    Terminal_Clear();
    Terminal_RestoreCursorSavedPosition();

    if (!result)
        printf("\nYou did not pick a file!\n");
    else
        printf("\nPath: %s\n", path);
}

void demo_Slider()
{
    float def = 5.0;
    float min = 1.0;
    float max = 10.0;
    float step = 0.5;

    float val = ShowSliderBox("TITLE HERE", "Use arrows to move slider below", min, def, max, step, DIALOG_BOX_STYLE_GREY);

    Terminal_Clear();
    Terminal_RestoreCursorSavedPosition();
    printf("\nThe value is %f\n", val);
}

int main(int argc, char** argv)
{
    printf("1 - demo boxes\n");
    printf("2 - demo message box\n");
    printf("3 - demo file explorer\n");
    printf("4 - demo slider bar\n");
    printf("\n>> ");

    fflush(stdin);
    char opt = getc(stdin);

    Terminal_SaveCursorPosition();

    switch (opt)
    {
        case '1':
            demo_Boxes();
        break;

        case '2':
            demo_Msg();
        break;

        case '3':
            demo_Explore();
        break;

        case '4':
            demo_Slider();
        break;

        default: break;
    }
}
