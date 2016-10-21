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

#ifndef __DAMARIS_CALLBACK_H
#define __DAMARIS_CALLBACK_H

#include "damaris/util/ForwardDcl.hpp"
#include "damaris/util/Pointers.hpp"
#include "damaris/util/Deleter.hpp"

namespace damaris {

USING_POINTERS;

template<typename F>
class CallbackImpl;

/**
 * The Callback class is used to wrap a function that has to be called uppon
 * reception of a message of a certain tag in the Reactor. It is an abstract
 * class whose main child class is the CallbackImpl.
 */
class Callback {
	
	friend class Deleter<Callback>;

	protected:
	/**
	 * Constructor.
	 */
	Callback() {}
	
	/**
	 * Destructor.
	 */
	virtual ~Callback() {}
		
	public:
	
	/**
	 * Overloaded parenthesis operator, used to call the inner function.
	 *
	 * \param[in] tag : tag that initiated this callback.
	 * \param[in] source : source that triggered the callback.
	 * \param[in] buf : pointer to potential data sent by the caller.
	 * \param[in] count : number of bytes sent in the buffer.
	 */
	virtual void operator()(int tag, int source, 
		const void* buf, int count) = 0;
	
	/**
	 * Creates a callback from a function or object with overloaded ().
	 * The function (or object) should provide the () operator with
	 * the same prototype as the Callback's.
	 *
	 * \param[in] f : function or object to wrap.
	 */
	template<typename F>
	static shared_ptr<Callback> New(F f) 
	{
		return shared_ptr<Callback>(new CallbackImpl<F>(f),
					Deleter<Callback>());
	}
};

/**
 * CallbackImpl is an effective, generic implementation of Callback.
 */
template<typename F>
class CallbackImpl : public Callback {
	
	friend class Callback;
	friend class Deleter<CallbackImpl>;
		
	private:
	F function_; /*!< Wrapper object or function. */
	
	/**
	 * Constructor.
	 * 
	 * \param[in] f : function or object to wrap.
	 */
	CallbackImpl(F f) 
	: Callback(), function_(f) {}
	
	/**
	 * Destructor.
	 */
	virtual ~CallbackImpl() {}

	public:
		
	/**
	 * \see Callback::operator()
	 */
	virtual void operator()(int tag, int source, const void* buf, int count)
	{
		function_(tag,source,buf,count);
	}
};

}

#endif
