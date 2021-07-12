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
 Select the light types:
▪ 7: All lights become point lights.
▪ 8: All lights become spot lights.
▪ 9: All lights become directional lights.
 P: Toggle to pause/start the light animation.
 N: Toggle normal rendering
 F: Toggle face/averaged normal
 M: Toggle wireframe mode on/off
 T: Change between rendering modes:
▪ Normal Mapping: Scene with lighting and normal mapping with texture
▪ Normals: Geometry normal in camera space as color
▪ Tangent: Geometry tangent in camera space as color
▪ Bitangent: Geometry bitangent in camera space as color