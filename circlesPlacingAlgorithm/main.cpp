#include <iostream>

#include "objects.hpp"
#include "DataLoader.hpp"
#include "Algorithm.hpp"
#include "ImageCreator.hpp"
#include "AreasGrid.hpp"

std::string getUserInput(std::string_view text) {
	std::string user_input;
	std::cout << text;
	std::cin >> user_input;
	return user_input;
}

std::optional<objects::Scene> getScene() {
	auto dataLoader = dataloader::createDefaultDataLoader();
	for (int i = 0; i < 3; ++i) {
		try {
			auto path = getUserInput("Input file path: ");
			return dataLoader->loadData(path.c_str());
		} catch (std::exception& e) {
			std::cout << e.what() << "\n";
		}
	}
	return std::nullopt;
}

void saveResults(const objects::ResultData& res) {
	auto dataLoader = dataloader::createDefaultDataLoader();
	for (int i = 0; i < 3; ++i) {
		try {
			auto path = getUserInput("Output file path: ");
			dataLoader->saveData(res, path.c_str());
			return;
		} catch (std::exception& e) {
			std::cout << e.what() << "\n";
		}
	}
}

void saveImage(const objects::Scene& scene, const objects::ResultData& res) {
	for (int i = 0; i < 3; ++i) {
		try {
			auto path = getUserInput("Output image file path (png): ");
			imagecreator::createResultingImage(scene, res, path);
			return;
		} catch (std::exception& e) {
			std::cout << e.what() << "\n";
		}
	}
}


int main() {
	auto data = getScene();
	if (!data)
		return 0;

	auto algorithm = algo::createDefaultAlgorithm();
	auto res = algorithm->calculate(data.value());

	if (!res) {
		std::cout << "Algorithm couldn't calculate circles positions\n";
		return 0;
	}
	saveResults(res.value());
	saveImage(data.value(), res.value());
	return 0;
}