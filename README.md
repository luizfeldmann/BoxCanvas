# BoxCanvas

Utility to draw box characters on terminal. Also creates "windows" inside the terminal.

### File browser dialog box

```c
uint8_t ShowFileExplorer(char *out_filename, const char* filterextension, const char* title, uint8_t fileMustExist, DialogBoxStyle styleSelector);
```

![screenshot_file](screenshot/file.PNG?raw=true "File browser")

### Message dialog box

```c
uint8_t ShowMessageBox(const char *title, const char *text, uint8_t numOptions, char* options[], DialogBoxStyle styleSelector);
```
![screenshot_msg](screenshot/msgbox.PNG?raw=true "Message Box")

### Slider dialog box

```c
float ShowSliderBox(const char *title, const char *text, float minValue, float curValue, float maxValue, float increment, DialogBoxStyle styleSelector);
```

![screenshot_slide](screenshot/slider.PNG?raw=true "Slider")
