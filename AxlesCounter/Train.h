#ifndef TRAIN_H
#define TRAIN_H
#include "Common.h"
#include "AxleSensor.h"
#include "TrainRoute.h"

extern std::mutex cout_mutex; // семафор для вывода в выходной поток из разных потоков

// Поезд
// следует по указанному маршруту route, в указанном направлении direction, с указанной скоростью speed(км/ч)
// содержит axlesCount количество осей
// 
// функция CalcAxlesDelay() рассчитывает время в милисекундах, за которое поезд проедет 1 метр
//
// функция move() имитирует проезд поезда над датчиками
// для каждого датчика входящего в маршрут рассчитывается время, за которое поезд доедет до датчика
// ( у маршрута есть свойство датчик и расстояние до датчика от начала маршрута, у поезда есть скорость )
// создаётся отдельный поток для каждого датчика, в котором вызывается функция датчика generateSensorSignal
// передаются параметры: количество осей поезда; направление; время за которое поезд доедет до датчика; время за которое поезд проедет 1 метр
// после создания всех потоков ожидается их завершение
class Train {
public:
	Train(int id, int direction, int axlesCount, TrainRoute* route, int speed);

	void move();

	int getId() const;
	bool getIsMove() const;

private:
	int id;
	int direction;
	int axlesCount;
	TrainRoute* route; //маршрут
	int speed; //km/h
	int sensor_delay;
	double speed_m_s;
	bool isMove;

	void CalcAxlesDelay();
};


#endif