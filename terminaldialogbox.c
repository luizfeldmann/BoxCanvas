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

#include "terminaldialogbox.h"
#include "../BrailleCanvas/BrailleCanvas/terminal.h" // -- get this file (and the matching .c file too) in the repo "BrailleCanvas" at: https://github.com/luizfeldmann/BrailleCanvas
#include "port_kbhit.h"         // portable kbhit and getch functions
#include "tinydir.h"            // get this file at https://github.com/cxong/tinydir
#include "boxcanvas.h"          // draws boxing using ascii/unicode characters
#include <stdio.h>              // printf, fwrite etc
#include <ctype.h>              // upper, lower, numerical and alphabetical types

struct dialogBoxStyle
{
    // box
    ConsoleStyleText        BoxText;
    ConsoleStyleBackground  BoxBack;
    // title
    ConsoleStyleText        TitleText;
    ConsoleStyleBackground  TitleBack;
    // message
    ConsoleStyleText        ContentText;
    ConsoleStyleBackground  ContentBack;
    // options
    ConsoleStyleText        OptionsText_Active;
    ConsoleStyleBackground  OptionsBack_Active;
    ConsoleStyleText        OptionsText_Normal;
    ConsoleStyleBackground  OptionsBack_Normal;
};

struct dialogBoxStyle stylePalette[] = {
                                                    /* 0 */
    {   .BoxText            = CONSOLE_STYLE_TEXT_WHITE,     .BoxBack            = CONSOLE_STYLE_BACKGROUND_GREY,
        .TitleText          = CONSOLE_STYLE_TEXT_BLACK,     .TitleBack          = CONSOLE_STYLE_BACKGROUND_WHITE,
        .ContentText        = CONSOLE_STYLE_TEXT_WHITE,     .ContentBack        = CONSOLE_STYLE_BACKGROUND_GREY,
        .OptionsText_Active = CONSOLE_STYLE_TEXT_BLACK,     .OptionsBack_Active = CONSOLE_STYLE_BACKGROUND_WHITE,
        .OptionsText_Normal = CONSOLE_STYLE_TEXT_WHITE,     .OptionsBack_Normal = CONSOLE_STYLE_BACKGROUND_GREY},
                                                    /* 1 */
    {   .BoxText            = CONSOLE_STYLE_TEXT_WHITE,     .BoxBack            = CONSOLE_STYLE_BACKGROUND_BLUE,
        .TitleText          = CONSOLE_STYLE_TEXT_BLACK,     .TitleBack          = CONSOLE_STYLE_BACKGROUND_WHITE,
        .ContentText        = CONSOLE_STYLE_TEXT_WHITE,     .ContentBack        = CONSOLE_STYLE_BACKGROUND_BLUE,
        .OptionsText_Active = CONSOLE_STYLE_TEXT_BLACK,     .OptionsBack_Active = CONSOLE_STYLE_BACKGROUND_WHITE,
        .OptionsText_Normal = CONSOLE_STYLE_TEXT_WHITE,     .OptionsBack_Normal = CONSOLE_STYLE_BACKGROUND_BLUE},
                                                    /* 2 */
    {   .BoxText            = CONSOLE_STYLE_TEXT_WHITE,     .BoxBack            = CONSOLE_STYLE_BACKGROUND_RED,
        .TitleText          = CONSOLE_STYLE_TEXT_RED,       .TitleBack          = CONSOLE_STYLE_BACKGROUND_WHITE,
        .ContentText        = CONSOLE_STYLE_TEXT_WHITE,     .ContentBack        = CONSOLE_STYLE_BACKGROUND_RED,
        .OptionsText_Active = CONSOLE_STYLE_TEXT_BLACK,     .OptionsBack_Active = CONSOLE_STYLE_BACKGROUND_WHITE,
        .OptionsText_Normal = CONSOLE_STYLE_TEXT_WHITE,     .OptionsBack_Normal = CONSOLE_STYLE_BACKGROUND_RED}
};

void PrintWidth(uint8_t width, uint8_t centered, const char* text)
{
    //if (width % 2 == 1) // if it's odd
    //    width--; // make it even

    uint8_t len = strlen(text);
    if (len > width)
        printf("%.*s..%s", width/2 -1, text, &text[strlen(text) - width/2 + 1] );
    else if (len == width)
        printf("%s", text);
    else
    {
        uint8_t leftComplete = centered ? (width-len)/2 : 0;
        uint8_t rightComplete = width - len - leftComplete;

        for (;leftComplete>0;leftComplete--)
            printf(" ");

        printf("%s", text);

        for (;rightComplete>0;rightComplete--)
            printf(" ");
    }
}

