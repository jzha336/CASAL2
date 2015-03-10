/**
 * @file Basic.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SIZEWEIGHTS_BASIC_H_
#define SIZEWEIGHTS_BASIC_H_

// headers
#include "SizeWeights/SizeWeight.h"

// namespaces
namespace niwa {
namespace sizeweights {

/**
 * class definition
 */
class Basic : public niwa::SizeWeight {
public:
  // methods
  Basic();
  virtual                     ~Basic() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final { };
  void                        DoReset() override final { };

  // accessors
  Double                      mean_weight(Double size, const string &distribution, Double cv) const override final;

private:
  // members
  Double                      a_ = 0;
  Double                      b_ = 0;
  Double                      cv_ = 0;
  string                      units_ = "";
};

} /* namespace sizeweights */
} /* namespace niwa */
#endif /* SIZEWEIGHTS_BASIC_H_ */
