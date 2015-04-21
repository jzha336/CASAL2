/**
 * @file ProcessProportionsAtAge.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "ProcessProportionsAtAge.h"

#include "TimeSteps/Manager.h"
#include "Utilities/DoubleCompare.h"

// namespaces
namespace niwa {
namespace observations {

/**
 *
 */
ProcessProportionsAtAge::ProcessProportionsAtAge() {
  parameters_.Bind<string>(PARAM_PROCESS, &process_label_, "Process label", "");
  parameters_.Bind<Double>(PARAM_PROCESS_PROPORTION, &process_proportion_, "Process proportion", "", Double(0.5));

  mean_proportion_method_ = false;
}

/**
 *
 */
void ProcessProportionsAtAge::DoBuild() {
  ProportionsAtAge::DoBuild();

  if (process_proportion_ < 0.0 || process_proportion_ > 1.0)
    LOG_ERROR_P(PARAM_PROCESS_PROPORTION) << ": process_proportion (" << AS_DOUBLE(process_proportion_) << ") must be between 0.0 and 1.0";
  proportion_of_time_ = process_proportion_;

  TimeStepPtr time_step = timesteps::Manager::Instance().GetTimeStep(time_step_label_);
  if (!time_step)
    LOG_FATAL_P(PARAM_TIME_STEP) << time_step_label_ << " could not be found. Have you defined it?";

  for (unsigned year : years_)
    time_step->SubscribeToProcess(shared_ptr(), year, process_label_);
}

} /* namespace observations */
} /* namespace niwa */
