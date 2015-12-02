/**
 * @file CommandLineParser.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/11/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

 // Headers
#include "CommandLineParser.h"

#include <iostream>
#include <sstream>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>

#include "BaseClasses/Object.h"
#include "GlobalConfiguration/GlobalConfiguration.h"
#include "Logging/Logging.h"
#include "Model/Factory.h"
#include "Reports/Factory.h"

// Namespaces
namespace niwa {
namespace utilities {

using boost::program_options::options_description;
using boost::program_options::value;
using boost::program_options::variables_map;
using std::cout;
using std::endl;
using std::ostringstream;

/**
 * This method will take the raw command line input from the main() method
 * and process them into something more useful.
 *
 * @param argc The number of arguments that have been provided
 * @param argv Pointer to an array containing the arguments
 */
void CommandLineParser::Parse(int argc, const char* argv[]) {
  LOG_TRACE();

  // Build Options menu
  options_description oDesc("Usage");
  oDesc.add_options()
    ("help,h", "Print help")
    ("license,l", "Display iSAM license")
    ("version,v", "Display version information")
    ("config,c", value<string>(), "Configuration file")
    ("run,r", "Basic model run mode")
    ("estimate,e", "Point estimation run mode")
    ("mcmc,m", "Markov Chain Monte Carlo run mode (arg = continue, default: false)")
    ("resume", "Resume the MCMC chain")
    ("objective-file", value<string>(), "Objective file for resuming an MCMC")
    ("sample-file", value<string>(), "Sample file for resuming an MCMC")
    ("profiling,p", "Profling run mode")
    ("simulation,s", value<unsigned>(), "Simulation mode (arg = number of candidates)")
    ("projection,f", "Projection mode")
    ("input,i", value<string>(), "Load free parameter values from file")
    ("fi", "Force the input file to only allow @estimate parameters (basic run mode only)")
    ("seed,g", value<string>(), "Random number seed")
    ("query,q", value<string>(), "Query an object type to see it's description and parameters")
    ("debug,d", "Run in debug mode (with debug output")
    ("nostd", "Do not print the standard header report")
    ("loglevel", value<string>(), "Set log level: finest, fine, trace, none(default)")
    ("output,o", value<string>(), "Create estimate value report directed to <file>")
    ("single-step", "Single step the model each year with new estimable values");


  ostringstream o;
  o << oDesc;
  command_line_usage_ = o.str();

  // Read our Parameters
  variables_map parameters;

  try {
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, oDesc), parameters);
    notify(parameters);

  } catch (boost::program_options::unknown_option &ex) {
    cout << "An error occurred while processing the command line. " << ex.what() << endl;
  }

  /**
   * Load any variables into the global config that need to be available
   * immediately
   */
  if (parameters.count("debug"))
    global_config_.set_debug_mode("true");
  if (parameters.count("config"))
    global_config_.set_config_file(parameters["config"].as<string>());
  if (parameters.count("input"))
    global_config_.set_estimable_value_file(parameters["input"].as<string>());
  if (parameters.count("fi"))
    global_config_.set_force_estimable_values_file();
  if (parameters.count("nostd"))
    global_config_.set_disable_standard_report();
  if (parameters.count("loglevel"))
    Logging::Instance().SetLogLevel(parameters["loglevel"].as<string>());
  if (parameters.count("output")) {
    auto report = reports::Factory::Create(&model_, PARAM_REPORT, PARAM_ESTIMATE_VALUE);
    report->parameters().Add(PARAM_LABEL, "estimate_value_output", __FILE__, __LINE__);
    report->parameters().Add(PARAM_TYPE, PARAM_ESTIMATE_VALUE, __FILE__, __LINE__);
    report->parameters().Add(PARAM_FILE_NAME, parameters["output"].as<string>(), __FILE__, __LINE__);
    report->set_skip_tags(true);
  }
  if (parameters.count("single-step"))
    global_config_.flag_single_step();

  /**
   * Determine what run mode we should be in. If we're
   * in help, version or license then we don't need to continue.
   */
  if ( (parameters.count("help")) || (parameters.size() == 0) ) {
    run_mode_ = RunMode::kHelp;
    return;

  } else if (parameters.count("version")) {
    run_mode_ = RunMode::kVersion;
    return;

  } else if (parameters.count("license")) {
    run_mode_ = RunMode::kLicense;
    return;

  } else if (parameters.count("query")) {
    string lookup = parameters["query"].as<string>();
    vector<string> parts;
    boost::split(parts, lookup, boost::is_any_of("."));
    if (parts.size() == 1)
      parts.push_back("");
    if (parts.size() == 2) {
      base::Object* object = model_.factory().CreateObject(parts[0], parts[1]);
      if (object) {
        cout << "Printing information for " << parts[0] << " with sub-type " << parts[1] << endl;
        object->PrintParameterQueryInfo();
      } else {
        cout << "Object type " << lookup << " is invalid" << endl;
      }
    } else {
      cout << "Please use object_type.sub_type only when querying an object" << endl;
    }

    run_mode_ = RunMode::kQuery;
    return;
  }

  /**
   * At this point we know we've been asked to do an actual model
   * run. So we need to check to ensure the command line makes
   * sense.
   */
  unsigned run_mode_count = 0;
  run_mode_count += parameters.count("run");
  run_mode_count += parameters.count("estimate");
  run_mode_count += parameters.count("mcmc");
  run_mode_count += parameters.count("profiling");
  run_mode_count += parameters.count("simulation");
  run_mode_count += parameters.count("projection");

  if (run_mode_count == 0)
    LOG_ERROR() << "No valid run mode has been specified on the command line. Please specify a valid run mode (e.g -r)";
  if (run_mode_count > 1)
    LOG_ERROR() << "Multiple run modes have been specified on the command line. Only 1 run mode is valid";

  if (parameters.count("run"))
    run_mode_ = RunMode::kBasic;

  else if (parameters.count("estimate"))
    run_mode_ = RunMode::kEstimation;

  else if (parameters.count("mcmc")) {
    run_mode_ = RunMode::kMCMC;
    if (parameters.count("resume")) {
      if (!parameters.count("objective-file") || !parameters.count("sample-file")) {
        LOG_ERROR() << "Resuming an MCMC chain requires the objective-file and sample-file parameters";
        return;
      }

      global_config_.set_mcmc_objective_file(parameters["objective-file"].as<string>());
      global_config_.set_mcmc_sample_file(parameters["sample-file"].as<string>());
      global_config_.flag_resume();
    }
  } else if (parameters.count("profiling"))
    run_mode_ = RunMode::kProfiling;

  else if (parameters.count("simulation")) {
    run_mode_ = RunMode::kSimulation;
    global_config_.set_simulation_candidates(parameters["simulation"].as<unsigned>());

  } else if (parameters.count("projection"))
    run_mode_ = RunMode::kProjection;

  else {
    LOG_ERROR() << "An invalid or unknown run mode has been specified on the command line.";
  }

  /**
   * Now we store any variables we want to use to override global defaults.
   */
  if (parameters.count("seed")) {
    override_values_[PARAM_RANDOM_NUMBER_SEED] = parameters["seed"].as<string>();
    cout << override_values_[PARAM_RANDOM_NUMBER_SEED] << " : " << parameters["seed"].as<string>() << endl;
  }
}

} /* namespace utilities */
} /* namespace niwa */







