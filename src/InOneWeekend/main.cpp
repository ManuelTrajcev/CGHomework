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
#include "material.h"



//Hit Spahere and Shadding
//X, Y, Z axis
//x - max, y - min->red
//x - min, y - max->green
//x - min, y - min->blue
//x - min, y - min->blue
//x - max, y - max->brigtes(white)




int main() {
	hittable_list world;

	auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			auto chose_mat = random_double();
			point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

			if ((center - point3(4, 0.2, 0)).length() > 0.9)
			{
				shared_ptr<material> sphere_material;
				if (chose_mat < 0.8)		//diffuse
				{
					auto albed = color::random() * color::random();
					sphere_material = make_shared<lambertian>(albed);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else if (chose_mat < 0.95)		//metal
				{
					auto albed = color::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<metal>(albed, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				} 
				else		//glass
				{
					sphere_material = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

	camera cam;

	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 1200;
	cam.samples_per_pixel = 500;
	cam.max_depth = 50;
	cam.vfov = 20;
	cam.lookfrom = point3(13, 2, 3);
	cam.lookat = point3(0, 0, 0);
	cam.vup = vec3(0, 1, 0);
	cam.defocus_angle = 0.6;
	cam.focus_dist = 10.0;

	cam.render(world);
}