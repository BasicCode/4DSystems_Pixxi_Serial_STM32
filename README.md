# 4D Systems Pixxi Based Displays: Serial Interface

This is a direct port of the serial display library for 4D Systems' Pixxi based display modules for use with the STM32 HAL library.<br>
This library implements the HAL UART and GPIO features; all of the hardware serial interactions are contained in the first few functios if you need to modify them to suit your particular project.

* Original manufacturer code is available here: https://github.com/4dsystems/Pixxi-Serial-Arduino-Library/.

## Installation
* NOTE: An aditional file containing the commands and other constants is required. Download *Pixxi_Const4D.h* from the above library and include it with your project.
Add the *.cpp* and *.h* files to their respective parts of your project. The file *main.cpp* is included as an example to initialise the display, but your probably don't
want to include this in your own project.

## Usage
An example program is included in *main.cpp* which initialises the display, initialises the SD / FLASH storage, and displays some shaped.
* Create an instance of the Pixxi Serial class.
```
Pixxi_Serial_4DLib Display(&huart1);
```
* Reset the display by pulling the **RESET** pin LOW for at least 2 microseconds.
* Set the display to **LANDSCAPE** or **PORTRAIT**
```
Display.gfx_ScreenMode(PORTRAIT);
```
* The display may have a test pattern on it at initial boot which can then be cleared:
```
Display.gfx_Cls();
```

<br><br>
Feel free to add functions and modify as required. Licensed under GNUv3.