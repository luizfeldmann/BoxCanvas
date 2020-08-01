# BoxCanvas

Utility to draw box characters on terminal. Also creates "windows" inside the terminal.

### File browser dialog box

```c
uint8_t ShowFileExplorer(char *out_filename, const char* filterextension, const char* title, uint8_t fileMustExist, DialogBoxStyle styleSelector);
```

![screenshot](screenshot/file.png)

### Message dialog box

```c
uint8_t ShowMessageBox(const char *title, const char *text, uint8_t numOptions, char* options[], DialogBoxStyle styleSelector);
```
![screenshot](screenshot/msgbox.png)

### Slider dialog box

```c
float ShowSliderBox(const char *title, const char *text, float minValue, float curValue, float maxValue, float increment, DialogBoxStyle styleSelector);
```

![screenshot](screenshot/slider.png)
