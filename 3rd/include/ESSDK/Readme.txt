EarthSculptor SDK 0.75 Beta
Copyright (c) 2007 Ernest Szoka

You may reuse the sample source code in this sdk for any purpose without warranty at your own risk.

The purpose of this sdk is to allow you to create plugins for Earthsculptor. 
A template and source code for 4 existing plugins used in Earthsculptor are provided to give you a head start and give examples on how to implement your own plugins.

Plugins types are as follows:
export - export data out of Earthsculptor
import - import data into Earthsculptor
generate - modify the heightmap, terrain textures and/or map variables

To create a project from scratch you must link Earthsculptor.lib and include ESCore.h in your project.

The following functions must be defined as follows:

bool Init(ESPlugIn& out_plugIn) - initilizes the plugin UI(optional), data requests and local variables
bool Run(ESPlugIn& in_plugIn) - performs the plugins primary operation
bool Die(ESPlugIn& in_plugIn) - plugin shutdown


The project files included are Visual Studio C++ 6.0 but should compile under Visual C++ 2005 Express Edition and Visual Studio .Net after converting the project files. The plaform sdk for windows is required.

After compiling your plugin simply deposit the dll from the debug/release folder into the plugin folder in your Earthsulptor folder. Earthsculptor will detect all plugins in the plugin directory and attempt to load them. Plugins can be debugged by pointing the executable to Earthsculptor.exe.

Because this is still a beta sdk, the sdk is subject to change and new releases may break your developed plugins. 

For help on using the sdk go to the forum at www.earthsculptor.com/forum

