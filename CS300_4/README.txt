/* ---------------------------------------------------------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
Project: cs300_oscar.s_2
Author: Oscar Sanchez Bayton Griffith, oscar.s, 540002116
Creation date: 06/20/2021
----------------------------------------------------------------------------------------------------------*/
Move the camera around in always looking at the object. 
▪ W: Move up.
▪ S: Move down.
▪ A: Move left. 
▪ D: Move right. 
▪ E: Further from object. 
▪ Q: Closer to object. 
 Select shape to be rendered through the number keys.
▪ Numbers 1 to 5: Change the shape to be rendered
– 1: Plane
– 2: Cube
– 3: Cone
– 4: Cylinder
– 5: Sphere
▪ +: Increase the shape subdivisions
▪ -: Decrease the shape subdivisions
 O: Toggle to pause/start adjacent object animation.
 N: Toggle normal rendering
 F: Toggle face/averaged normal
 M: Toggle wireframe mode on/off
 T: Cycle between the different modes (texture/normal reflection/reflective/ 
refractive)
 Object rotation for center shape.
▪ Arrows Up/Down: Rotate the shape along X-axis
▪ Arrows Right/Left: Rotate the shape along Y-axis