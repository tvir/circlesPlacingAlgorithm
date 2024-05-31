#pragma once

#include <iosfwd>
#include <vector>
#include <string>
#include <exception>

namespace objects {
    class ObjectConstructionException : public std::exception {
    public:
        ObjectConstructionException(const std::string& error = "") { error_str += error; }
        const char* what() const noexcept override { return error_str.c_str(); }
    private:
        std::string error_str{ "Object construction exception: " };
    };

    struct Point {
        double x{};
        double y{};
    };

    inline bool operator>(const Point& a, const Point& b) { return a.x > b.x && a.y > b.y; }
    inline bool operator<(const Point& a, const Point& b) { return a.x < b.x && a.y < b.y; }
    inline bool operator==(const Point& a, const Point& b) { return a.x == b.x && a.y == b.y; }
    inline bool operator!=(const Point& a, const Point& b) { return !(a == b); }
    inline bool operator<=(const Point& a, const Point& b) { return a.x <= b.x && a.y <= b.y; }
    inline bool operator>=(const Point& a, const Point& b) { return a.x >= b.x && a.y >= b.y; }

    class Rectangle {
    public:
        Rectangle(const Point& min, const Point& max) : min_point{ min }, max_point{ max } {
            if (min >= max)
                throw ObjectConstructionException("Unnormalized rectangle");
        }
        Rectangle(const Rectangle& rectangle) = default;
        Rectangle(Rectangle&& rectangle) = default;
        Point minPoint() const { return min_point; }
        Point maxPoint() const { return max_point; }
    private:
        Point min_point;
        Point max_point;
    };

    class Circle {
    public:
        Circle(int id, double inner, double outter) : id{ id }, inner_rad{ inner }, outter_rad{ outter } {
            if (inner > outter || inner <= 0)
                throw ObjectConstructionException("Incorrect circle radiuses");
        }
        double inRad() const { return inner_rad; }
        double outRad() const { return outter_rad; }
        int getId() const { return id; }

    private:
        double inner_rad;
        double outter_rad;
        int id;
    };

    class PositionedCircle : public Circle {
    public:
        PositionedCircle(int id, double inner, double outter) : Circle{id, inner, outter} {}
        PositionedCircle(const Circle& circle, Point p) : Circle{ circle }, position{p} {}
        PositionedCircle(const Circle& circle) : Circle{circle} {}
        Point position{};
    };
    
    class Scene {
    public:
        Scene(const Rectangle& zone) : zone{zone} {}

        void addExclusionArea(const Rectangle& area) {
            if (area.minPoint() >= zone.minPoint() && area.maxPoint() <= zone.maxPoint())
                exclusion_areas.push_back(area); 
            else
                throw ObjectConstructionException("Incorrect exclusion areas positions");
        }
        void addCircle(const Circle& circle) {
            circles.push_back(circle);
        }

        const std::vector<Rectangle>& getExclusionAreas() const { return exclusion_areas; }
        const std::vector<Circle>& getCircles() const { return circles; }
        const Rectangle& getZone() const { return zone; }

    private:
        Rectangle zone;
        std::vector<Rectangle> exclusion_areas;
        std::vector<Circle> circles;
    };

    struct ResultData {
        std::vector<PositionedCircle> circles;
    };

    std::ostream& operator<< (std::ostream& out, const Point& p);
    std::ostream& operator<< (std::ostream& out, const Rectangle& r);
    std::ostream& operator<< (std::ostream& out, const Circle& c);
    std::ostream& operator<< (std::ostream& out, const PositionedCircle& c);
    std::ostream& operator<< (std::ostream& out, const Scene& s);
    std::ostream& operator<< (std::ostream& out, const ResultData& r);
}