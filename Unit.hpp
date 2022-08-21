#pragma once
#include "LinMath.hpp"
#include <string>
#include <sstream>


class Unit {
public:
    Unit(float FOV, float view_distance) : FOV(FOV), view_distance(view_distance)
    {
        //It may be calculated one time when launches program,
        //but then any unit has the same FOV. My idea is ownn FOV for each unit,
        //but calculate cos one time when creates unit
        float l = view_distance / std::cos(DegreesToRadians(FOV / 2.0f));
        view_frustrum_width = 2.0f * std::sqrt(l * l - view_distance * view_distance);
    };
    ~Unit(){};
    void SetTransform(const vec2D& pos, const vec2D& dir)
    {
        position = pos;
        direction = dir;
        vec2D height = direction * view_distance;
        vec2D half_base = height.makePerpendicular().normalize().scale(view_frustrum_width / 2.0f);
        view_frustrum_v2 = (height + half_base).translate(pos);
        view_frustrum_v3 = (height - half_base).translate(pos);
    }

    std::string ToString() const {
        std::stringstream ss;
        ss << "pos = [" << position.x << ", " << position.y <<
            "]; dir = [" << direction.x << ", " << direction.y << "]";
        return ss.str();
    }

    vec2D position;
    vec2D direction;
    float FOV;//in degrees
    float view_distance;
    vec2D view_frustrum_v2;
    vec2D view_frustrum_v3;
    protected:
    float view_frustrum_width;

};
