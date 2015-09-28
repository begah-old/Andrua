-- Andrua --

Andrua is an Code Editor for Lua with a simple Image Editor.
Andrua runs on both Desktop (Linux/Windows and supposibly OSX but not tested) 
	and on Android, lowest supported version of Android is version 2.3 ( API level 9 )

-- Features --

- Code Editor
	- Clipboard support
	- Text highlighting
- Lua Interpreter
	- Reworked to work on Android, specifically inputs, outputs
	- Custom console ( shows program output, no input possible )
	- Graphical library to render images, colored shapes, gui functionallity's and text rendering
	- Can have a mix of functions in main file such as : init, render, close, mouse_action, none are required
	- Can have a mix of variables in main file such as : mouse_x, mouse_y, Screen_Width, Screen_Height, useDisplay, useFullScreen
- Image Editor
	- Pencil tool
	- Color picker tool
	- Image panning tool
	- Color filler tool
	- Color creating tool ( combine red, green, blue and alpha )

- Orginized into folder "projects", all your files and images will be in a project folder
- Delete/Rename/Create new files

- Opensource, small and extensible

All Andrua's specific functions are described in LIBRARY.txt

-- Compiling --

See COMPILE.platform

