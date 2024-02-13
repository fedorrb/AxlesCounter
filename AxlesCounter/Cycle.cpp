#include "Cycle.h"

// инициализируется списком секций, для которых нужно выполнять подсчет осей
Cycle::Cycle(std::vector<Section*> Sections) : sections(Sections) {
	stop = false;
}

// засекается время начала цикла
// вызывается метод (ДФБ) подсчета осей для всех секций AxleCount
// рассчитывается время ожидания до 100 мс с учетом затраченого на выполнение функций подсчета осей
void Cycle::Run_Cycle() {
	constexpr int intervalMilliseconds = 100;
	while (true) {
		// время начала цикла
		auto startTime = std::chrono::steady_clock::now();
		// тело цикла, вызов ДФБ секций
		for (auto it = sections.begin(); it != sections.end(); ++it) {
			(*it)->AxleCount(); // подсчет осей на участке
		}
		// Рассчитываем время ожидания
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