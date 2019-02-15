// EASY-CONSOLE: Make games in the command-line

// Copyright (C) 2019 Sidali Mostefaoui

// For details see: README.md
// For LICENSE see: LICENSE.md

#ifndef EASY_CONSOLE_H
#define EASY_CONSOLE_H


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <wchar.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#ifndef WINVER
#define WINVER _WIN32_WINNT
#endif


// COLORS
#define FOREGROUND_BLACK 0x00
#define FOREGROUND_BLUE_DARK 0x01
#define FOREGROUND_GREEN_DARK 0x02
#define FOREGROUND_CYAN_DARK 0x03
#define FOREGROUND_RED_DARK 0x04
#define FOREGROUND_MAGENTA_DARK 0x05
#define FOREGROUND_YELLOW_DARK 0x06
#define FOREGROUND_GREY_LIGHT 0x07
#define FOREGROUND_GREY_DARK 0x08
#define FOREGROUND_BLUE_LIGHT 0x09
#define FOREGROUND_GREEN_LIGHT 0x0A
#define FOREGROUND_CYAN_LIGHT 0x0B
#define FOREGROUND_RED_LIGHT 0x0C
#define FOREGROUND_MAGENTA_LIGHT 0x0D
#define FOREGROUND_YELLOW_LIGHT 0x0E
#define FOREGROUND_WHITE 0x0F

#define BACKGROUND_BLACK 0x00
#define BACKGROUND_BLUE_DARK 0x10
#define BACKGROUND_GREEN_DARK 0x20
#define BACKGROUND_CYAN_DARK 0x30
#define BACKGROUND_RED_DARK 0x40
#define BACKGROUND_MAGENTA_DARK 0x50
#define BACKGROUND_YELLOW_DARK 0x60
#define BACKGROUND_GREY_LIGHT 0x70
#define BACKGROUND_GREY_DARK 0x80
#define BACKGROUND_BLUE_LIGHT 0x90
#define BACKGROUND_GREEN_LIGHT 0xA0
#define BACKGROUND_CYAN_LIGHT 0xB0
#define BACKGROUND_RED_LIGHT 0xC0
#define BACKGROUND_MAGENTA_LIGHT 0xD0
#define BACKGROUND_YELLOW_LIGHT 0xE0
#define BACKGROUND_WHITE 0xF0

#define CHAR_RECT (char)0xdb


typedef struct _Console{
    CHAR_INFO* buffer;
    int width;
    int height;
}_Console;
typedef void* Console;


/***** Interal wrapper functions around Win32 API *****/
/***** DON'T CALL THESE FUNCTIONS DIRECTLY !!!! *****/
void _show_error_message(char* label, char* msg) {
    MessageBoxA(GetConsoleWindow(), msg, label, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
}

void _set_console_size(int width, int height) {
    HANDLE hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    // shows an error box and exits if size is larger than max
    COORD maxConsoleSize = GetLargestConsoleWindowSize(hStdOutput);
    if(width >= maxConsoleSize.X || height >= maxConsoleSize.Y) {
        char msg[150];
        char formatString[] = "Invalid function arguments:\n"
                              "console size set by function console_init() is too large.\n"
                              "Maximum is: %u x %u";
        snprintf(msg, 150, formatString, maxConsoleSize.X, maxConsoleSize.Y);
        _show_error_message("EASY_CONSOLE API error!", msg);
        exit(1);
    }

    // shows an error box and exits if size is smaller than min
    COORD minConsoleSize = {20, 20};
    if(width < minConsoleSize.X || minConsoleSize.Y < 20) {
        char msg[150];
        char formatString[] = "Invalid function arguments:\n"
                              "console size set by function console_init() is too small.\n"
                              "Minimum is: %u x %u";
        snprintf(msg, 150, formatString, minConsoleSize.X, minConsoleSize.Y);
        _show_error_message("EASY_CONSOLE API error!", msg);
        exit(1);
    }

    COORD largeBufferSize = {300, 300};
    SetConsoleScreenBufferSize(hStdOutput, largeBufferSize);

    SMALL_RECT windowSize = {0, 0, (short)(width - 1), (short)(height - 1)};
    SetConsoleWindowInfo(hStdOutput, TRUE, &windowSize);

    COORD bufferSize = {(short)width, (short)height};
    SetConsoleScreenBufferSize(hStdOutput, bufferSize);
}

void _set_console_font(wchar_t* font_name, short font_size) {
    CONSOLE_FONT_INFOEX fontInfo = {0};
    fontInfo.cbSize       = sizeof(fontInfo);
    fontInfo.FontWeight   = FW_NORMAL;
    fontInfo.dwFontSize.Y = font_size;
    wcscpy(fontInfo.FaceName, font_name);

    if(!SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), 0, &fontInfo)) {
        char msg[50];
        char formatString[] = "SetCurrentConsoleFontEx() error code: %u";
        snprintf(msg, 100, formatString, GetLastError());
        _show_error_message("WinAPI error!", msg);
        exit(1);
    };
}

