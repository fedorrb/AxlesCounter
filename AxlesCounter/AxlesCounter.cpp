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

		// ����������� ���������� ���������
		std::multimap<int, int> hostileRouts;
		for (auto it1 = TrainRoutes.begin(); it1 != TrainRoutes.end(); ++it1) {
			std::set<int> routeSensors1 = (*it1)->getSetSensors();
			for (auto it2 = TrainRoutes.begin(); it2 != TrainRoutes.end(); ++it2) {
				std::set<int> routeSensors2 = (*it2)->getSetSensors();
				int old_size = routeSensors2.size();
				for (int sensorId : routeSensors1) {
					routeSensors2.insert(sensorId); // ���� ������� ����������, �� ������������ ������� �� ���������
				}
				if (routeSensors1.size() + old_size != routeSensors2.size()) {
					hostileRouts.insert( std::make_pair((*it1)->getId(), (*it2)->getId()) );
				}
			}
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
						std::this_thread::sleep_for(std::chrono::seconds(timeDelay));
						// �������� �� ���������� �������
						// ����� ��������
						int routeId = (*TrainIterator)->getIdRoute();
						while (true) {
							bool hostile = false;
							// ��������� ���� �� ��� ���� ����������
							auto range = hostileRouts.equal_range(routeId);
							for (auto it = range.first; it != range.second; ++it) {
								for (auto route : TrainRoutes) {
									if (it->second == route->getId() && route->getLock()) {
										hostile = true; 	// ���������� � ���������
										break;
									}
								}
							}
							if (hostile)
								std::this_thread::sleep_for(std::chrono::seconds(1)); // ���� ����������, �� ���� � ��������� �����
							else
								break;
						}
						// end �������� �� ������������
						// "��������" �������, ���������� ��� ���������� ������
						// �����, ������ ��� ����� �� �������� ������ ������ ������� �������� � ������ �����
						for (auto route : TrainRoutes) {
							if (routeId == route->getId()) {
								route->setLock(true);
								break;
							}
						}
						threads.emplace_back(&Train::move, (*TrainIterator)); // ������ �������� ������
						threads.back().detach();
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
		while (true) {
			bool running = false;
			std::this_thread::sleep_for(std::chrono::seconds(1));
			for (auto& train : Trains) {
				if (train->getIsMove())
					running = true;
			}
			if (!running)
				break;
		}

		// 3 ����� ��������� train.move() ���������� ����
		// �������� ������������ ������ ( � ��� ���� �������� ������� ��� ������������ )
		std::this_thread::sleep_for(std::chrono::seconds(4));
		mainCycle.Stop();
		Cycle100.join();

		// ����������� ��������
		for (auto train : Trains) {
			delete train;
		}
		Trains.clear();

		for (auto section : Sections) {
			delete section;
		}
		Sections.clear();

		for (auto trainRoute : TrainRoutes) {
			delete trainRoute;
		}
		TrainRoutes.clear();

		for (auto sensor : AxleSensors) {
			delete sensor;
		}
		AxleSensors.clear();

	} // ����� �� ������� ���������, ����������� ��������

	std::cout.rdbuf(consoleBuffer);
	outputFile.close();

	return 0;
}