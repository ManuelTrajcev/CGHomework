#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
public:
    // stationary 
    sphere(const point3& center, double radius, shared_ptr<material> mat)
        : center1(center), radius(std::fmax(0, radius)), mat(mat), is_moving(false) {}

    // moving 
    sphere(const point3& center1, const point3& center2, double radius,     //center1 -> center2 za vreme [0,1]
        shared_ptr<material> mat)
        : center1(center1), radius(std::fmax(0, radius)), mat(mat), is_moving(true)
    {
        center_vec = center2 - center1;
    }
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        point3 center = is_moving ? sphere_center(r.time()) : center1;  //alteranive - center1=center2

        vec3 oc = center - r.origin();
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius * radius;

        auto discriminant = h * h - a * c;
        if (discriminant < 0)
            return false;

        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    }

private:
    point3 center1;
    double radius;
    bool is_moving;
    shared_ptr<material> mat;

    point3 sphere_center(double time) const {
        return center1 + time * center_vec; //center1 -> center2, linearno pomestuvanje za vreme na interval [0,1]
    }
};

#endif