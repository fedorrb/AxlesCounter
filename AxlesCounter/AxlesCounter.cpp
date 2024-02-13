#include "common.h"
#include "AxleSensor.h"
#include "Section.h"
#include "TrainRoute.h"
#include "Train.h"
#include "Cycle.h"

/*
*  назначение классов
*  Датчик - напольное оборудование
*  Маршрут и Поезд - вспомогательные классы для запуска датчика в работу
*  Цикл - цикл 100 мс в котором вызывается ДФБ
*  Секция - ДФБ
*/

std::mutex cout_mutex; // семафор для вывода в выходной поток из разных потоков

int main() {
	std::vector<std::string> errors;
	errors.clear();
	std::cout << "Working..." << std::endl;
	// файл журнала
	std::ofstream outputFile("output.txt");
	if (!outputFile.is_open()) {
		std::cerr << "Error opening the output file!" << std::endl;
		return 1;
	}
	std::streambuf* consoleBuffer = std::cout.rdbuf(); // Save the console buffer
	std::cout.rdbuf(outputFile.rdbuf()); // перенаправление std::cout в файл

	{
		// 1 Подготовка
		// 1.1 Создаем датчики
		// название, исходное значение
		AxleSensor sensor1{ 1, 32760 };
		AxleSensor sensor2(2, 0);
		AxleSensor sensor3(3, 0);
		AxleSensor sensor4(4, 32761);
		AxleSensor sensor5(5, 32727);
		AxleSensor sensor6(6, 0);
		AxleSensor sensor7(7, 0);
		AxleSensor sensor8(8, 0);
		AxleSensor sensor9(9, 0);
		AxleSensor sensor10(10, 0);
		AxleSensor sensor11(11, 0);
		AxleSensor sensor12(12, 0);
		AxleSensor sensor13(13, 32750);
		AxleSensor sensor14(14, 0);
		AxleSensor sensor15(15, 0);
		AxleSensor sensor16(16, 32760);
		AxleSensor sensor17(17, 0);
		AxleSensor sensor18(18, 0);
		AxleSensor sensor19(19, 32761);
		AxleSensor sensor20(20, 0);
		AxleSensor sensor21(21, 33);
		AxleSensor sensor22(22, 0);
		AxleSensor sensor23(23, 0);
		AxleSensor sensor24(24, 1000);

		// 1.2 Создаем маршруты
		// номер
		TrainRoute route1(1);
		route1.addSensor(&sensor4, 0); //сенсор, расстояние от первого сенсора (начала маршрута) в метрах
		route1.addSensor(&sensor5, 120);
		route1.addSensor(&sensor13, 220);

		TrainRoute route2(2);
		route2.addSensor(&sensor13, 0);
		route2.addSensor(&sensor5, 100);
		route2.addSensor(&sensor3, 210);
		route2.addSensor(&sensor1, 260);

		TrainRoute route3(3);
		route3.addSensor(&sensor12, 0);
		route3.addSensor(&sensor9, 150);
		route3.addSensor(&sensor2, 300);
		route3.addSensor(&sensor1, 350);

		TrainRoute route4(4);
		route4.addSensor(&sensor22, 0);
		route4.addSensor(&sensor21, 50);
		route4.addSensor(&sensor24, 150);
		route4.addSensor(&sensor14, 200);

		// 1.3 Создаем участки
		// название, исходное количество осей на участке
		Section section_3_7SP("3_7SP", 0);
		// описание датчиков относящихся к участку
		// датчик, признак инкремента при условном направлении "А"
		section_3_7SP.addSensor(&sensor3, true);//+
		section_3_7SP.addSensor(&sensor4, true);//+
		section_3_7SP.addSensor(&sensor5, false);//-
		section_3_7SP.addSensor(&sensor6, false);//-
		section_3_7SP.addSensor(&sensor7, false);//-

		Section section_9SP("9SP", 0);
		section_9SP.addSensor(&sensor5, true);//+
		section_9SP.addSensor(&sensor10, false);//-
		section_9SP.addSensor(&sensor13, false);//-

		Section section_1SP("1SP", 0);
		section_1SP.addSensor(&sensor1, true);//+
		section_1SP.addSensor(&sensor2, false);//-
		section_1SP.addSensor(&sensor3, false);//-

		Section section_1_11P("1/11P", 0);
		section_1_11P.addSensor(&sensor2, true);//+
		section_1_11P.addSensor(&sensor9, false);//-

		Section section_11_13SP("11_13SP", 0);
		section_11_13SP.addSensor(&sensor9, true);//+
		section_11_13SP.addSensor(&sensor10, true);//+
		section_11_13SP.addSensor(&sensor11, false);//-
		section_11_13SP.addSensor(&sensor12, false);//-

		Section put1P("1P", 10);
		put1P.addSensor(&sensor13, true);//+
		put1P.addSensor(&sensor14, false);//-

		Section put2P("2P", 38);
		put2P.addSensor(&sensor12, true);//+
		put2P.addSensor(&sensor16, false);//-

		Section put3P("3P", 0);
		put3P.addSensor(&sensor6, true);//+
		put3P.addSensor(&sensor15, false);//-

		Section put4P("4P", 10);
		put4P.addSensor(&sensor7, true);//+
		put4P.addSensor(&sensor8, false);//-

		Section section_2SP("2SP", 0);
		section_2SP.addSensor(&sensor20, true);//+
		section_2SP.addSensor(&sensor21, true);//+
		section_2SP.addSensor(&sensor22, false);//-

		Section section_12SP("12SP", 0);
		section_12SP.addSensor(&sensor16, true);//+
		section_12SP.addSensor(&sensor17, false);//-
		section_12SP.addSensor(&sensor18, false);//-

		Section section_2_12P("2/12P", 0);
		section_2_12P.addSensor(&sensor18, true);//+
		section_2_12P.addSensor(&sensor20, false);//-

		Section section_4_6SP("4_6SP", 0);
		section_4_6SP.addSensor(&sensor24, true);//+
		section_4_6SP.addSensor(&sensor19, true);//+
		section_4_6SP.addSensor(&sensor21, false);//-
		section_4_6SP.addSensor(&sensor23, false);//-

		Section section_8SP("8SP", 0);
		section_8SP.addSensor(&sensor8, true);//+
		section_8SP.addSensor(&sensor15, true);//+
		section_8SP.addSensor(&sensor19, false);//-

		Section section_10SP("10SP", 0);
		section_10SP.addSensor(&sensor14, true);//+
		section_10SP.addSensor(&sensor17, true);//+
		section_10SP.addSensor(&sensor24, false);//-

		// вектор указателей на Участки, который будет передан в цикл 100 мс
		std::vector<Section*> Sections;
		Sections.push_back(&section_3_7SP);
		Sections.push_back(&section_9SP);
		Sections.push_back(&section_1SP);
		Sections.push_back(&section_1_11P);
		Sections.push_back(&section_11_13SP);
		Sections.push_back(&put1P);
		Sections.push_back(&put2P);
		Sections.push_back(&put3P);
		Sections.push_back(&put4P);
		Sections.push_back(&section_2SP);
		Sections.push_back(&section_12SP);
		Sections.push_back(&section_2_12P);
		Sections.push_back(&section_4_6SP);
		Sections.push_back(&section_8SP);
		Sections.push_back(&section_10SP);

		// 1.4 Создаем поезда
		// направление, количество осей, маршрут, скорость (км/ч)
		Train trainA(0, 44, &route1, 60);//0 - direction "A", 44 - axles, 60 km/h
		Train trainB(1, 6, &route2, 60);//1 - direction "B", 6 - axles, 60 km/h
		Train trainB3(1, 38, &route3, 60);// со 2-го пути в четном направлении
		Train trainB4(1, 46, &route4, 60);// на 1-й путь в четном направлении

		// 2. Симуляция
		// запустить в отдельном потоке функцию Run_Cycle, главный цикл 100 мс
		Cycle mainCycle(Sections);
		std::thread Cycle100(&Cycle::Run_Cycle, &mainCycle);

		// дать некоторое время для старта цикла
		std::this_thread::sleep_for(std::chrono::seconds(1));

		// 1-й,3-й,4-й поезда едут одновременно
		// начинают движение с небольшой случайной задержкой
		// в отдельном потоке вызываем train.move()
		// который создаст отдельный поток для каждого сенсора
		// маршруты поездов не пересекаются
		// два одновременных поезда на 1-й путь с разных перегонов
		// и один со 2-го пути на перегон
		std::thread trainMoveThread(&Train::move, &trainA);
		std::this_thread::sleep_for(std::chrono::milliseconds(generateRandomValue(100, 2000))); // перерыв между поездами
		std::thread trainMoveThreadB3(&Train::move, &trainB3);
		std::this_thread::sleep_for(std::chrono::milliseconds(generateRandomValue(100, 2000))); // перерыв между поездами
		std::thread trainMoveThreadB4(&Train::move, &trainB4);

		// дождаться завершения проезда поездов
		trainMoveThreadB3.join();
		trainMoveThreadB4.join();
		trainMoveThread.join();

		// проверки правильности счета после первых 3-х поездов (добавить проверки)
		if (put1P.getAxles() != 100)
			errors.push_back("Put 1P has no 100 axles");
		if (put2P.getAxles() != 0)
			errors.push_back("Put 2P has axles");
		if (sensor1.getCount() != 32722)
			errors.push_back("Sensor1 expects 32722");
		if (sensor2.getCount() != 32730)
			errors.push_back("Sensor2 expects 32730");
		if (sensor5.getCount() != 3)
			errors.push_back("Sensor5 expects 3");
		if (sensor13.getCount() != 26)
			errors.push_back("Sensor13 expects 26");
		if (sensor21.getCount() != 32755)
			errors.push_back("Sensor21 expects 32755");

		// 2-й поезд, с 1-го пути на перегон
		std::this_thread::sleep_for(std::chrono::seconds(3)); // перерыв между поездами
		// в отдельном потоке вызываем train.move()
		std::thread trainMoveThread2(&Train::move, &trainB);
		// дождаться завершения проезда поезда
		trainMoveThread2.join();

		// 3 после отработки train.move() остановить поток
		// ожидание освобождения секций ( у них есть выдержка времени для освобождения )
		std::this_thread::sleep_for(std::chrono::seconds(4));
		mainCycle.Stop();
		Cycle100.join();

		// проверки правильности счета
		if (put1P.getAxles() != 94)
			errors.push_back("Put 1P has no 94 axles");
		if (sensor1.getCount() != 32716)
			errors.push_back("Sensor1 expects 32716");
		if (sensor2.getCount() != 32730)
			errors.push_back("Sensor2 expects 32730");
		if (sensor5.getCount() != 32765)
			errors.push_back("Sensor5 expects 32765");
		if (sensor13.getCount() != 20)
			errors.push_back("Sensor13 expects 20");
		if (sensor21.getCount() != 32755)
			errors.push_back("Sensor21 expects 32755");

	} // выход за область видимости, уничтожение объектов

	// 4
	if (errors.size()) {
		std::cout << "****** ERRORS! ******" << std::endl;
		for (auto e : errors) {
			std::cout << e << std::endl;
		}
		std::cout << "*********************" << std::endl;
	}

	std::cout.rdbuf(consoleBuffer);
	outputFile.close();

	return 0;
}