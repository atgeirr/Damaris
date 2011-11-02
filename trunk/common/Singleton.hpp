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
 * \file Singleton.hpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#ifndef __DAMARIS_SINGLETON_H
#define __DAMARIS_SINGLETON_H

#include "common/Debug.hpp"

namespace Damaris {

template <typename T>
	class Singleton
	{
		protected:
			Singleton () { }
			~Singleton () { }
		public:
			static T *getInstance ()
			{
				if (NULL == _instance)
				{
					_instance = new T;
				}
				else
				{
					WARN("Singleton object already initialized");
				}
				return (static_cast<T*> (_instance));
			}

			static void kill ()
			{
				if (NULL != _instance)
				{
					delete _instance;
					_instance = NULL;
				}
			}

		private:
			static T *_instance;
	};

	template <typename T>
		T *Singleton<T>::_instance = NULL;

}
#endif
