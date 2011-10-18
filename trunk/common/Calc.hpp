/*******************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
/**
 * \file Calc.hpp
 * \date September 2011
 * \author Matthieu Dorier
 * \version 0.3
 * 
 * This file defines the Calc structure, which is used to
 * compute list of values from a string representation of a Layout.
 * It uses a Boost.Spirit parser.
 */
#ifndef __DAMARIS_CALC_H
#define __DAMARIS_CALC_H

#include <boost/bind.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

namespace sp = boost::spirit;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

namespace Damaris {
/**
 * Calc is a structure containing Boost.Spirit parser elements
 * required to compute a list of values from a string-based
 * representation of a Layout. The input is a comma-separated
 * list of arythmetic expressions using +, -, *, /, % and
 * identifier whose value is defined in a SymTable mapping.
 */
template <typename Iterator, typename SymTable>
struct Calc : qi::grammar<Iterator, std::vector<int>(), ascii::space_type>
{
	qi::rule<Iterator, std::vector<int>(), ascii::space_type> start;
	qi::rule<Iterator, int(), ascii::space_type> expr;
	qi::rule<Iterator, int(), ascii::space_type> factor;
	qi::rule<Iterator, int(), ascii::space_type> simple;
	qi::rule<Iterator, std::string(), ascii::space_type> identifier;
	qi::rule<Iterator, int(SymTable), ascii::space_type> value;
	
	Calc(SymTable &sym) : Calc::base_type(start)
	{
		identifier = qi::lexeme[( qi::alpha | '_') >> *( qi::alnum | '_')];
		
		value 	= identifier[qi::_val = qi::labels::_r1[qi::_1]];
		
		simple 	= ('(' >> expr >> ')')
			| qi::int_
			| value(boost::phoenix::ref(sym));

		factor 	%= (simple >> '*' >> factor)[qi::_val = qi::_1 * qi::_2]
			|  (simple >> '/' >> factor)[qi::_val = qi::_1 / qi::_2]
			|  (simple >> '%' >> factor)[qi::_val = qi::_1 % qi::_2]
			|   simple;

		expr 	%= (factor >> '+' >> expr)[qi::_val = qi::_1 + qi::_2]
			|  (factor >> '-' >> expr)[qi::_val = qi::_1 - qi::_2]
			|   factor;

		start = (expr % ',');
	}
};

}
#endif
