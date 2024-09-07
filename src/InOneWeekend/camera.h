#ifndef CAMERA_H
#define CAMERA_H
#include "rtweekend.h"
#include"interval.h"
#include "hittable.h"
#include"rtweekend.h"
#include"vec3.h"
#include "material.h"
#include "pdf.h"



class camera {
public:
	double aspect_ratio = 1.0;
	int image_width = 100;
	int samples_per_pixel = 10;		//random samples for every pixel
	int    max_depth = 10;   // Maximum number of ray bounces into scene, Diffuse limit
	double vfov = 90;	//Vertical field of view (angle)
	point3 lookfrom = point3(0, 0, 0);   // Point camera is looking from
	point3 lookat = point3(0, 0, -1);  // Point camera is looking at
	vec3   vup = vec3(0, 1, 0);     // Camera-relative "up" direction
	double defocus_angle = 0;  // Variation angle of rays through each pixel
	double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus
	color  background;               // Background color

	void render(const hittable& world, const hittable& lights) {
		initialize();

		std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

		for (int j = 0; j < image_height; j++) {
			std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
			for (int i = 0; i < image_width; i++) {
				color pixel_color(0, 0, 0);
				for (int s_j = 0; s_j < sqrt_spp; s_j++) {
					for (int s_i = 0; s_i < sqrt_spp; s_i++) {
						ray r = get_ray(i, j, s_i, s_j);
						pixel_color += ray_color(r, max_depth, world, lights);
					}
				}
				write_color(std::cout, pixel_samples_scale * pixel_color);
			}
		}

		std::clog << "\rDone.                 \n";
	}
private:
	int    image_height;         // Rendered image height
	double pixel_samples_scale;  // Color scale factor for a sum of pixel samples
	point3 center;               // Camera center
	point3 pixel00_loc;          // Location of pixel 0, 0
	vec3   pixel_delta_u;        // Offset to pixel to the right
	vec3   pixel_delta_v;        // Offset to pixel below
	vec3   u, v, w;              // Camera frame basis vectors
	vec3   defocus_disk_u;       // Defocus disk horizontal radius
	vec3   defocus_disk_v;       // Defocus disk vertical radius
	int    sqrt_spp;             // Square root of number of samples per pixel
	double recip_sqrt_spp;       // 1 / sqrt_spp

	void initialize() {
		image_height = int(image_width / aspect_ratio);
		image_height = (image_height < 1) ? 1 : image_height;
		sqrt_spp = int(std::sqrt(samples_per_pixel));
		pixel_samples_scale = 1.0 / (sqrt_spp * sqrt_spp);
		recip_sqrt_spp = 1.0 / sqrt_spp;
		pixel_samples_scale = 1.0 / samples_per_pixel;

		center = lookfrom;

		// Determine viewport dimensions.
		auto theta = degrees_to_radians(vfov);
		auto h = std::tan(theta / 2);
		auto viewport_height = 2 * h * focus_dist;
		auto viewport_width = viewport_height * (double(image_width) / image_height);

		// Calculate the u,v,w unit basis vectors for the camera coordinate frame.
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);

		// Calculate the vectors across the horizontal and down the vertical viewport edges.
		vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
		vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

		// Calculate the horizontal and vertical delta vectors from pixel to pixel.
		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		// Calculate the location of the upper left pixel.
		auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

		// Calculate the camera defocus disk basis vectors.
		auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
		defocus_disk_u = u * defocus_radius;
		defocus_disk_v = v * defocus_radius;
	}

	ray get_ray(int i, int j, int s_i, int s_j) const {
		auto offset = sample_square_stratified(s_i, s_j);
		auto pixel_sample = pixel00_loc
			+ ((i + offset.x()) * pixel_delta_u)
			+ ((j + offset.y()) * pixel_delta_v);

		auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
		auto ray_direction = pixel_sample - ray_origin;
		auto ray_time = random_double();

		return ray(ray_origin, ray_direction, ray_time);
	}

	vec3 sample_square() const {// vector to a random point  [-.5,-.5]-[+.5,+.5] 
		return vec3(random_double() - 0.5, random_double() - 0.5, 0);
	}

	point3 defocus_disk_sample() const {		//random point in the camera defocus disk
		auto p = random_in_unit_disk();
		return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
	}

	color ray_color(const ray& r, int depth, const hittable& world, const hittable& lights)
		const {
		if (depth <= 0)		//Break if ray bounce limit is excited
			return color(0, 0, 0);


		hit_record rec;


		if (!world.hit(r, interval(0.001, infinity), rec))		// return backgorund color if ray doesn't hit an object
			return background;

		ray scattered;
		color attenuation;
		color color_from_emission = rec.mat->emitted(r, rec, rec.u, rec.v, rec.p);	//light noisines fix, emit light in only one direction
		double pdf_value;


		if (!rec.mat->scatter(r, rec, attenuation, scattered, pdf_value))
			return color_from_emission;

		cosine_pdf surface_pdf(rec.normal);
		scattered = ray(rec.p, surface_pdf.generate(), r.time());
		pdf_value = surface_pdf.value(scattered.direction());

		auto on_light = point3(random_double(213, 343), 554, random_double(227, 332));
		auto to_light = on_light - rec.p;
		auto distance_squared = to_light.length_squared();
		to_light = unit_vector(to_light);

		if (dot(to_light, rec.normal) < 0)
			return color_from_emission;

		//Light ray
		double light_area = (343 - 213) * (332 - 227);
		auto light_cosine = std::fabs(to_light.y());
		if (light_cosine < 0.000001)
			return color_from_emission;

		pdf_value = distance_squared / (light_cosine * light_area);
		scattered = ray(rec.p, to_light, r.time());


		hittable_pdf light_pdf(lights, rec.p);
		scattered = ray(rec.p, light_pdf.generate(), r.time());
		pdf_value = light_pdf.value(scattered.direction());

		double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);

		color sample_color = ray_color(scattered, depth - 1, world, lights);
		color color_from_scatter = (attenuation * scattering_pdf * sample_color) / pdf_value;

		return color_from_emission + color_from_scatter;
	}

	vec3 sample_square_stratified(int s_i, int s_j) const {
		// Returns the vector to a random point in the square
		// indices s_i and s_j - idealized unit square pixel [-.5,-.5] to [+.5,+.5].
		auto px = ((s_i + random_double()) * recip_sqrt_spp) - 0.5;
		auto py = ((s_j + random_double()) * recip_sqrt_spp) - 0.5;
		return vec3(px, py, 0);
	}
};

#endif