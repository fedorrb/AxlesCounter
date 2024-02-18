#ifndef TRAIN_ROUTE_H
#define TRAIN_ROUTE_H
#include "Common.h"
#include "AxleSensor.h"

// �������
// ������� �� ������������������ ��������
// ��� ������� ������� ����������� ���������� � ������ �� ������ ��������
class TrainRoute {
public:
	TrainRoute(int id);

	void addSensor(AxleSensor* sensor, int section_length);

	const std::vector<std::pair<AxleSensor*, int>>& getSensors() const;

	int getId() const;

	bool getLock() const;

	void setLock(bool b);

	std::set<int> getSetSensors();

private:
	int id;
	std::vector<std::pair<AxleSensor*, int>> sensors;
	bool lock;
};

#endif