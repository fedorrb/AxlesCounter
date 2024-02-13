#include "TrainRoute.h"

TrainRoute::TrainRoute(int id) : id(id) {}

void TrainRoute::addSensor(AxleSensor* sensor, int section_length) {
	sensors.push_back(std::make_pair(sensor, section_length));
}

const std::vector<std::pair<AxleSensor*, int>>& TrainRoute::getSensors() const {
	return sensors;
}