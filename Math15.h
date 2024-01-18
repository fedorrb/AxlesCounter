#ifndef MATH15
#define MATH15
#include <type_traits>
// ����������� � ��������� ����� ��� ������������
class Math15
{
public:

	// ������� �������� ��� 15 ������� ������������ ������ ( 0 - 32767 )
	static unsigned short Add15(unsigned short a, unsigned short b) {
		unsigned short const max = 32767;
		unsigned short d = 0;
		unsigned short c = 0;

		d = max - a;
		if (d >= b) {
			c = a + b;
		}
		else {
			// ���� ������ ��������� ������ ������� 32767 � ������� ���������� ��
			// �� ������� ���������� �������� ������� � �������� 1
			// |       32767         |
			//               |   d   |   c   |
			// 
			// 0-------------a-------*-------*		�������� ������ � �������: 0, a, 32767, a+b
			// 
			// |     a       |       b       | 

			c = b - d - 1;
		}

		return c;
	}

	// ������� ��������� ��� 15 ������� ������������ ������ ( 0 - 32767 )
	static unsigned short Sub15(unsigned short a, unsigned short b) {
		unsigned short const max = 32767;
		unsigned short d = 0;
		unsigned short c = 0;

		if (a >= b) {
			// 0------------------(32767)
			// 0--------------a
			// 0----b
			c = a - b;
		}
		else {
			// 0------------------(32767)
			// 0----a
			// 0--------------b
			//          0--------d
			// 0-------c
			d = b - a;
			c = max - d + 1;
		}

		return c;
	}

	// �������������� ����������:

	template <typename T>
	static typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, T>::type
		Add(T a, T b, T max) {
		T d = 0;
		T c = 0;

		d = max - a;
		if (d >= b) {
			c = a + b;
		}
		else {
			c = b - d - 1;
		}

		return c;
	}

	static unsigned short Add15_2(unsigned short a, unsigned short b) {
		unsigned short const max = 32767;
		unsigned short d = max - a;
		unsigned short c = a + (d >= b) * b + (b - d - 1) * (d < b);
		return c;
	}

};
#endif