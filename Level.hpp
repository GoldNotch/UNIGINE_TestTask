#pragma once
#include "Unit.hpp"
#include "Quadtree.hpp"
#include "ThreadPool.hpp"
#include <vector>
#include <functional>
#include <algorithm>



inline static constexpr float sign(const vec2D& p, const vec2D& v1, const vec2D& v2) noexcept
{
    return (p.x - v2.x) * (v1.y - v2.y) - (v1.x - v2.x) * (p.y - v2.y);
}

inline static bool IsPointInTriangle(const vec2D& p, const vec2D& v1, const vec2D& v2, const vec2D& v3) noexcept
{
    bool b1, b2, b3;
    b1 = sign(p, v1, v2) < 0.0f;
    b2 = sign(p, v2, v3) < 0.0f;
    b3 = sign(p, v3, v1) < 0.0f;
    return (b1 == b2) && (b2 == b3);
}

class Level
{
public:
    Level(size_t units_count, float scene_size, ThreadPool* thread_pool = nullptr) :
        quadtree(scene_size), thread_pool(thread_pool)
    {
        units.reserve(units_count);
    };
    ~Level(){}
    size_t AddUnit(const Unit& unit)
    {
        units.push_back(unit);
        quadtree.AddPoint(unit.position, &unit);
        return units.size() - 1;
    }

    using UnitProcessingFunc = std::function<void (size_t id, const Unit& unit)>;
    void ForEachUnit(const UnitProcessingFunc& process_unit) const
    {
        if (thread_pool)
            for(size_t i = 0; i < units.size(); ++i){
                auto& unit = units[i];
                thread_pool->QueueTask([=]
                                       {process_unit(i, unit);});
            }
        else
            for(size_t i = 0; i < units.size(); ++i)
                process_unit(i, units[i]);
    }

    size_t GetViewedUnitsCount(const Unit& unit)
    {
        //build field of view as triangle with vertices v1, v2, v3
        vec2D v1, v2, v3;
        v1 = unit.position;
        float l = unit.view_distance / std::cos(DegreesToRadians(unit.FOV / 2.0f));
        float half_base_length = fast_sqrt(l * l - unit.view_distance * unit.view_distance);
        vec2D height = unit.direction * unit.view_distance;
        vec2D half_base = height.makePerpendicular().normalize().scale(half_base_length);
        v2 = v1 + height + half_base;
        v3 = v1 + height - half_base;

        //build BoundingBox and check all nodes in BB
        BoundingBox area_bbox;
        area_bbox.left = std::min({v1.x, v2.x, v3.x});
        area_bbox.top = std::max({v1.y, v2.y, v3.y});
        float right = std::max({v1.x, v2.x, v3.x});
        float bottom = std::min({v1.y, v2.y, v3.y});
        area_bbox.width = right - area_bbox.left;
        area_bbox.height = area_bbox.top - bottom;
        size_t units_in_fov = 0;
        {
            std::lock_guard<std::mutex> lk(use_quadtree);
            int checked_points_count = quadtree.ForEachPointInBB(area_bbox,
                                    [&v1, &v2, &v3, &units_in_fov]
                                    (const vec2D& point, const Unit* unit_ptr)
                                    {
                                        if (point != v1 && IsPointInTriangle(point, v1,v2,v3)){
                                            ++units_in_fov;
                                            //printf("[%f, %f](%f)\n", point.x, point.y, (point - v1).length());
                                        }
                                    });
        }
        //printf("%i\n", checked_points_count);
        return units_in_fov;
    }
private:
    std::vector<Unit> units;
    Quadtree<Unit, 4> quadtree;
    std::mutex use_quadtree;
    ThreadPool *thread_pool;
};
