#include "Train.h"
#include "Math15.h"

Train::Train(int id, int direction, int axlesCount, TrainRoute* route, int speed)
	: id(id), direction(direction), axlesCount(axlesCount), route(route), speed(speed) {
	sensor_delay = 0;
	speed_m_s = 0;
	CalcAxlesDelay();
}

void Train::move() {
	isMove = true;
	cout_mutex.lock();
	std::cout << "******************************" << std::endl;
	std::cout << "Start simulation route: " << route->getId() << ". Train axles = " << axlesCount << ", direction to " << (direction == 0 ? "A" : "B") << std::endl;
	cout_mutex.unlock();

	std::vector<unsigned short> sensorsCount;

	std::vector<std::thread> threads;

	for (const auto& sensorDelayPair : route->getSensors()) {
		AxleSensor* sensor = sensorDelayPair.first;
		// �������� �� ��������� ����� ������� (����� ����������� ������, ����� ������� �� �������)
		// second - ����� ������ � ������
		// �������� � �����������
		int delay = round(double(sensorDelayPair.second) / speed_m_s * 1000);
		// ��������� �������� ��������� ��������
		sensorsCount.push_back(sensor->getCount());
		// �������� ������� ��� ������� �������
		threads.emplace_back(&AxleSensor::generateSensorSignal, sensor, axlesCount, direction, delay, sensor_delay);
	}

	// ��������� ��� ������
	for (auto& thread : threads) {
		thread.join();
	}

	int vec_size = sensorsCount.size();
	if (vec_size) {
		int i = 0;
		unsigned short ttt = 0;
		for (const auto& sensorDelayPair : route->getSensors()) {
			AxleSensor* sensor = sensorDelayPair.first;
			// ��������� �������� ��������� ��������
			if (direction)
				ttt = Math15::Sub15(sensorsCount[i], axlesCount);
			else
				ttt = Math15::Add15(sensorsCount[i], axlesCount);

			if (sensor->getCount() != ttt) {
				std::cerr << "Sensor #" << sensor->getId() << " count error! Old value is:" << sensorsCount[i] << ", New values is:" << sensor->getCount() << ", Must be:" << ttt << std::endl;
			}
			i++;
			if (i >= vec_size)
				break;
		}
	}

	cout_mutex.lock();
	std::cout << "End simulation route: " << route->getId() << std::endl;
	std::cout << "******************************" << std::endl;
	cout_mutex.unlock();
	isMove = false;
}

int Train::getId() const
{
	return id;
}

bool Train::getIsMove() const
{
	return isMove;
}

void Train::CalcAxlesDelay() {
	speed_m_s = (double)speed * 10 / 36;
	sensor_delay = round(1000 / speed_m_s);// �����, �� ����� ����� ��������� 1�, �����������
}
