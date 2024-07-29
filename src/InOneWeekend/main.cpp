#include "color.h"
#include "vec3.h"
#include "ray.h"
#include <iostream>
#include "rtweekend.h"
#include"hittable.h"
#include"hittable_list.h"
#include"sphelper.h"
#include "sphere .h"
#include"camera.h"


//Hit Spahere and Shadding
//X, Y, Z axis
//x - max, y - min->red
//x - min, y - max->green
//x - min, y - min->blue
//x - min, y - min->blue
//x - max, y - max->brigtes(white)




int main() {
    hittable_list world;

    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.render(world);
}