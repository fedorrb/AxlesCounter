#include "AxleSensor.h"

// ����� �������, �������� �������� ��������
AxleSensor::AxleSensor(int id, int initialCount) : id(id), count(initialCount) {
	occupied = false; //�������� ��������� �������
	delays.clear();
	print();
}

AxleSensor::~AxleSensor()
{
	print();
}

// ������������ �������� ���������� �������� (axles) �� �������
// ����������� �� ������
void AxleSensor::generateSensorSignal(int axles, int direction, int delay, int axle_delay)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(delay)); // �������� �� ��������� ������� � ������
	int sensorOccupied = axle_delay / 4; //����� ���������� ������ ��� ��������� �������� �������; axle_delay �������� �� ������� 1 �, sensorOccupied - 25��
	delays.push_back(axle_delay * 2 - sensorOccupied); // 2 ����� ����� �����
	delays.push_back(axle_delay * 13 - sensorOccupied); // 13 ������ ����� ���������
	delays.push_back(axle_delay * 2 - sensorOccupied); // 2 ����� ����� �����
	delays.push_back(axle_delay * 3 - sensorOccupied); // 3 ����� ����� ��������� �������� �������

	for (int i = 0, j = 0; i < axles; i++, j++) {
		// ���� ������ ��������� ��� �������� ��������� ������� ������ 
		// �������� �������� �� ��������
		occupied = true;
		std::this_thread::sleep_for(std::chrono::milliseconds(sensorOccupied));
		occupied = false;
		// ������ �������� ���� �������� �������
		// ��������� �������� ��������
		if (direction == 0) {
			count = Math15::Add15(count, 1);
		}
		else {
			count = Math15::Sub15(count, 1);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(delays[j % 4])); // �������� �� ������ ���
	}
}

int AxleSensor::getId() const { return id; }

bool AxleSensor::getOccupied() const { return occupied; }

unsigned short AxleSensor::getCount() const { return count; }

void AxleSensor::print()
{
	std::cout << "Sensor " << id << ":\t count = " << count << std::endl;
}
