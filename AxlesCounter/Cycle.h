#ifndef CYCLE_H
#define CYCLE_H
#include "Common.h"
#include "Section.h"

// ���� 100 ��
// �������� ����� 100 �� �� �������� ���������� ���
class Cycle {
public:
	Cycle(std::vector<Section*> Sections);
	void Run_Cycle();
	void Stop();
private:
	bool stop;
	std::vector<Section*> sections;
};

#endif