// disables window resize & maximize
void _disable_resize_and_maximize() {
    HMENU sysMenu = GetSystemMenu(GetConsoleWindow(), false);
    DeleteMenu(sysMenu, SC_SIZE, MF_BYCOMMAND);
    DeleteMenu(sysMenu, SC_MAXIMIZE, MF_BYCOMMAND);
}

/* disables built-in mouse and keyboard input:
     * Mouse left-click text selection
     * Keyboard CTRL-M text selection
     * CTRL_C program exit
     */
void _disable_input() {
    long disabledModeFlags = ~ENABLE_QUICK_EDIT_MODE & ~ENABLE_PROCESSED_INPUT & ~ENABLE_LINE_INPUT;
    DWORD prevMode;
    GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &prevMode);
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), prevMode & disabledModeFlags);
}

// disables console cursor
void _disable_cursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}


/***** EASY_CONSOLE public API *****/

void console_set_title(char* title) {
    SetConsoleTitleA(title);
}

Console console_init(int width, int height) {
    // Exit program if this function is called more than once
    static bool EASY_CONSOLE_INITIALIZED = false;

    if(EASY_CONSOLE_INITIALIZED) {
        char msg[] = "function console_init() can be called one time only.";
        _show_error_message("EASY_CONSOLE API error!", msg);
        exit(1);
    }

    EASY_CONSOLE_INITIALIZED = true;

    _Console* _console = malloc(sizeof(_Console));
    if(_console == NULL) exit(1);

    _console->width   = width;
    _console->height  = height;
    _console->buffer  = malloc(width * height* sizeof(CHAR_INFO));

    _disable_resize_and_maximize();
    _disable_input();
    _disable_cursor();
    _set_console_size(width, height);
    _set_console_font(L"Consolas", 16);

    console_set_title("EASY-CONSOLE");
    return (Console)_console;
}

void console_update(Console console) {
    _Console* _console = (_Console*)console;
    HANDLE hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD bufferSize = {(short)_console->width, (short)_console->height};
    COORD bufferCoord = {0, 0}; // top left corner (x = 0 & y = 0)
    SMALL_RECT writeRegion = {0, 0, (short)(_console->width - 1), (short)(_console->height - 1)};

    WriteConsoleOutput(hStdOutput, _console->buffer, bufferSize, bufferCoord, &writeRegion);
}

// draw character at x,y coordinates
void console_draw_char(Console console, int x, int y, char c, unsigned short color) {
    _Console* _console = (_Console*)console;
    // Shows ErrorBox and exits if x & y are invalid
    if(x >= _console->width || y >= _console->height) {
        char msg[] = "ERROR: invalid function args!\n x or y argument of function: console_draw_char()\n"
                     "is outside the console buffer";
        _show_error_message("EASY_CONSOLE API error!", msg);
        exit(1);
    }

    int index = (y * _console->width) + x; // convert 2D Array coordinates to 1D Array index
    _console->buffer[index].Char.AsciiChar = c;
    _console->buffer[index].Attributes = color;
}

// draw string at x,y coordinates
void console_draw_string(Console console, int x, int y, char* string, unsigned short color) {
    _Console* _console = (_Console*)console;

    int index = (y * _console->width) + x; // convert 2D Array coordinates to 1D Array index
    for(int i = 0; i < (int)strlen(string); i++) {
        _console->buffer[index + i].Char.AsciiChar = string[i];
        _console->buffer[index + i].Attributes = color;
    }
}

// fills the console with the desired color
void console_fill(Console console, unsigned short color) {
    _Console* _console = (_Console*)console;
    for(int i = 0; i < _console->width; i++) {
        for(int j = 0; j < _console->height; j++) {
            console_draw_char(console, i, j, CHAR_RECT, color);
        }
    }
}

bool key_is_pressed(unsigned short key_code) {
    /* returns the most significant bit of key_state
     * more info at:
     * https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-getasynckeystate */
    short key_state = GetAsyncKeyState(key_code);
    return (bool)(key_state & 0x8000);
}


#endif
