#include "AxleSensor.h"

// номер датчика, исходное значение счетчика
AxleSensor::AxleSensor(int id, int initialCount) : id(id), count(initialCount) {
	occupied = false; //свободна сенсорная система
	delays.clear();
	print();
}

AxleSensor::~AxleSensor()
{
	print();
}

// генерировать заданное количество сигналов (axles) от датчика
// запускается из потока
void AxleSensor::generateSensorSignal(int axles, int direction, int delay, int axle_delay)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(delay)); // задержка на включение датчика в работу
	int sensorOccupied = axle_delay / 4; //время нахождения колеса над сенсорной системой датчика; axle_delay задержка из расчёта 1 м, sensorOccupied - 25см
	delays.push_back(axle_delay * 2 - sensorOccupied); // 2 метра между осями
	delays.push_back(axle_delay * 13 - sensorOccupied); // 13 метров между тележками
	delays.push_back(axle_delay * 2 - sensorOccupied); // 2 метра между осями
	delays.push_back(axle_delay * 3 - sensorOccupied); // 3 метра между тележками соседних вагонов

	for (int i = 0, j = 0; i < axles; i++, j++) {
		// пока колесо находится над датчиком сенсорная система занята 
		// значение счетчика не меняется
		occupied = true;
		std::this_thread::sleep_for(std::chrono::milliseconds(sensorOccupied));
		occupied = false;
		// колесо проехало зону действия датчика
		// изменение значения счетчика
		if (direction == 0) {
			count = Math15::Add15(count, 1);
		}
		else {
			count = Math15::Sub15(count, 1);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(delays[j % 4])); // задержка на проход оси
	}
}

int AxleSensor::getId() const { return id; }

bool AxleSensor::getOccupied() const { return occupied; }

unsigned short AxleSensor::getCount() const { return count; }

void AxleSensor::print()
{
	std::cout << "Sensor " << id << ":\t count = " << count << std::endl;
}
