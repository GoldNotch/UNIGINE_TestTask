#pragma once
#include <memory>
#include <vector>
#include <functional>
#include <stack>

struct BoundingBox
{
    float left, top;
    float width, height;
    inline constexpr float GetRight() const noexcept{return left + width;}
    inline constexpr float GetBottom() const noexcept{return top - height;}
    inline constexpr bool ContainsPoint(const vec2D& point) const noexcept
        {return point.x >= left && point.y <= top &&
                point.x <= GetRight() && point.y >= GetBottom();}
    inline constexpr bool Intersects(const BoundingBox& other) const noexcept
    {
        return !(left >= other.GetRight() ||
                 GetRight() <= other.left ||
                    top <= other.GetBottom() ||
                 GetBottom() >= other.top);
    }
};

template<typename T, size_t node_capacity = 1>
class Quadtree
{
public:
    Quadtree(float scene_half_size)
    {
        root.bbox.left = -scene_half_size;
        root.bbox.top = scene_half_size;
        root.bbox.width = 2 * scene_half_size;
        root.bbox.height = 2 * scene_half_size;
    }

    inline void AddPoint(const vec2D& point, const T* object_weak_ptr)
    {
        if (root.bbox.ContainsPoint(point)){
            TreeNode* cur_node = &root;
            //find leaf node to insert point
            while(!cur_node->IsLeaf())
                cur_node = cur_node->FindChildQuad(point);
            //break up node if it's full
            while (cur_node->points_count == node_capacity)
            {
                cur_node->BreakUp();
                cur_node = cur_node->FindChildQuad(point);
            }
            cur_node->AddPoint({point, object_weak_ptr});
        }
    }

    using PointProcessingFunc = std::function<void(const vec2D& point, const T* object_weak_ptr)>;
    //returns checked points count. Traverse quadtree. Complexity: O((d + 1)n), where d - height of tree and n - node_capacity
    inline int ForEachPointInBB(const BoundingBox& area_bbox, const PointProcessingFunc& process_point) const
    {
        if (!root.bbox.Intersects(area_bbox))
            return 0;
        size_t checked_points_count = 0;
        std::stack<const TreeNode*> way;//better create stack as array, and limit height of tree
        way.push(&root);
        while(!way.empty())
        {
            auto node = way.top();
            way.pop();
            for(size_t i = 0; i < node->points_count; ++i)
            {
                auto point_data = node->points[i];//for thread-safety it must be atomic
                if (area_bbox.ContainsPoint(point_data.point))
                    process_point(point_data.point, point_data.object_weak_ptr);
                checked_points_count++;
            }

            for(auto& quad: node->children)//for thread-safety it must be atomic
                if (quad.bbox.Intersects(area_bbox))
                    way.push(&quad);
        }

        return checked_points_count;
    }

private:
    enum QuadrantID
    {
        NE,//right up
        NW,//left up
        SW,//left bottom
        SE,//right bottom
        QUADRANTS_TOTAL
    };
    struct PointData
    {
        vec2D point;
        const T* object_weak_ptr;
    };
    struct TreeNode
    {
        BoundingBox bbox;
        std::vector<TreeNode> children;//don't store children in node, better make global storage in tree and points on nodes from there
        PointData points[node_capacity];
        size_t points_count = 0;

        ~TreeNode() {children.resize(0);}

        inline constexpr bool IsLeaf() const noexcept {return children.size() == 0;}

        inline constexpr TreeNode* FindChildQuad(const vec2D& point) const noexcept
        {
            if (children[NE].bbox.ContainsPoint(point)) return (TreeNode*)&children[NE];
            else if (children[NW].bbox.ContainsPoint(point)) return (TreeNode*)&children[NW];
            else if (children[SW].bbox.ContainsPoint(point)) return (TreeNode*)&children[SW];
            else if (children[SE].bbox.ContainsPoint(point)) return (TreeNode*)&children[SE];
            else return nullptr;
        }
        inline void AddPoint(const PointData& data)
        {
            points[points_count++] = data;
        }
        inline void BreakUp()
        {
            children.resize(QUADRANTS_TOTAL);
            const float half_width = bbox.width / 2.0f;
            const float half_height = bbox.height / 2.0f;
            for(auto& quad : children){
                quad.bbox.width = half_width;
                quad.bbox.height = half_height;
            }
            children[NE].bbox.top = bbox.top;
            children[NE].bbox.left = bbox.left + half_width;
            children[NW].bbox.top = bbox.top;
            children[NW].bbox.left = bbox.left;
            children[SW].bbox.top = bbox.top - half_height;
            children[SW].bbox.left = bbox.left;
            children[SE].bbox.top = bbox.top - half_height;
            children[SE].bbox.left = bbox.left + half_width;

            for(size_t i = 0; i < points_count; ++i){
                TreeNode* quad = FindChildQuad(points[i].point);
                quad->AddPoint(points[i]);
            }
            points_count = 0;
        }
    };
    TreeNode root;
};
