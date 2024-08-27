#ifndef QUAD_H
#define QUAD_H

#include "rtweekend.h"

#include "hittable.h"

class quad : public hittable {
public:
    quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat) : Q(Q), u(u), v(v), mat(mat)
    {
        auto n = cross(u, v);
        normal = unit_vector(n);
        D = dot(normal, Q);
        w = n / dot(n, n);
        set_bounding_box();
    }

    virtual void set_bounding_box() {       //bounding boxes
        auto bbox_diagonal1 = aabb(Q, Q + u + v);
        auto bbox_diagonal2 = aabb(Q + u, Q + v);
        bbox = aabb(bbox_diagonal1, bbox_diagonal2);
    }

    aabb bounding_box() const override { return bbox; }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        auto denom = dot(normal, r.direction());
        if (std::fabs(denom) < 1e-8)    // ray - parallel to the plane
            return false;

        auto t = (D - dot(normal, r.origin())) / denom;     // t is outside the ray interval
        if (!ray_t.contains(t))
            return false;

        auto intersection = r.at(t);

        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);

        return true;
    }

private:
    point3 Q;
    vec3 u, v;
    shared_ptr<material> mat;
    aabb bbox;
    vec3 normal;
    double D;
    vec3 w;
};

#endif