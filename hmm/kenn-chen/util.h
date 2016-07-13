#pragma once
#include <vector>
namespace nlp
{
	class Number
	{
	private:
		int					base;
		int					decimal;
		int					digits;
		std::vector<int>	numvec;

		std::vector<int>	decimal_to_base(int num);
		void				set_numvec(std::vector<int> vec);

	public:
					Number();
					Number(int num);
					Number(int num, int base);
					Number(int num, int base, int digits);
		int			to_decimal();
		void		set_digits(int digits);
		int			get_digits();
		int			operator++	();
		int			operator++	(int);
		void		operator=	(int num);
		const int	operator[] (unsigned int i) const;

	};
}