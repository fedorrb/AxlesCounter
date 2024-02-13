#ifndef CYCLE_H
#define CYCLE_H
#include "Common.h"
#include "Section.h"

// цикл 100 мс
// имитация цикла 100 мс из которого вызываются ДФБ
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