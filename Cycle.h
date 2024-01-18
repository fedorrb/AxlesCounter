#ifndef CYCLE_H
#define CYCLE_H
#include "Common.h"
#include "Section.h"

// цикл 100 мс
// имитация цикла 100 мс из которого вызываются ДФБ
// инициализируется списком секций, для которых нужно выполнять подсчет осей
// засекается время начала цикла
// вызывается метод (ДФБ) подсчета осей для всех секций AxleCount
// рассчитывается время ожидания до 100 мс с учетом затраченого на выполнение функций подсчета осей
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