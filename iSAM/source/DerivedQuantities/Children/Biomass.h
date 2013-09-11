/**
 * @file Biomass.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 18/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef DERIVEDQUANTITIES_BIOMASS_H_
#define DERIVEDQUANTITIES_BIOMASS_H_

// headers
#include "DerivedQuantities/DerivedQuantity.h"

// namespaces
namespace isam {
namespace derivedquantities {

/**
 * class definition
 */
class Biomass : public isam::DerivedQuantity {
public:
  // methods
  Biomass();
  virtual                     ~Biomass() = default;
  void                        Calculate() override final;
  void                        Calculate(unsigned initialisation_phase) override final;
};

} /* namespace derivedquantities */
} /* namespace isam */
#endif /* DERIVEDQUANTITIES_BIOMASS_H_ */