float ShowSliderBox(const char *title, const char *text, float minValue, float curValue, float maxValue, float increment, DialogBoxStyle styleSelector)
{
    #if (defined(unix) || defined(__unix__) || defined(__unix))
        uint8_t bSliderboxUseUTF8 = 1;
    #elif (defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__))
        uint8_t bSliderboxUseUTF8 = (GetConsoleOutputCP() == CP_UTF8);
    #endif

    // get the dimensions
    uint8_t termW, termH;
    Terminal_GetSize(&termW, &termH);

    uint8_t dialogHeight = 7; // top border (title) / text / values / slider / blank / bottom border
    uint8_t dialogWidth = min(max(max(strlen(title),strlen(text)) + 2, termW/2), termW); // left border / content / right border

    // center on terminal
    uint8_t dialogX = (termW - dialogWidth)/2;
    uint8_t dialogY = (termH - dialogHeight)/2;

    // get the style from the palette
    struct dialogBoxStyle* style = &stylePalette[styleSelector];

    // create and configure canvas
    BoxCanvas canvas;
    BoxCanvas_Create(&canvas, dialogX, dialogY, dialogWidth, dialogHeight);
    canvas.BackgroundStyle  = style->BoxBack;
    canvas.FillStyle        = style->BoxText;

    BoxCanvas_Box(&canvas, 0, 0, dialogWidth, dialogHeight, BOX_STYLE_STRONG | BOX_STYLE_SHADOW);   // window

    // draw to the screen
    Terminal_Lock();
    Terminal_SaveCursorPosition();

    // draw the form
    BoxCanvas_Render(&canvas);
    BoxCanvas_Destroy(&canvas);

    // print title
    Terminal_SetCursorPosition(dialogX+1,dialogY);
    Terminal_SetStyle(style->TitleText, style->TitleBack);
    PrintWidth(dialogWidth-2,1,title);

    // print message
    Terminal_SetCursorPosition(dialogX+1,dialogY+2);
    Terminal_SetStyle(style->ContentText, style->ContentBack);
    PrintWidth(dialogWidth-2,1,text);

    // print the slider-bar
    char slider[1000];

    DRAWSLIDER:

    // min max values
    slider[0] = '\0';

    size_t formatWidth = sprintf(slider, "%4.2f", minValue); // min value

    for (uint8_t i = 0; i < (dialogWidth-4 - formatWidth*2); i++)
        strcat(slider, " ");                                // spaces

    sprintf(slider, "%.*s%4.2f", (int)min(strlen(slider), sizeof(slider)-formatWidth-1),slider, maxValue);

    Terminal_SetStyle(style->OptionsText_Normal, style->OptionsBack_Normal);
    Terminal_SetCursorPosition(dialogX+2,dialogY+3);
    printf("%s", slider);

    // bar
    uint8_t sliderLength = dialogWidth - 6;
    slider[0] = '\0';
    sprintf(slider, bSliderboxUseUTF8 ? "\xE2\x94\x9C" : "|"); // starter

    uint8_t markerPosition = (uint8_t) (sliderLength * (curValue - minValue) / (maxValue - minValue));

    for (uint8_t i = 0; i < sliderLength; i++)
        if (i == markerPosition)
            strcat(slider, bSliderboxUseUTF8 ? "\xE2\x95\x91" : "X");
        else
            strcat(slider, bSliderboxUseUTF8 ? "\xE2\x94\x80" : "-");

    strcat(slider, bSliderboxUseUTF8 ? "\xE2\x94\xA4" : "|");

    Terminal_SetCursorPosition(dialogX+2,dialogY+4);
    Terminal_SetStyle(style->OptionsText_Active, style->OptionsBack_Active);
    printf("%s", slider);

    // current value
    slider[0] = '\0';
    for (uint8_t i = 0; i < dialogWidth-2; i++)
        if (i == markerPosition + 2 - ((markerPosition > sliderLength/2) ? formatWidth-1 : 0) )
            i = sprintf(slider, "%.*s%4.2f", (int)min(strlen(slider), sizeof(slider)-4-1), slider, curValue);
        else
            strcat(slider, " ");

    Terminal_SetStyle(style->OptionsText_Normal, style->OptionsBack_Normal);
    Terminal_SetCursorPosition(dialogX+1,dialogY+5);
    printf("%s", slider);

    for (;;)
    {
        char kb =  getchNavigation();

        if (kb == KEY_ENTER || kb == KEY_RETURN)
            break;

        if (kb == KEY_ARROW_LEFT && curValue - increment >= minValue)
        {
            curValue -= increment;
            goto DRAWSLIDER;
        }

        else if (kb == KEY_ARROW_RIGHT && curValue + increment <= maxValue )
        {
            curValue += increment;
            goto DRAWSLIDER;
        }
    }

    Terminal_RestoreCursorSavedPosition();
    Terminal_Unlock();

    return curValue;
}

