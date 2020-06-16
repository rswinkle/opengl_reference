opengl_reference
================

This is going to be a repository of modern (3.3/4.x) OpenGL programs.
Most of these will be either exploring/demonstrating concepts or
be framework/template programs for starting new projects.

There are better repositories of modern OpenGL programs to
use for learning because they're more complete/organized,
they go with a book etc.  However, this is mostly for my own
personal use/code and will be constantly changing.  Writing
a bunch of OpenGL programs will also force me to learn glm
and finish my own OpenGL math/helper code.

Download
========
Get the source from [Github](https://github.com/rswinkle/opengl_reference).


I'll try to maintain a table/list of programs here.  Maybe
I'll split them up by category (templates/frameworks, feature examples, demos, "games") later.
Those listed use my own math library, rsw_math, but there are glm versions as well, ie glm_modelviewer.cpp.

| Program | Description | Image |
| --- | --- | --- |
| ex1.c | Hello Triangle                                            | ![ex1](https://raw.githubusercontent.com/rswinkle/opengl_reference/master/media/screenshots/ex_1.png) |
| ex2.c | Hello Interpolation                                       | ![ex2](https://raw.githubusercontent.com/rswinkle/opengl_reference/master/media/screenshots/ex_2.png) |
| gears.c | Classic gears demo.  Switch between polygon modes with p.  | ![gears](https://raw.githubusercontent.com/rswinkle/opengl_reference/master/media/screenshots/gears.png) |
| ex3.cpp | Load a model from ./media/models.  Switch between Gouraud and Phong shading with s.  Switch between polygon modes with p. | ![ex3](https://raw.githubusercontent.com/rswinkle/opengl_reference/master/media/screenshots/ex_3.png) |
| ex4.cpp | Switch between textures with 1, filtering modes with f, zoom/rotate with arrow keys | ![ex4](https://raw.githubusercontent.com/rswinkle/opengl_reference/master/media/screenshots/ex_4.png) |
| grass.cpp | Based on example in superbible, uses flying controls, WIP | ![grass](https://raw.githubusercontent.com/rswinkle/opengl_reference/master/media/screenshots/grass.png) |
| point_sprites.c | Point Sprite example, uses multitexturing and discard keyword in fragment shader to "disolve" textured point.  | ![point_sprites](https://raw.githubusercontent.com/rswinkle/opengl_reference/master/media/screenshots/point_sprites.png) |
| sphereworld_color.cpp | Based on SphereWorld example from SB with ADS lighting, moving/rotating objects, with flying controls.  | ![sphere_world_color](https://raw.githubusercontent.com/rswinkle/opengl_reference/master/media/screenshots/sphereworld_color.png) |
| flying.cpp | Template with 6DOF flying controls ala Descent. Mouse + WASDQE + LShift + Space | ![flying](https://raw.githubusercontent.com/rswinkle/opengl_reference/master/media/screenshots/flying.png) |
| left_handed.cpp | [OpenGL *is* left handed](https://stackoverflow.com/questions/5168163/is-the-opengl-coordinate-system-right-handed-or-left-handed/22675164#22675164). | ![left_handed](https://raw.githubusercontent.com/rswinkle/opengl_reference/master/media/screenshots/left_handed.png) |
