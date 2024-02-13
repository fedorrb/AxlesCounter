#include "Train.h"
Train::Train(int id, int direction, int axlesCount, TrainRoute* route, int speed)
	: id(id), direction(direction), axlesCount(axlesCount), route(route), speed(speed) {
	sensor_delay = 0;
	speed_m_s = 0;
	CalcAxlesDelay();
}

void Train::move() {
	cout_mutex.lock();
	std::cout << "******************************" << std::endl;
	std::cout << "Start simulation route: " << route->getId() << ". Train axles = " << axlesCount << ", direction to " << (direction == 0 ? "A" : "B") << std::endl;
	cout_mutex.unlock();

	std::vector<std::thread> threads;

	for (const auto& sensorDelayPair : route->getSensors()) {
		AxleSensor* sensor = sensorDelayPair.first;
		// задержка на включение счета датчика (время необходимое поезду, чтобы доехать до датчика)
		// second - длина секции в метрах
		// пересчет в милисекунды
		int delay = round(double(sensorDelayPair.second) / speed_m_s * 1000);

		// создание потоков для каждого датчика
		threads.emplace_back(&AxleSensor::generateSensorSignal, sensor, axlesCount, direction, delay, sensor_delay);
	}

	// подождать все потоки
	for (auto& thread : threads) {
		thread.join();
	}
	cout_mutex.lock();
	std::cout << "End simulation route: " << route->getId() << std::endl;
	std::cout << "******************************" << std::endl;
	cout_mutex.unlock();
}

int Train::getId() const
{
	return id;
}

void Train::CalcAxlesDelay() {
	speed_m_s = (double)speed * 10 / 36;
	sensor_delay = round(1000 / speed_m_s);// время, за котое поезд проезжает 1м, милисекунды
}
