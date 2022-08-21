#pragma once
#include "Unit.hpp"
#include "Quadtree.hpp"
#include "ThreadPool.hpp"
#include <vector>
#include <functional>
#include <algorithm>





class Level
{
public:
    Level(size_t units_count, float scene_size, ThreadPool* thread_pool = nullptr) :
        quadtree(scene_size), thread_pool(thread_pool)
    {
        units.reserve(units_count);
    };
    ~Level(){}
    inline size_t AddUnit(const Unit& unit)
    {
        units.push_back(unit);
        quadtree.AddPoint(unit.position, &unit);
        return units.size() - 1;
    }

    using UnitProcessingFunc = std::function<void (size_t id, const Unit& unit)>;
    inline void ForEachUnit(const UnitProcessingFunc& process_unit) const
    {
        if (thread_pool)
            for(size_t i = 0; i < units.size(); ++i){
                auto& unit = units[i];
                thread_pool->QueueTask([=]{process_unit(i, unit);});
            }
        else
            for(size_t i = 0; i < units.size(); ++i)
                process_unit(i, units[i]);
    }

    //return how many units viewed by the unit. Complexity: O()
    inline size_t GetViewedUnitsCount(const Unit& unit) const
    {
        //build field of view as triangle with vertices v1, v2, v3
        vec2D v1 = unit.position;
        vec2D v2 = unit.view_frustrum_v2;
        vec2D v3 = unit.view_frustrum_v3;

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
            //std::lock_guard<std::mutex> lk(use_quadtree);// I don't use mutex because traverse of quadtree is read-only operation and there is no thread is changing quadtree
            int checked_points_count = quadtree.ForEachPointInBB(area_bbox,
                                    [v1, v2, v3, &units_in_fov]
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
    Quadtree<Unit, 8> quadtree;
    ThreadPool *thread_pool;
};