uint8_t ShowMessageBox(const char *title, const char *text, uint8_t numOptions, char* options[], DialogBoxStyle styleSelector)
{
    if (Terminal_Lock() != 0) // cannot let anything else mess the screen while the dialog is on
        return 0;

    uint8_t termW, termH;
    Terminal_GetSize(&termW, &termH);
    Terminal_SaveCursorPosition();

    uint8_t dialogHeight = numOptions + 4; // top border (title) / text / divider / option1...optionN / bottom border
    uint8_t dialogWidth = max(strlen(title),strlen(text));
    for (uint8_t optIndex = 0; optIndex < numOptions; optIndex++)
        dialogWidth = max(dialogWidth, strlen(options[optIndex]));
    dialogWidth += 2; // left border / content / right border

    // center on terminal
    uint8_t dialogX = (termW - dialogWidth)/2;
    uint8_t dialogY = (termH - dialogHeight)/2;

    struct dialogBoxStyle* style = &stylePalette[styleSelector]; // get the style from the palette

    // create and configure canvas
    BoxCanvas canvas;
    BoxCanvas_Create(&canvas, dialogX, dialogY, dialogWidth, dialogHeight);

    canvas.BackgroundStyle  = style->BoxBack;
    canvas.FillStyle        = style->BoxText;

    BoxCanvas_Box(&canvas, 0, 0, dialogWidth, dialogHeight, BOX_STYLE_STRONG | BOX_STYLE_SHADOW);   // the big box
    BoxCanvas_Box(&canvas, 0, 0, dialogWidth, 3,            BOX_STYLE_WEAK   | BOX_STYLE_NOSHADOW); // the small box
    BoxCanvas_Render(&canvas);

    // print title
    Terminal_SetCursorPosition(dialogX+1,dialogY);
    Terminal_SetStyle(style->TitleText, style->TitleBack);
    PrintWidth(dialogWidth-2,1,title);

    // print message
    Terminal_SetCursorPosition(dialogX+1,dialogY+1);
    Terminal_SetStyle(style->ContentText, style->ContentBack);
    PrintWidth(dialogWidth-2,1,text);

    // print the options
    uint8_t selectedOption = 0;

    DRAWOPTIONS:
    for (uint8_t opt = 0; opt < numOptions; opt++)
    {
        Terminal_SetCursorPosition(dialogX+1,dialogY+3+opt);
        if (opt == selectedOption)
            Terminal_SetStyle(style->OptionsText_Active, style->OptionsBack_Active);
        else
            Terminal_SetStyle(style->OptionsText_Normal, style->OptionsBack_Normal);

        PrintWidth(dialogWidth-2,1,options[opt]);
    }

    READKB:
    switch (getchNavigation())
    {
        case KEY_ARROW_UP:
            if (selectedOption>0)
                selectedOption--;
            goto DRAWOPTIONS;
        break;

        case KEY_ARROW_DOWN:
            if (selectedOption<numOptions-1)
                selectedOption++;
            goto DRAWOPTIONS;
        break;

        case KEY_ENTER:
        case KEY_RETURN: // on <enter> just exit the loop and follow along ...
            break;

        default: goto READKB; break; // unknown key will repeat the loop
    }

    BoxCanvas_Destroy(&canvas);

    Terminal_RestoreCursorSavedPosition();
    Terminal_Unlock();

    return selectedOption;
}


