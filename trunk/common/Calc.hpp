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
 * \date February 2012 
 * \author Matthieu Dorier
 * \version 0.4
 * 
 * This file defines the Calc structure, which is used to
 * compute list of values from a string representation of a Layout
 * (comma-separated list of arithmetical expressions).
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
 * The "?" string can also be parser and returns the array [-1].
 */
template <typename Iterator, typename SymTable>
struct Calc : qi::grammar<Iterator, std::vector<int>(), ascii::space_type>
{
	/**
	 * \brief Constructor.
	 * \param[in] sym : table of symboles.
	 */
	Calc(SymTable &sym) : Calc::base_type(list_expr)
	{
			qi::_val_type _val;
			qi::_1_type _1;
			qi::uint_type uint_;

			qmark   = qi::char_('?')[qi::_val = -1];

			list_expr = qmark 
						| (expression % ',');

			expression =
					term                            [_val = _1]
					>> *(   ('+' >> term            [_val += _1])
						|   ('-' >> term            [_val -= _1])
						)
			;

			term =
					factor                          [_val = _1]
					>> *(   ('*' >> factor          [_val *= _1])
						|   ('/' >> factor          [_val /= _1])
					)
			;

			factor =
					value(boost::phoenix::ref(sym)) [_val = _1]
					|   uint_                       [_val = _1]
					|   '(' >> expression           [_val = _1] >> ')'
					|   ('-' >> factor              [_val = -_1])
					|   ('+' >> factor              [_val = _1])
			;

			identifier = qi::lexeme[( qi::alpha | '_') >> *( qi::alnum | '_')];

			value   = identifier[_val = qi::labels::_r1[_1]];
        }

		qi::rule<Iterator, std::string(), ascii::space_type> identifier;
		qi::rule<Iterator, int(), ascii::space_type> expression, term, factor;
		qi::rule<Iterator, int(SymTable), ascii::space_type> value;
		qi::rule<Iterator, std::vector<int>(), ascii::space_type> list_expr;
		qi::rule<Iterator, int(), ascii::space_type> qmark;
};

}

#endif
