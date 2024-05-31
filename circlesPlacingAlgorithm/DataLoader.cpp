#include "DataLoader.hpp"

#include <iostream>

#include "xmlAttributes.hpp"

namespace dataloader {
    std::unique_ptr<DataLoader> createDefaultDataLoader() {
        return std::make_unique<XmlDataLoader>();
    }

    objects::Scene XmlDataLoader::loadData(const char* path) {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(path);
        if (!result)
            throw DataLoadException("File can't be opened");

        auto zoneNode = doc.select_node(xmlAttributes::zonePath).node();
        objects::Scene scene(loadRectangle(zoneNode));

        auto alarmsNodes = doc.select_nodes(xmlAttributes::alarmsPath);
        for (pugi::xpath_node_set::const_iterator it = alarmsNodes.begin(); it != alarmsNodes.end(); ++it) {
            scene.addExclusionArea(loadRectangle(it->node()));
        }

        auto circlesNodes = doc.select_nodes(xmlAttributes::circlesPath);
        for (pugi::xpath_node_set::const_iterator it = circlesNodes.begin(); it != circlesNodes.end(); ++it) {
            scene.addCircle(loadCircle(it->node()));
        }

        return scene;
    }

    void XmlDataLoader::saveData(const objects::ResultData& results, const char* path) {
        pugi::xml_document doc;

        pugi::xml_node node = doc.append_child(xmlAttributes::resultStr[0]);
        for (size_t i = 1; i < xmlAttributes::resultStr.size() - 1; ++i) {
            node = node.append_child(xmlAttributes::resultStr[i]);
        }
        for (auto& c : results.circles) {
            pugi::xml_node circle = node.append_child(xmlAttributes::resultStr.back());
            circle.append_attribute(xmlAttributes::id).set_value(c.getId());
            circle.append_attribute(xmlAttributes::x).set_value(c.position.x);
            circle.append_attribute(xmlAttributes::y).set_value(c.position.y);
        }

        if (!doc.save_file(path))
            throw DataLoadException("File can't be saved");
    }

    objects::Rectangle XmlDataLoader::loadRectangle(const pugi::xml_node& node) {
        double minX = loadAttribute(node.child(xmlAttributes::minPoint), xmlAttributes::x).as_double();
        double minY = loadAttribute(node.child(xmlAttributes::minPoint), xmlAttributes::y).as_double();
        double maxX = loadAttribute(node.child(xmlAttributes::maxPoint), xmlAttributes::x).as_double();
        double maxY = loadAttribute(node.child(xmlAttributes::maxPoint), xmlAttributes::y).as_double();
        return objects::Rectangle{ {minX, minY}, {maxX, maxY} };
    }

    objects::Circle XmlDataLoader::loadCircle(const pugi::xml_node& node) {
        int id = loadAttribute(node, xmlAttributes::id).as_int();
        double inRad = loadAttribute(node, xmlAttributes::inRad).as_double();
        double outRad = loadAttribute(node, xmlAttributes::outRad).as_double();
        return objects::Circle{ id, inRad, outRad };
    }

    pugi::xml_attribute XmlDataLoader::loadAttribute(const pugi::xml_node& node, const char* attributeName) {
        auto attribute = node.attribute(attributeName);
        if (attribute.empty())
            throw DataLoadException("Incorrect data structure");
        return attribute;
    }
}