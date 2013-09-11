/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef OBSERVATIONS_FACTORY_H_
#define OBSERVATIONS_FACTORY_H_

// Headers
#include "Observations/Observation.h"

// Namespaces
namespace isam {
namespace observations {

/**
 * Class Definition
 */
class Factory {
public:
  // methods
  static ObservationPtr       Create(const string& block_type, const string& object_type);

private:
  // Methods
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace observations */
} /* namespace isam */
#endif /* OBSERVATIONS_FACTORY_H_ */