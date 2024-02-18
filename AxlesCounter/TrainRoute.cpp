#include "TrainRoute.h"

TrainRoute::TrainRoute(int id) : id(id) {
	lock = false;
}

void TrainRoute::addSensor(AxleSensor* sensor, int section_length) {
	sensors.push_back(std::make_pair(sensor, section_length));
}

const std::vector<std::pair<AxleSensor*, int>>& TrainRoute::getSensors() const {
	return sensors;
}

int TrainRoute::getId() const
{
    return id;
}

bool TrainRoute::getLock() const
{
	return lock;
}

void TrainRoute::setLock(bool b)
{
	lock = b;
}

std::set<int> TrainRoute::getSetSensors()
{
	std::set<int> listSensors;
	for (const auto& sensorLenPair : sensors) {
		AxleSensor* sensor = sensorLenPair.first;
		listSensors.insert(sensor->getId());
	}
	return listSensors;
}
