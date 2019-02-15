## What is Easy-Console ?

Easy-Console is a small **Windows** C library to make games in the **command-line**. It is aimed at
beginners who want to code cool stuff fast and people who want a simple and fast way to try out
different algorithms(ray-tracing, path-finding, maze generation..etc) without the overhead of a
large library.

## Getting started

### Install
* Download "easy_console.h" and put it in your working folder.
* Include it in your source file.
    ```C
    #include "easy_console.h"
    ```
### Compile

#### From the command-line:
 Compile with **MinGW-w64** or **Visual C++** and link with user32 static library.
* MinGW-w64:
    ```
    $ gcc my_program.c -luser32
    ```
* Visual C++:
    ```
    $ cl my_program.c user32.lib<z
    ```
#### With an IDE:
* Visual Studio: add the following line at the top of your code to link with user32 library.
    ```C
    #pragma comment(lib, "User32.lib")
    ```
* Other IDEs: find **linker settings** in your IDE and add **libuser32.a** or add **-luser32** to
  the **compiler options**.
  
### Program Example
```C
int main(void) {
    Console console = console_init(120, 40); // init library & create console window
    
    // repeat until "ESCAPE" is pressed
    while(!key_is_pressed(VK_ESCAPE)) {
        console_fill(console, FOREGROUND_RED_LIGHT | BACKGROUND_RED_LIGHT);
        
        // draw "EASY-CONSOLE EXAMPLE" at pos 0,0  with white foreground & light red background
        console_draw_string(console, 0, 0, "EASY-CONSOLE EXAMPLE",
                            FOREGROUND_WHITE | BACKGROUND_RED_LIGHT);
                            
        // draw 'H' at pos 0,10 with black foreground & light green background
        console_draw_char(console, 0, 10, 'H', FOREGROUND_BLACK | BACKGROUND_GREEN_LIGHT);
        
        // draw 5 char wide horizontal line starting from middle of console screen
        for(int i = 120/2; i < (120/2) + 5; i++)
            console_draw_char(console, i, 40/2, CHAR_RECT,
                              FOREGROUND_WHITE | BACKGROUND_WHITE);
            
        console_update(console); // update console screen
    }
    
    return 0; 
}
```
For a full game example see:
[Easy-Shooter](https://github.com/sidmostefaoui/Easy-Console/blob/master/easy_shooter.c).
    
## API

### Initialization, Update and Setup:
* Initialize the library and create a console window.
    ```C
    Console console_init(int width, int height)
    ```
* Update the console screen(should be called every frame after drawing).
    ```C
    void console_update(Console console)
    ```
* Set the console window title(default title is "EASY-CONSOLE").
  ```C
  void console_set_title(char* title)
  ```
### Drawing primitives:
* Draw ASCII character c at x, y position.
  ```C
  void console_draw_char(Console console, int x, int y, char c, unsigned short color)
  ```
* Draw ASCII string at x, y position.
  ```C
  void console_draw_string(Console console, int x, int y, char* string, unsigned short color)
  ```
* Paints the console screen with the desired color.
  ```C
  void console_fill(Console console, unsigned short color)
  ```
### Events:
* Check if a key is pressed or not.
  ```C
  bool key_is_pressed(unsigned short key_code)
  ```
### Colors:
The Windows command-line supports just 16 RGB color combinations for both the background and
foreground character colors. EASY-CONSOLE provides
[Macros](https://github.com/sidmostefaoui/Easy-Console/blob/master/colors.txt) for all possible
combinations. Here's how to use them:
* To draw characters or strings OR the background and foreground colors together.
  ```C
  console_draw_char(console, x, y, c, BACKGROUND_BLACK | FOREGROUND_WHITE);
  ```
* To fill the console use the same color for both background and foreground.
  ```C
  console_fill(console, BACKGROUND_WHITE | FOREGROUND_WHITE);
  ```
### Key Codes:
* For alphabet keys(A, B, C......,Z), just use the **uppercase** ASCII value of the key.
  ```C
  if(key_is_pressed('A') {
      // do something
  }
  ```
  ```C
  if(key_is_pressed(0x41) {
      // do something
  }
  ```
* For other keys see:
  [Virtual-Key Codes](https://docs.microsoft.com/en-us/windows/desktop/inputdev/virtual-key-codes)
