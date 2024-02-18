#include "common.h"
#include "AxleSensor.h"
#include "Section.h"
#include "TrainRoute.h"
#include "Train.h"
#include "Cycle.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/*
*  назначение классов
*  Датчик - напольное оборудование
*  Маршрут и Поезд - вспомогательные классы для запуска датчика в работу
*  Цикл - цикл 100 мс в котором вызывается ДФБ
*  Секция - ДФБ
*/

std::mutex cout_mutex; // семафор для вывода в выходной поток из разных потоков

int main(int argc, char** argv) {

	std::string JSONFileName = "AxlesCounter.json";
	if (argc > 1) {
		JSONFileName = argv[1];
	}
	std::cout << "Loading configuration from " << JSONFileName << std::endl;
	if(argc == 1)
		std::cout << "To read the configuration from another file, pass it as a parameter" << std::endl;
	std::cout << "Working..." << std::endl;

	// файл журнала
	std::ofstream outputFile("output.txt");
	if (!outputFile.is_open()) {
		std::cerr << "Error opening the output file!" << std::endl;
		return 1;
	}
	std::streambuf* consoleBuffer = std::cout.rdbuf(); // Save the console buffer
	std::cout.rdbuf(outputFile.rdbuf()); // перенаправление std::cout в файл

	{
		// Відкриття файлу JSON для зчитування
		std::ifstream file(JSONFileName);
		// Перевірка, чи вдалося відкрити файл
		if (!file.is_open()) {
			std::cerr << "Could not open the file " << JSONFileName << std::endl;
			return 1;
		}
		// Зчитування даних з файлу JSON
		json data;
		try {
			file >> data;
		}
		catch (...) {
			std::cerr << "Error in JSON file!" << std::endl;
			return 1;
		}

		// 1 Подготовка
		// 1.1 Создаем датчики
		auto axle_sensors = data["AxleSensors"];
		// вектор указателей на Сенсоры
		std::vector<AxleSensor*> AxleSensors;

		if (!axle_sensors.is_null() && axle_sensors.is_array()) {
			for (const auto& sensor : axle_sensors) {
				int sensorID = sensor["SensorID"];
				int initValue = sensor["InitValue"];
				AxleSensors.push_back(new AxleSensor(sensorID, initValue)); 		// название, исходное значение
			}
		}
		else {
			std::cerr << "Failed to access 'AxleSensors' field or it is not an array." << std::endl;
		}

		// 1.2 Создаем маршруты
		auto routes = data["Routes"];
		// вектор указателей на Маршруты
		std::vector<TrainRoute*> TrainRoutes;
		if (!routes.is_null() && routes.is_array() && routes.size()) {
			for (const auto& route : routes) {
				int routeID = route["RouteID"];
				TrainRoutes.push_back(new TrainRoute(routeID)); 	// создается пустой маршрут
				// добавление сенсоров в маршрут
				json sensorsArray = route["Sensors"];
				if (!sensorsArray.is_null() && sensorsArray.is_array() && sensorsArray.size()) {
					for (const auto& sensor : sensorsArray) {
						int sensorID = sensor["SensorID"];
						int len = sensor["Length"];
						// получить ссылку на сенсор со считаным ID
						auto AxleSensorIterator = AxleSensors.begin();
						bool added = false;
						for (AxleSensorIterator = AxleSensors.begin(); AxleSensorIterator != AxleSensors.end(); ++AxleSensorIterator) {
							if ((*AxleSensorIterator)->getId() == sensorID) {
								TrainRoutes.back()->addSensor((*AxleSensorIterator), len); // добавление сенсоров в маршрут
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

		// проверка, что в маршруте есть хотя бы один сенсор
		for (const auto& trainRoute : TrainRoutes) {
			if((trainRoute->getSensors()).size() == 0)
				std::cerr << "Zero sensors in Route " << trainRoute->getId() << std::endl;
		}

		// определение враждебных маршрутов
		std::multimap<int, int> hostileRouts;
		for (auto it1 = TrainRoutes.begin(); it1 != TrainRoutes.end(); ++it1) {
			std::set<int> routeSensors1 = (*it1)->getSetSensors();
			for (auto it2 = TrainRoutes.begin(); it2 != TrainRoutes.end(); ++it2) {
				std::set<int> routeSensors2 = (*it2)->getSetSensors();
				int old_size = routeSensors2.size();
				for (int sensorId : routeSensors1) {
					routeSensors2.insert(sensorId); // если маршрут враждебный, то существующие сенсоры не добавятся
				}
				if (routeSensors1.size() + old_size != routeSensors2.size()) {
					hostileRouts.insert( std::make_pair((*it1)->getId(), (*it2)->getId()) );
				}
			}
		}

		// 1.3 Создаем участки
		auto sections = data["Sections"];
		// вектор указателей на Участки, который будет передан в цикл 100 мс
		std::vector<Section*> Sections;
		if (!sections.is_null() && sections.is_array() && sections.size()) {
			for (const auto& section : sections) {
				std::string name = section["Name"];
				int axles = section["Axles"];
				Sections.push_back(new Section(name, axles)); // создание нового участка. название, исходное количество осей на участке
				json sensorsArray = section["Sensors"];
				if (!sensorsArray.is_null() && sensorsArray.is_array() && sensorsArray.size()) {
					for (const auto& sensor : sensorsArray) {
						int sensorID = sensor["SensorID"];
						std::string sign = sensor["Sign"];
						bool isIn = (sign == "+");
						// получить ссылку на сенсор со считаным ID
						auto AxleSensorIterator = AxleSensors.begin();
						bool added = false;
						for (AxleSensorIterator = AxleSensors.begin(); AxleSensorIterator != AxleSensors.end(); ++AxleSensorIterator) {
							if ((*AxleSensorIterator)->getId() == sensorID) {
								Sections.back()->addSensor((*AxleSensorIterator), isIn); // добавление сенсоров в секцию
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

		// проверка, что в секции есть хотя бы два сенсора
		for (const auto& section : Sections) {
			if ((section->getSensors()).size() < 2)
				std::cerr << "Less than 2 sensors in section " << section->GetName() << std::endl;
		}

		// 1.4 Создаем поезда
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
				// получить ссылку на маршрут
				auto RouteIterator = TrainRoutes.begin();
				bool added = false;
				for (RouteIterator = TrainRoutes.begin(); RouteIterator != TrainRoutes.end(); ++RouteIterator) {
					if ((*RouteIterator)->getId() == routeID) {
						Trains.push_back(new Train(trainId, direction, axles, (*RouteIterator), speed)); // создание нового поезда. id, направление, количество осей, маршрут, скорость (км/ч)
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

		// 2. Симуляция
		// запустить в отдельном потоке функцию Run_Cycle, главный цикл 100 мс
		Cycle mainCycle(Sections);
		std::thread Cycle100(&Cycle::Run_Cycle, &mainCycle);

		// дать некоторое время для старта цикла
		std::this_thread::sleep_for(std::chrono::seconds(1));

		std::vector<std::thread> threads; // потоки с запущенными поездами

		// получить очередность движения поездов
		auto runs = data["Run"];
		if (!runs.is_null() && runs.is_array() && runs.size()) {
			for (const auto& one_run : runs) {
				int trainID = one_run["TrainID"];
				int timeDelay = one_run["TimeDelay"];
				// получить ссылку на поезд
				auto TrainIterator = Trains.begin();
				bool added = false;
				for (TrainIterator = Trains.begin(); TrainIterator != Trains.end(); ++TrainIterator) {
					if ((*TrainIterator)->getId() == trainID) {
						std::this_thread::sleep_for(std::chrono::seconds(timeDelay));
						// проверка на враждебный маршрут
						// номер маршрута
						int routeId = (*TrainIterator)->getIdRoute();
						while (true) {
							bool hostile = false;
							// проверить есть ли для него враждебные
							auto range = hostileRouts.equal_range(routeId);
							for (auto it = range.first; it != range.second; ++it) {
								for (auto route : TrainRoutes) {
									if (it->second == route->getId() && route->getLock()) {
										hostile = true; 	// враждебный и замкнутый
										break;
									}
								}
							}
							if (hostile)
								std::this_thread::sleep_for(std::chrono::seconds(1)); // если враждебный, то ждем и проверяем снова
							else
								break;
						}
						// end проверка на враждебность
						// "замкнуть" маршрут, размыкание при завершении потока
						// здесь, потому что время на создание потока больше времени итерации в данном цикле
						for (auto route : TrainRoutes) {
							if (routeId == route->getId()) {
								route->setLock(true);
								break;
							}
						}
						threads.emplace_back(&Train::move, (*TrainIterator)); // начать движение поезда
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

		// дождаться завершения проезда поездов
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

		// 3 после отработки train.move() остановить цикл
		// ожидание освобождения секций ( у них есть выдержка времени для освобождения )
		std::this_thread::sleep_for(std::chrono::seconds(4));
		mainCycle.Stop();
		Cycle100.join();

		// уничтожение объектов
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

	} // выход за область видимости, уничтожение объектов

	std::cout.rdbuf(consoleBuffer);
	outputFile.close();

	return 0;
}