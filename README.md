# Table of Contents
* [Introduction](#intro)
* [Build](#build)
* [Usage](#usage)

## Introduction <a name="intro"></a>
This project draw a figure called "Mandelbox" using OpenGL 4.6. Also I wrote some functions to simplify the work with OpenGL and debugging, I will tell more in [Usage](#usage) section. You can read about Mandelbox figure [here](#link).  
Some screenshot of this figure, that my program draw.   
![Screen](screen/example.png)

You can see some another examples of this figure here [shadertoy](#shadertoy_link).

## Build <a name="build"></a>
This project was builded in my own system (Arch Linux 5.18.2-arch1-1). I do not test any other systems.  
For build do following:
~~~bash
git clone ../
mkdir build
cd build
cmake ..
~~~
This will create an executable file (named "Mandelbox").

## Usage <a name="usage"></a>
You can see the examples of usage in the [documentation](https:/github). 
Also you can see [GLFW](https://) documentation. GLAD is not provide doxygen documentation (at least in
my version of this library).
