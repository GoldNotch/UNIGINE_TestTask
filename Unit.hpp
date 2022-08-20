#pragma once
#include "LinMath.hpp"
#include <string>
#include <sstream>


class Unit {
public:
    Unit(float FOV, float view_distance) : FOV(FOV), view_distance(view_distance){};
    ~Unit(){};
    void SetTransform(const vec2D& pos, const vec2D& dir)
    {
        position = pos;
        direction = dir;
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
};
