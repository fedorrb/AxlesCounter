#include "Common.h"
#include "Section.h"

//имя секции, начальное кол-во осей на секции
Section::Section(const std::string& name, int initialAxles) : name(name), axlesCount(initialAxles) {
	free = (axlesCount == 0);
	sumCounterBefore.store(0);
	sumCounterCurrent.store(0);
	axlesBefore.store(axlesCount.load());
	cycles = 30; // циклов ( для времени задержки 3 сек )
	countDown = cycles; // обратный отсчет времени до изменения состояния на "свободна", при отсутствии занятости сенсорной системы и отсутствии осей на секции
	print(2);
}

Section::~Section() {
	print(2);
}

// Добавление сенсора в секцию
void Section::addSensor(AxleSensor* sensor, bool add) {
	sensors.push_back(std::make_pair(sensor, add));
	InitBefore();
}

// по состоянию счетчиков ДПК определить кол-во осей на секции
void Section::AxleCount() {
	sumCounterCurrent = 0;
	axlesBefore.store(axlesCount.load()); //присваивание
	// проход по всем наблюдаемым сенсорам
	for (const auto& sensorInfo : sensors) {
		if (sensorInfo.second)
			sumCounterCurrent = Math15::Add15(sumCounterCurrent, sensorInfo.first->getCount());
		else
			sumCounterCurrent = Math15::Sub15(sumCounterCurrent, sensorInfo.first->getCount());
		// занятость
		if (sensorInfo.first->getOccupied()) {
			free = false;
			countDown = cycles;
		}
	}
	// подсчитать оси
	axlesCount = Math15::Sub15(sumCounterCurrent, sumCounterBefore);
	axlesCount = Math15::Add15(axlesCount, axlesBefore);
	sumCounterBefore.store(sumCounterCurrent.load()); //присваивание

	// свободность секции
	if (axlesCount) {
		free = false;
		countDown = cycles;
	}
	else {
		if (!countDown)
			free = true;
	}
	if (countDown)
		countDown--;

	if (!axlesCount && !free) //отслеживание занятия сенсорной системы до подсчета оси (скорость должна быть <= 3 км/ч)
		print(2);
	else
		print(1);
}

void Section::print(int p) {
	cout_mutex.lock(); // для разруливания параллельной записи из нескольких потоков

	std::cout << std::setw(10) << "Section "
		<< std::setw(8) << name
		<< std::setw(12) << " Axles = " << axlesCount << std::endl;
	if (p == 2) {
		std::cout << std::setw(10) << "Section "
			<< std::setw(8) << name
			<< std::setw(12) << " Free -  " << (free == true ? "Yes" : "No") << std::endl;
	}

	cout_mutex.unlock();
}

int Section::getAxles() const { return axlesCount; }

//инициализация после добавления в секцию датчика
void Section::InitBefore() {
	sumCounterBefore = 0;
	// проход по наблюдаемым сенсорам
	for (const auto& sensorInfo : sensors) {
		if (sensorInfo.second)
			sumCounterBefore = Math15::Add15(sumCounterBefore, sensorInfo.first->getCount());
		else
			sumCounterBefore = Math15::Sub15(sumCounterBefore, sensorInfo.first->getCount());
	}
}