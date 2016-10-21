/**************************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#ifndef __DAMARIS_CALC_H
#define __DAMARIS_CALC_H

#include <boost/bind.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

namespace sp = boost::spirit;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

namespace damaris {
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
	SymTable sym; /*!< Table of symboles to access parameters. */
	
	/**
	 * Constructor.
	 * 
	 * \param[in] sym : table of symboles.
	 */
	Calc(const SymTable &s) : Calc::base_type(list_expr), sym(s)
	{
			qi::_val_type _val;
			qi::_1_type _1;
			qi::uint_type uint_;

			list_expr = (expression % ',');

			expression =
				term				[_val = _1]
				>> *(   ('+' >> term		[_val += _1])
					|   ('-' >> term	[_val -= _1])
				    )
			;

			term =
				factor				[_val = _1]
				>> *(   ('*' >> factor		[_val *= _1])
					|   ('/' >> factor	[_val /= _1])
				    )
			;

			factor =
				value(boost::phoenix::ref(sym)) [_val = _1]
				|   uint_                       [_val = _1]
				|   '(' >> expression           [_val = _1]>>')'
				|   ('-' >> factor              [_val = -_1])
				|   ('+' >> factor              [_val = _1])
			;

			identifier = qi::lexeme[( qi::alpha | qi::char_("_")) 
					>> *( qi::alnum | qi::char_("_"))];

			value   = identifier[_val = qi::labels::_r1[_1]];
        }

	qi::rule<Iterator, std::string(), ascii::space_type> identifier;
	qi::rule<Iterator, int(), ascii::space_type> expression, term, factor;
	qi::rule<Iterator, int(SymTable), ascii::space_type> value;
	qi::rule<Iterator, std::vector<int>(), ascii::space_type> list_expr;
};

}

#endif
