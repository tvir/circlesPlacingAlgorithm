#include "objects.hpp"
#include <ostream>

namespace objects{
    std::ostream& operator<< (std::ostream& out, const Point& p) {
        out << "{" << p.x << " " << p.y << "}";
        return out;
    }

    std::ostream& operator<< (std::ostream& out, const Rectangle& r) {
        out << "{" << r.minPoint() << " " << r.maxPoint() << "}";
        return out;
    }

    std::ostream& operator<< (std::ostream& out, const Circle& c) {
        out << "{id: " << c.getId() << " R: " << c.outRad() << " " << c.inRad() << "}";
        return out;
    }

    std::ostream& operator<< (std::ostream& out, const PositionedCircle& c) {
        out << "{id: " << c.getId() << " R: " << c.outRad() << " " << c.inRad() << " " << c.position << "}";
        return out;
    }

    std::ostream& operator<< (std::ostream& out, const Scene& s) {
        out << "Z: " << s.getZone() << "\n";
        for (auto& a : s.getExclusionAreas())
            out << "A: " << a << "\n";
        for (auto& c : s.getCircles())
            out << "C: " << c << "\n";
        return out;
    }

    std::ostream& operator<< (std::ostream& out, const ResultData& d) {
        for (auto& c : d.circles)
            out << "C: " << c << "\n";
        return out;
    }
}