uint8_t ShowFileExplorer(char *out_filename, const char* filterextension, const char* title, uint8_t fileMustExist, DialogBoxStyle styleSelector)
{
    if (Terminal_Lock() != 0)
        return 0;

    uint8_t termW, termH;
    Terminal_GetSize(&termW, &termH);
    Terminal_SaveCursorPosition();

    uint16_t diagW = 3*termW/4;
    uint16_t diagH = 3*termH/4;
    uint16_t diagX = termW/8;
    uint16_t diagY = termH/8;

    struct dialogBoxStyle* style = &stylePalette[styleSelector];

    // dimensions of browser
    uint8_t numRows = diagH-7;
    uint8_t numCols = 4;
    uint8_t widCols = (diagW-2)/numCols;
    uint8_t clearBack = 1;

    // draw the form
    BoxCanvas canvas;
    BoxCanvas_Create(&canvas, diagX, diagY, diagW, diagH);
    canvas.BackgroundStyle  = style->BoxBack;
    canvas.FillStyle        = style->BoxText;
    BoxCanvas_Box(&canvas, 0, 0, diagW, diagH, BOX_STYLE_STRONG | BOX_STYLE_SHADOW ); // outside border
    BoxCanvas_Box(&canvas, 0, 0, diagW, 3,     BOX_STYLE_WEAK   | BOX_STYLE_NOSHADOW); // box for "folder name"
    BoxCanvas_Box(&canvas, 0, 0, diagW, 5,     BOX_STYLE_STRONG | BOX_STYLE_NOSHADOW); // box for "file name"
    BoxCanvas_Render(&canvas);
    BoxCanvas_Destroy(&canvas);

    // draw the title
    Terminal_SetCursorPosition(diagX+diagW/4,diagY);
    Terminal_SetStyle(style->TitleText, style->TitleBack);
    PrintWidth(diagW/2,1,title);

    uint8_t currentPage = 0;
    int16_t SelectionIndex = 0; // must be signed because value -1 is used to indicate "nothing selected"
    char folderpath[_TINYDIR_PATH_MAX] = ".";
    char filename[_TINYDIR_PATH_MAX] = "";

    tinydir_dir dir;
    tinydir_file file;

    if (tinydir_open_sorted(&dir, folderpath) == -1)
        printf("Tinydir error");

    char kb;
    for (;;) // keep in this loop reading keyboard
    {
        FORCE_REDRAW:

        // clear old files from the file view
        if (clearBack) // we don't do it every cycle because it's time consuming and makes the screen flicker .. it must be requested when, for instance, a new directory is browsed or when the page number changes (whenever the number of items in screen may change)
        {
            Terminal_SetStyle(style->BoxText, style->BoxBack);
            Terminal_ClearArea(diagX+1, diagY+5, diagW-2, numRows+1); // clear all rows + the page counter
            clearBack = 0;
        }

        // draw the folder path
        Terminal_SetStyle(style->TitleText, style->TitleBack);
        Terminal_SetCursorPosition(diagX+1,diagY+1);
        printf("Directory: ");
        Terminal_SetStyle(style->ContentText, style->ContentBack);
        PrintWidth(diagW-14,0,folderpath);

        // draw filename
        Terminal_SetStyle(style->TitleText, style->TitleBack);
        Terminal_SetCursorPosition(diagX+1,diagY+3);
        printf("File name: ");
        Terminal_SetStyle(style->ContentText, style->ContentBack);
        PrintWidth(diagW-14,0,filename);

        uint8_t numPages = (dir.n_files / (numRows * numCols)); // count how many pages are required to display all items in this directory
        if (dir.n_files % (numRows * numCols) != 0)
            numPages++;

        if (numPages > 1) // if more than 1, then show a status bar indicating that...
        {
            char pageDescriptor[100];
            sprintf(pageDescriptor, "Page %u/%d", currentPage+1, numPages);

            Terminal_SetCursorPosition(diagX+1, diagY+diagH-2);
            Terminal_SetStyle(style->TitleText, style->TitleBack);
            PrintWidth(diagW-2,1,pageDescriptor); // centered
        }

        // draw browser
        for (size_t index = 0; index < dir.n_files; index++)
        {
            uint8_t rowTotal = index / numCols;
            uint8_t col = index % numCols;
            uint8_t page = rowTotal / numRows;
            uint8_t row = rowTotal % numRows;
            uint8_t newPage = (SelectionIndex>=0) ? (SelectionIndex / numCols) / numRows : currentPage; // if nothing selected .. dont change the page

            if (newPage != currentPage) // see if we switched pages ...
            {
                clearBack = 1; // if page changed we must clear the back, because if we didn't and the number of files in the new page is smaller than the in last, thoses files would remain "ghost-printed"
                currentPage = newPage;
                goto FORCE_REDRAW; // .. because if we did, we must "clear back" to prevent ghost-items showing up from the previous page
            }

            if (page != currentPage)
                continue; // only display items in the current page

            Terminal_SetCursorPosition(diagX+1 + col*widCols, diagY+5+row);

            if (tinydir_readfile_n(&dir, &file, index) == -1)
                printf("Tinydir error");

            if (index == SelectionIndex)
                Terminal_SetStyle(style->OptionsText_Active, style->OptionsBack_Active);
            else
                Terminal_SetStyle(style->OptionsText_Normal, style->OptionsBack_Normal);

            char displayName[_TINYDIR_PATH_MAX];

            if (file.is_dir)
                sprintf(displayName, "[%s]", file.name);
            else
                sprintf(displayName, "%s", file.name);

            PrintWidth(widCols-2, 0, displayName);
        }

        // put the cursor in the "filename" field
        Terminal_SetCursorPosition(min(diagX+12+strlen(filename), diagX+diagW-2), diagY+3); // make sure the cursor does not end up outside the dialog in case the filename is really long

        // run keyboard interactivity
        REGET:
        kb = getchNavigation();

        int16_t newSel = SelectionIndex; // holds the candidate for new selected item
        switch (kb)
        {
            case KEY_ARROW_UP:
                newSel -= numCols;
                break;

            case KEY_ARROW_DOWN:
                newSel += numCols;
                //if (newSel > dir.n_files && currentPage == numPages - 1) // if last page, possibly not enough files to switch page using arrow, so this case is needed to make sure last page is accessible
                //    newSel = dir.n_files -1;
                break;
            case KEY_ARROW_LEFT: newSel--; break;
            case KEY_ARROW_RIGHT: newSel++; break;

            case KEY_BACKSPACE: // backspace
                if (strlen(filename)>0)
                    if (sprintf(filename, "%.*s", (int)strlen(filename) - 1, filename)) // must cast strlen from size_t to avoid warning -Wformat
                        newSel = -1;
                break;

            case KEY_ESC: // esc key
                    goto CLOSE;
                break;

            case KEY_ENTER:
            case KEY_RETURN: // enter key to navigate or accept file
                //if (fileMustExist)
                //{
                //
                //}
                if (SelectionIndex>=0) // something is selected
                {
                    if (tinydir_readfile_n(&dir, &file, SelectionIndex) != -1) // read success
                    {
                        if (file.is_dir) // browse new directory
                        {
                            if (tinydir_open_subdir_n(&dir, SelectionIndex) != -1) // open success
                            {
                                strcpy(folderpath, dir.path); // update folderpath
                                strcpy(filename, ""); // clear filename
                                SelectionIndex = -1; // de-select item on new folder
                                newSel = SelectionIndex;
                                clearBack = 1;
                            }
                            else
                            {
                                // failed to open ... may cause error - discard current session and reopen
                                tinydir_close(&dir);
                                tinydir_open_sorted(&dir, folderpath);
                            }
                        }
                        else // try accept the selected file
                        {
                            if (strcmp(file.extension, filterextension) == 0) // accept the file if the extension matches
                            {
                                strcpy(out_filename, file.path);
                                goto CLOSE;
                            }
                        }
                    }
                }
                else // nothing is selected ...
                {
                    if (!fileMustExist) // ... but it does not have to be
                    {
                        char *ptrDot;
                        char *ext = &filename[0]; // we begin with the full file name

                        while ((ptrDot = strstr(ext, ".")) > 0) // while there is dot in the name...
                            ext = ptrDot+1; // we move past it ... until there is no more dots

                        if (strcmp(filterextension, ext) == 0 || strlen(filterextension) == 0)
                        {
                            sprintf(out_filename, "%s/%s", dir.path, filename);
                            goto CLOSE;
                        }

                    }
                }

                break;

            default:
                if (isalnum(kb) || kb == '.' || kb == '_' || kb == ' ')
                {
                    if (sprintf(filename, "%.*s%c", (int)min(strlen(filename), _TINYDIR_FILENAME_MAX-1),filename, kb)) // GCC WARNING -Wformat-overflow: filename + c may be bigger than sizeof(filename)
                        newSel = -1;               // ^ must cast from size_t to int to avoid -Wformat warning
                }
                else goto REGET; // this character serves no purpose
            break;
        }

        if (newSel == -1) // user typed a filename
        {
            SelectionIndex = -1; // deselect current file

            for (size_t index = 0; index < dir.n_files; index++)
                if (tinydir_readfile_n(&dir, &file, index) != -1)
                    if (strcmp(file.name, filename) == 0 && strlen(file.name)>0)
                    {
                        SelectionIndex = index; // select the file with the right name if there is any ...
                        break;
                    }
        }

        if (newSel >=0 && newSel < dir.n_files && newSel != SelectionIndex)
        {
            SelectionIndex = newSel;

            tinydir_readfile_n(&dir, &file, SelectionIndex);
            strcpy(filename, file.name);
        }
    }

    CLOSE:
    Terminal_RestoreCursorSavedPosition();
    Terminal_Unlock();
    tinydir_close(&dir);

    return (kb == KEY_ESC) ? 0 : 1; // if we close because of ESC key, then return 0 (failure)
}
