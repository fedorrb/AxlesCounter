#include "Cycle.h"

// ���������������� ������� ������, ��� ������� ����� ��������� ������� ����
Cycle::Cycle(std::vector<Section*> Sections) : sections(Sections) {
	stop = false;
}

// ���������� ����� ������ �����
// ���������� ����� (���) �������� ���� ��� ���� ������ AxleCount
// �������������� ����� �������� �� 100 �� � ������ ����������� �� ���������� ������� �������� ����
void Cycle::Run_Cycle() {
	constexpr int intervalMilliseconds = 100;
	while (true) {
		// ����� ������ �����
		auto startTime = std::chrono::steady_clock::now();
		// ���� �����, ����� ��� ������
		for (auto it = sections.begin(); it != sections.end(); ++it) {
			(*it)->AxleCount(); // ������� ���� �� �������
		}
		// ������������ ����� ��������
		auto endTime = std::chrono::steady_clock::now();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
		//std::cout << "TIME = " << elapsedTime << std::endl;
		int sleepTime = intervalMilliseconds - static_cast<int>(elapsedTime);
		if (sleepTime > 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
		}
		if (stop)
			break;
	}
}

void Cycle::Stop() {
	stop = true;
}