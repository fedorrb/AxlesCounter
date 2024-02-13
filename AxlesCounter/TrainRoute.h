#ifndef TRAIN_ROUTE_H
#define TRAIN_ROUTE_H
#include "Common.h"
#include "AxleSensor.h"

// Маршрут
// состоит из последовательности датчиков
// для каждого датчика указывается расстояние в метрах от начала маршрута
class TrainRoute {
public:
	TrainRoute(int id);

	void addSensor(AxleSensor* sensor, int section_length);

	const std::vector<std::pair<AxleSensor*, int>>& getSensors() const;

	int getId() const;

private:
	int id;
	std::vector<std::pair<AxleSensor*, int>> sensors;
};

#endif