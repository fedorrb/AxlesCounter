#ifndef TRAIN_H
#define TRAIN_H
#include "Common.h"
#include "AxleSensor.h"
#include "TrainRoute.h"

extern std::mutex cout_mutex; // ������� ��� ������ � �������� ����� �� ������ �������

// �����
// ������� �� ���������� �������� route, � ��������� ����������� direction, � ��������� ��������� speed(��/�)
// �������� axlesCount ���������� ����
// 
// ������� CalcAxlesDelay() ������������ ����� � ������������, �� ������� ����� ������� 1 ����
//
// ������� move() ��������� ������ ������ ��� ���������
// ��� ������� ������� ��������� � ������� �������������� �����, �� ������� ����� ������ �� �������
// ( � �������� ���� �������� ������ � ���������� �� ������� �� ������ ��������, � ������ ���� �������� )
// �������� ��������� ����� ��� ������� �������, � ������� ���������� ������� ������� generateSensorSignal
// ���������� ���������: ���������� ���� ������; �����������; ����� �� ������� ����� ������ �� �������; ����� �� ������� ����� ������� 1 ����
// ����� �������� ���� ������� ��������� �� ����������
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
	TrainRoute* route; //�������
	int speed; //km/h
	int sensor_delay;
	double speed_m_s;
	bool isMove;

	void CalcAxlesDelay();
};


#endif