#include "util.h"
#include <stdexcept>
#include <algorithm>
using namespace nlp;

Number::Number(int num, int base, int digits) : base(base), decimal(num), digits(digits)
{
	if (base <= 1 || digits <= 0)
		throw std::invalid_argument("base error");
	else
		set_numvec(decimal_to_base(num));
}

Number::Number(int num, int base)
{
	Number(num, base, 1);
}

Number::Number(int num)
{
	Number(num, 10);
}

Number::Number()
{
	Number(0);
}

int Number::operator++ ()
{
	++decimal;
	set_numvec(decimal_to_base(decimal));
	return decimal;
}

int Number::operator++ (int)
{
	int copy_of_decimal = decimal;
	++decimal;
	set_numvec(decimal_to_base(decimal));
	return copy_of_decimal;
}

void Number::operator=	(int num)
{
	decimal = num;
	set_numvec(decimal_to_base(decimal));
}

const int Number::operator[] (unsigned int i) const
{
	if (i < 0)
		throw std::invalid_argument("invalid index");
	else if (i >= numvec.size())
		return 0;
	else
		return numvec[i];
}

int Number::to_decimal()
{
	return decimal;
}

void Number::set_numvec(std::vector<int> vec)
{
	numvec = vec;
	if (vec.size() > digits)
		digits = vec.size();
}

void Number::set_digits(int digits)
{
	if (base <= 0)
		throw std::invalid_argument("base error");
	else
		digits = digits;
}

int	Number::get_digits()
{
	return digits;
}

std::vector<int> Number::decimal_to_base(int num)
{
	std::vector<int> nv;
	while (num > 0)
	{
		int rem = num % base;
		nv.push_back(rem);
		num /= base;
	}

	if (nv.size() < digits)
	{
		int diff = digits - nv.size();
		for (int i = 0; i < diff; i++)
			nv.push_back(0);
	}

	std::reverse(nv.begin(), nv.end());
	return nv;
}