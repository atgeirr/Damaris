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
#ifndef __DAMARIS_HDF5_STORE_H
#define __DAMARIS_HDF5_STORE_H

#include <stdint.h>
#include <string>

#include "storage/Store.hpp"
#include "hdf5.h"


namespace damaris {

/**
 * HDF5Store.hpp is a Store that writes HDF5 files, either file per process, 
 * or collective writes as single HDF5 file. Data to write is specified in
 * the Damaris XML file by specifying the name in the  <variable ... store="MyStore", />
 * tag where "MyStore" is the <store name="" > element
 */
class HDF5Store : public Store {

	friend class Deleter<HDF5Store>;	
	/**
	 * Constructor. 
	 */
	HDF5Store(const model::Store& mdl);
			
	/**
	 * Destructor.
	 */
	virtual ~HDF5Store() {}

private:
    enum FileMode {FilePerCore , FilePerNode , Collective, CollectiveTest};
    enum XdmfMode {NoIteration, FirstIteration, EveryIteration};

	FileMode fileMode_;
	XdmfMode xdmfMode_;
	std::string path_;

	/**
	* Updates the memory space and the dimentions according to ghost zones.
	*/
	bool UpdateGhostZones(std::shared_ptr<Variable> v , hid_t &memSpace , hsize_t* localDim);
	/**
    * Returns the full name of the variable, including the groups (separated by a "/")
    */
    std::string GetVariableFullName(std::shared_ptr<Variable> v , std::shared_ptr<Block> *b = NULL);
	/**
	* Returns the output file name of the result .h5 file.
	*/
    std::string GetOutputFileName(int32_t iteration);
    /**
    * This function gets a type from the model type and retuns its equivalient HDF5 type as the output parameter.
    * If the type could not be found, the return value is false, otherwise it is true.
    */
    bool GetHDF5Type(model::Type mdlType , hid_t& hdfType );

    /**
    * This function reads the related key-values from the store xml element and
    * initializes the corresponding values in the class.
    */
    bool ReadKeyValues(const model::Store& mdl);

   /**
	* This function writes the data of an iteration into a single HDF5 file using collective I/O.
	* Parallel HDF5 has been used as an I/O middleware here.
	* Collective I/O be used by specifying "CollectiveTest" in the Damaris
	* XML config file <store> section:
	*  <option key="FileMode">Collective</option>
	*/
	void OutputCollective(int32_t iteration);

   /**
	* This function writes the data of an iteration into a single HDF5 file using collective I/O.
	* Parallel HDF5 has been used as an I/O middleware here.
	* It is a to-be-tested version that can be used by specifying "CollectiveTest" in the Damaris
	* XML config file <store> section:
	*  <option key="FileMode">CollectiveTest</option>
	*/
	//void OutputCollectiveTest(int32_t iteration);

	/**
	* This function writes the data of an iteration into multiple files in a file-per-dedicated-core manner.
	* No collective I/O is used in this case.
	* Is enabled by specifying "CollectiveTest" in the Damaris
	* XML config file <store> section:
	*  <option key="FileMode">FilePerCore</option>
	*/
	void OutputPerCore(int32_t iteration);

public:
	
	/**
	 * This function is called after an iteration completed
	 * and is meant to output all variables into files.
	 */
	virtual void Output(int32_t iteration);


	template<typename SUPER>
	static std::shared_ptr<SUPER> New(const model::Store& mdl, const std::string& name) {
		return std::shared_ptr<SUPER>(new HDF5Store(mdl), Deleter<HDF5Store>());
	}
};

}

#endif
