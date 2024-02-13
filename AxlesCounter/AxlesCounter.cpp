#include "common.h"
#include "AxleSensor.h"
#include "Section.h"
#include "TrainRoute.h"
#include "Train.h"
#include "Cycle.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/*
*  ���������� �������
*  ������ - ��������� ������������
*  ������� � ����� - ��������������� ������ ��� ������� ������� � ������
*  ���� - ���� 100 �� � ������� ���������� ���
*  ������ - ���
*/

std::mutex cout_mutex; // ������� ��� ������ � �������� ����� �� ������ �������

int main(int argc, char** argv) {
	//std::vector<std::string> errors;
	//errors.clear();
	std::string JSONFileName = "AxlesCounter.json";
	if (argc > 1) {
		JSONFileName = argv[1];
	}
	std::cout << "Loading configuration from " << JSONFileName << std::endl;
	if(argc == 1)
		std::cout << "To read the configuration from another file, pass it as a parameter" << std::endl;
	std::cout << "Working..." << std::endl;
	// ���� �������
	std::ofstream outputFile("output.txt");
	if (!outputFile.is_open()) {
		std::cerr << "Error opening the output file!" << std::endl;
		return 1;
	}
	std::streambuf* consoleBuffer = std::cout.rdbuf(); // Save the console buffer
	std::cout.rdbuf(outputFile.rdbuf()); // ��������������� std::cout � ����

	{
		// ³������� ����� JSON ��� ����������
		std::ifstream file(JSONFileName);
		// ��������, �� ������� ������� ����
		if (!file.is_open()) {
			std::cerr << "Could not open the file " << JSONFileName << std::endl;
			return 1;
		}
		// ���������� ����� � ����� JSON
		json data;
		try {
			file >> data;
		}
		catch (...) {
			std::cerr << "Error in JSON file!" << std::endl;
			return 1;
		}

		// 1 ����������
		// 1.1 ������� �������
		auto axle_sensors = data["AxleSensors"];
		// ������ ���������� �� �������
		std::vector<AxleSensor*> AxleSensors;

		if (!axle_sensors.is_null() && axle_sensors.is_array()) {
			for (const auto& sensor : axle_sensors) {
				int sensorID = sensor["SensorID"];
				int initValue = sensor["InitValue"];
				AxleSensors.push_back(new AxleSensor(sensorID, initValue)); 		// ��������, �������� ��������
			}
		}
		else {
			std::cerr << "Failed to access 'AxleSensors' field or it is not an array." << std::endl;
		}

		// 1.2 ������� ��������
		auto routes = data["Routes"];
		// ������ ���������� �� ��������
		std::vector<TrainRoute*> TrainRoutes;
		if (!routes.is_null() && routes.is_array() && routes.size()) {
			for (const auto& route : routes) {
				int routeID = route["RouteID"];
				TrainRoutes.push_back(new TrainRoute(routeID)); 	// ��������� ������ �������
				// ���������� �������� � �������
				json sensorsArray = route["Sensors"];
				if (!sensorsArray.is_null() && sensorsArray.is_array() && sensorsArray.size()) {
					for (const auto& sensor : sensorsArray) {
						int sensorID = sensor["SensorID"];
						int len = sensor["Length"];
						// �������� ������ �� ������ �� �������� ID
						auto AxleSensorIterator = AxleSensors.begin();
						bool added = false;
						for (AxleSensorIterator = AxleSensors.begin(); AxleSensorIterator != AxleSensors.end(); ++AxleSensorIterator) {
							if ((*AxleSensorIterator)->getId() == sensorID) {
								TrainRoutes.back()->addSensor((*AxleSensorIterator), len); // ���������� �������� � �������
								added = true;
								break;
							}
						}
						if (!added) {
							std::cerr << "Not found sensor for Route " << routeID << std::endl;
						}
					}
				}
				else {
					std::cerr << "Failed to access 'Routes - Sensors' field or it is not an array." << std::endl;
				}
			}
		}
		else {
			std::cerr << "Failed to access 'Routes' field or it is not an array." << std::endl;
		}

		// ��������, ��� � �������� ���� ���� �� ���� ������
		for (const auto& trainRoute : TrainRoutes) {
			if((trainRoute->getSensors()).size() == 0)
				std::cerr << "Zero sensors in Route " << trainRoute->getId() << std::endl;
		}

		// 1.3 ������� �������
		auto sections = data["Sections"];
		// ������ ���������� �� �������, ������� ����� ������� � ���� 100 ��
		std::vector<Section*> Sections;
		if (!sections.is_null() && sections.is_array() && sections.size()) {
			for (const auto& section : sections) {
				std::string name = section["Name"];
				int axles = section["Axles"];
				Sections.push_back(new Section(name, axles)); // �������� ������ �������. ��������, �������� ���������� ���� �� �������
				json sensorsArray = section["Sensors"];
				if (!sensorsArray.is_null() && sensorsArray.is_array() && sensorsArray.size()) {
					for (const auto& sensor : sensorsArray) {
						int sensorID = sensor["SensorID"];
						std::string sign = sensor["Sign"];
						bool isIn = (sign == "+");
						// �������� ������ �� ������ �� �������� ID
						auto AxleSensorIterator = AxleSensors.begin();
						bool added = false;
						for (AxleSensorIterator = AxleSensors.begin(); AxleSensorIterator != AxleSensors.end(); ++AxleSensorIterator) {
							if ((*AxleSensorIterator)->getId() == sensorID) {
								Sections.back()->addSensor((*AxleSensorIterator), isIn); // ���������� �������� � ������
								added = true;
								break;
							}
						}
						if (!added) {
							std::cerr << "Sensor " << sensorID << " in Section " << name << " not found " << std::endl;
						}
					}
				}
				else {
					std::cerr << "Failed to access 'Sections - Sensors' field or it is not an array." << std::endl;
				}
			}
		}
		else {
			std::cerr << "Failed to access 'Sections' field or it is not an array." << std::endl;
		}

		// ��������, ��� � ������ ���� ���� �� ��� �������
		for (const auto& section : Sections) {
			if ((section->getSensors()).size() < 2)
				std::cerr << "Less than 2 sensors in section " << section->GetName() << std::endl;
		}

		// 1.4 ������� ������
		auto trains = data["Trains"];
		std::vector<Train*> Trains;
		if (!trains.is_null() && trains.is_array() && trains.size()) {
			for (const auto& train : trains) {
				int trainId = train["TrainID"];
				std::string dir = train["Direction"];
				int direction = 0;
				if (dir == "B")
					direction = 1;
				int axles = train["Axles"];
				int routeID = train["RouteID"];
				int speed = train["Speed"];
				// �������� ������ �� �������
				auto RouteIterator = TrainRoutes.begin();
				bool added = false;
				for (RouteIterator = TrainRoutes.begin(); RouteIterator != TrainRoutes.end(); ++RouteIterator) {
					if ((*RouteIterator)->getId() == routeID) {
						Trains.push_back(new Train(trainId, direction, axles, (*RouteIterator), speed)); // �������� ������ ������. id, �����������, ���������� ����, �������, �������� (��/�)
						added = true;
						break;
					}
				}
				if (!added) {
					std::cerr << "Not found rout " << routeID << " for Train " << trainId << std::endl;
				}
			}
		}
		else {
			std::cerr << "Failed to access 'Trains' field or it is not an array." << std::endl;
		}

		// 2. ���������
		// ��������� � ��������� ������ ������� Run_Cycle, ������� ���� 100 ��
		Cycle mainCycle(Sections);
		std::thread Cycle100(&Cycle::Run_Cycle, &mainCycle);

		// ���� ��������� ����� ��� ������ �����
		std::this_thread::sleep_for(std::chrono::seconds(1));

		std::vector<std::thread> threads; // ������ � ����������� ��������

		// �������� ����������� �������� �������
		auto runs = data["Run"];
		if (!runs.is_null() && runs.is_array() && runs.size()) {
			for (const auto& one_run : runs) {
				int trainID = one_run["TrainID"];
				int timeDelay = one_run["TimeDelay"];
				// �������� ������ �� �����
				auto TrainIterator = Trains.begin();
				bool added = false;
				for (TrainIterator = Trains.begin(); TrainIterator != Trains.end(); ++TrainIterator) {
					if ((*TrainIterator)->getId() == trainID) {
						threads.emplace_back(&Train::move, (*TrainIterator)); // ������ �������� ������
						std::this_thread::sleep_for(std::chrono::seconds(timeDelay));
						added = true;
						break;
					}
				}
				if (!added) {
					std::cerr << "Can`t run train " << trainID << std::endl;
				}
			}
		}
		else {
			std::cerr << "Failed to access 'Run' field or it is not an array." << std::endl;
		}

		// ��������� ���������� ������� �������
		for (auto& thread : threads) {
			thread.join();
		}

		// 3 ����� ��������� train.move() ���������� ����
		// �������� ������������ ������ ( � ��� ���� �������� ������� ��� ������������ )
		std::this_thread::sleep_for(std::chrono::seconds(4));
		mainCycle.Stop();
		Cycle100.join();

	} // ����� �� ������� ���������, ����������� ��������

	// 4
	/*
	if (errors.size()) {
		std::cout << "****** ERRORS! ******" << std::endl;
		for (auto e : errors) {
			std::cout << e << std::endl;
		}
		std::cout << "*********************" << std::endl;
	}
	*/

	std::cout.rdbuf(consoleBuffer);
	outputFile.close();

	return 0;
}