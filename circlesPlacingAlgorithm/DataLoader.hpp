#pragma once

#include <memory>

#include "pugixml/pugixml.hpp"
#include "objects.hpp"


namespace dataloader {
    class DataLoadException : public std::exception {
    public:
        DataLoadException(const std::string& error = "") { errorStr += error; }
        const char* what() const noexcept override { return errorStr.c_str(); }
    private:
        std::string errorStr{ "Data load exception: " };
    };

    class DataLoader {
    public:
        virtual objects::Scene loadData(const char* path) = 0;
        virtual void saveData(const objects::ResultData& results, const char* path) = 0;
        virtual ~DataLoader() = default;
    };

    std::unique_ptr<DataLoader> createDefaultDataLoader();

    class XmlDataLoader : public DataLoader {
    public:
        objects::Scene loadData(const char* path) override;
        void saveData(const objects::ResultData& results, const char* path) override;

    private:
        objects::Rectangle loadRectangle(const pugi::xml_node& node);
        objects::Circle loadCircle(const pugi::xml_node& node);
        pugi::xml_attribute loadAttribute(const pugi::xml_node& node, const char* attributeName);
    };

}

