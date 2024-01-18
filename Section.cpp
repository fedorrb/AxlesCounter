#include "Common.h"
#include "Section.h"

//��� ������, ��������� ���-�� ���� �� ������
Section::Section(const std::string& name, int initialAxles) : name(name), axlesCount(initialAxles) {
	free = (axlesCount == 0);
	sumCounterBefore.store(0);
	sumCounterCurrent.store(0);
	axlesBefore.store(axlesCount.load());
	cycles = 30; // ������ ( ��� ������� �������� 3 ��� )
	countDown = cycles; // �������� ������ ������� �� ��������� ��������� �� "��������", ��� ���������� ��������� ��������� ������� � ���������� ���� �� ������
	print(2);
}

Section::~Section() {
	print(2);
}

// ���������� ������� � ������
void Section::addSensor(AxleSensor* sensor, bool add) {
	sensors.push_back(std::make_pair(sensor, add));
	InitBefore();
}

// �� ��������� ��������� ��� ���������� ���-�� ���� �� ������
void Section::AxleCount() {
	sumCounterCurrent = 0;
	axlesBefore.store(axlesCount.load()); //������������
	// ������ �� ���� ����������� ��������
	for (const auto& sensorInfo : sensors) {
		if (sensorInfo.second)
			sumCounterCurrent = Math15::Add15(sumCounterCurrent, sensorInfo.first->getCount());
		else
			sumCounterCurrent = Math15::Sub15(sumCounterCurrent, sensorInfo.first->getCount());
		// ���������
		if (sensorInfo.first->getOccupied()) {
			free = false;
			countDown = cycles;
		}
	}
	// ���������� ���
	axlesCount = Math15::Sub15(sumCounterCurrent, sumCounterBefore);
	axlesCount = Math15::Add15(axlesCount, axlesBefore);
	sumCounterBefore.store(sumCounterCurrent.load()); //������������

	// ����������� ������
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

	if (!axlesCount && !free) //������������ ������� ��������� ������� �� �������� ��� (�������� ������ ���� <= 3 ��/�)
		print(2);
	else
		print(1);
}

void Section::print(int p) {
	cout_mutex.lock(); // ��� ������������ ������������ ������ �� ���������� �������

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

//������������� ����� ���������� � ������ �������
void Section::InitBefore() {
	sumCounterBefore = 0;
	// ������ �� ����������� ��������
	for (const auto& sensorInfo : sensors) {
		if (sensorInfo.second)
			sumCounterBefore = Math15::Add15(sumCounterBefore, sensorInfo.first->getCount());
		else
			sumCounterBefore = Math15::Sub15(sumCounterBefore, sensorInfo.first->getCount());
	}
}