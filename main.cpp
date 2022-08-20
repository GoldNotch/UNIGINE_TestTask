#include <cstdio>
#include <cstdlib>
#include "Level.hpp"
#include <thread>
#include <random>

using namespace std;

static inline float randomFloat(float min, float max)
{
    return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

static inline vec2D randomVec2D(float min = -1.0f, float max = 1.0f)
{
    return vec2D(randomFloat(min, max), randomFloat(min, max));
}

int main()
{
    srand(time(NULL));
    const size_t UnitsCount = 10000;
    const float FOV = 135.0f;
    const float view_distance = 2.0f;
    const float max_scene_size = 10.0f;
    Level l(UnitsCount, max_scene_size);
    //generate units
    for(size_t i = 0; i < UnitsCount; ++i)
    {
        Unit unit(FOV, view_distance);
        unit.SetTransform(randomVec2D(-max_scene_size, max_scene_size),
                        randomVec2D().normalize());
        l.AddUnit(unit);
        //printf("%s\n", unit.ToString().c_str());//to show info about units
    }

    size_t viewed_units_count[UnitsCount];
    l.ForEachUnit([&l, &viewed_units_count](size_t id, const Unit& unit){
                        viewed_units_count[id] = l.GetViewedUnitsCount(unit);
                  });

    for(size_t i = 0; i < UnitsCount; ++i)
        printf("unit %llu: %llu\n", i, viewed_units_count[i]);
	return 0;
}
