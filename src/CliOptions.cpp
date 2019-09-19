#include "JoSIM/CliOptions.hpp"

#include "JoSIM/j_errors.h"
#include "JoSIM/j_globals.h"
#include "JoSIM/j_std_include.h"
#include <cstring>

#include <cstring>

using namespace JoSIM;

CliOptions CliOptions::parse(int argc, const char **argv) {
  CliOptions out;
  char swChar;
  bool specArg = false;
  int pos = 0;
  std::string argument;
  // If zero arguments are specified: complain
  if (argc <= 1) {
    Errors::cli_errors(TOO_FEW_ARGUMENTS);
    // If arguments are specified
  } else {
    // Loop through the arguments
    for (int i = 1; i < argc; i++) {
      // If the argument is a switch
      if (argv[i][0] == '-') {
        // If specific argument set tested char to 3rd char
        if (argv[i][1] == '-') {
          swChar = argv[i][2];
          specArg = true;
        }
        // If not set tested char to 2nd char
        else {
          swChar = argv[i][1];
          specArg = false;
        }
        switch (swChar) {
        // Analysis switch
        case 'a':
          if (specArg) {
            argument = argv[i];
            argument = argument.substr(argument.find('=') + 1);
            if (argument == "0")
              out.analysis_type = AnalysisType::Voltage;
            else if (argument == "1")
              out.analysis_type = AnalysisType::Phase;
            else
              Errors::cli_errors(INVALID_ANALYSIS);
          } else {
            if (i != argc - 1) {
              if (argv[i + 1][0] != '-') {
                if (std::strcmp(argv[i + 1], "0") == 0)
                  out.analysis_type = AnalysisType::Voltage;
                else if (std::strcmp(argv[i + 1], "1") == 0)
                  out.analysis_type = AnalysisType::Phase;
                else
                  Errors::cli_errors(INVALID_ANALYSIS);
                i++;
              } else
                Errors::cli_errors(NO_ANALYSIS);
            } else
              Errors::cli_errors(NO_ANALYSIS);
          }
          break;
        // Convention switch
        case 'c':
          // If specific argument, extract argument
          if (specArg) {
            argument = argv[i];
            argument = argument.substr(argument.find('=') + 1);
            if (argument == "0")
              out.input_type = InputType::Jsim;
            else if (argument == "1")
              out.input_type = InputType::WrSpice;
            else
              Errors::cli_errors(INVALID_CONVENTION);
          } else {
            if (i != argc - 1) {
              if (argv[i + 1][0] != '-') {
                if (std::strcmp(argv[i + 1], "0") == 0)
                  out.input_type = InputType::Jsim;
                else if (std::strcmp(argv[i + 1], "1") == 0)
                  out.input_type = InputType::WrSpice;
                else
                  Errors::cli_errors(INVALID_CONVENTION);
                i++;
              } else
                Errors::cli_errors(NO_CONVENTION);
            } else
              Errors::cli_errors(NO_CONVENTION);
          }
          break;
        // Help switch
        case 'h':
          display_help();
          exit(0);
        // Output file switch
        case 'o':
          out.output_to_file = true;
          if (specArg) {
            argument = argv[i];
            if (argument.find('=') != std::string::npos) {
              argument = argument.substr(argument.find('=') + 1);
              out.output_file_name = argument;
              std::cout << "Output file: " << out.output_file_name << std::endl;
              std::cout << std::endl;
            } else {
              out.output_file_type = FileOutputType::Csv;
              out.output_file_name = "output.csv";
              std::cout << "Output file: " << out.output_file_name << std::endl;
              std::cout << std::endl;
            }
            if (out.output_file_name.find('.') != std::string::npos) {
              std::string outExt = out.output_file_name.substr(
                  out.output_file_name.find_last_of('.'),
                  out.output_file_name.size() - 1);
              std::transform(outExt.begin(), outExt.end(), outExt.begin(),
                             toupper);
              if (outExt == ".CSV")
                out.output_file_type = FileOutputType::Csv;
              else if (outExt == ".DAT")
                out.output_file_type = FileOutputType::Dat;
              else
                out.output_file_type = FileOutputType::WrSpice;
            } else
              out.output_file_type = FileOutputType::WrSpice;
          } else {
            if (i != argc - 1) {
              if (argv[i + 1][0] != '-') {
                out.output_file_name = argv[i + 1];
                std::cout << "Output file: " << out.output_file_name
                          << std::endl;
                std::cout << std::endl;
                if (out.output_file_name.find('.') != std::string::npos) {
                  std::string outExt = out.output_file_name.substr(
                      out.output_file_name.find_last_of('.'),
                      out.output_file_name.size() - 1);
                  std::transform(outExt.begin(), outExt.end(), outExt.begin(),
                                 toupper);
                  if (outExt == ".CSV")
                    out.output_file_type = FileOutputType::Csv;
                  else if (outExt == ".DAT")
                    out.output_file_type = FileOutputType::Dat;
                  else
                    out.output_file_type = FileOutputType::WrSpice;
                } else
                  out.output_file_type = FileOutputType::WrSpice;
                i++;
              } else {
                out.output_file_type = FileOutputType::Csv;
                out.output_file_name = "output.csv";
                std::cout << "Output file: " << out.output_file_name
                          << std::endl;
                std::cout << std::endl;
                Errors::cli_errors(NO_OUTPUT);
              }
            } else {
              out.output_file_type = FileOutputType::Csv;
              out.output_file_name = "output.csv";
              std::cout << "Output file: " << out.output_file_name << std::endl;
              std::cout << std::endl;
              Errors::cli_errors(NO_OUTPUT);
            }
          }
          break;
        // Parallelization switch
        case 'p':
#ifdef _OPENMP
          std::cout << "Parallelization is ENABLED" << std::endl;
#else
          std::cout << "Parallelization is DISABLED" << std::endl;
#endif
          break;
        // Verbose switch
        case 'V':
          out.verbose = true;
          break;
        // Version switch
        case 'v':
          if (specArg) {
            argument = argv[i];
            if (argument.find("version") != std::string::npos)
              exit(0);
            else if (argument.find("verbose") != std::string::npos)
              out.verbose = true;
            else
              Errors::cli_errors(UNKNOWN_SWITCH, argument);
            break;
          } else
            exit(0);
        // Specific option specifier
        case '-':

          break;
        }
        // If the argument is not a switch it is the input file
      } else {
        out.cir_file_name = argv[i];
        std::cout << "Input file: " << out.cir_file_name << std::endl;
        std::cout << std::endl;
      }
    }
  }

  if (out.cir_file_name.empty())
    Errors::cli_errors(NO_INPUT);

  return out;
}

void CliOptions::display_help() {
  std::cout << "JoSIM help interface\n";
  std::cout << "====================\n";
  std::cout << std::setw(13) << std::left << "-a" << std::setw(3) << std::left
            << "|"
            << "Specifies the analysis type." << std::endl;
  std::cout << std::setw(13) << std::left << "--analysis=" << std::setw(3)
            << std::left << "|"
            << "0 for Voltage analysis (Default)." << std::endl;
  std::cout << std::setw(13) << std::left << "  " << std::setw(3) << std::left
            << "|"
            << "1 for Phase analysis." << std::endl;
  std::cout << std::setw(13) << std::left << "  " << std::setw(3) << std::left
            << "|" << std::endl;

  std::cout << std::setw(13) << std::left << "-c" << std::setw(3) << std::left
            << "|"
            << "Sets the subcircuit convention to left(0) or right(1)."
            << std::endl;
  std::cout << std::setw(13) << std::left << "--convention=" << std::setw(3)
            << std::left << "|"
            << "Default is left. WRSpice (normal SPICE) use right."
            << std::endl;
  std::cout << std::setw(13) << std::left << "  " << std::setw(3) << std::left
            << "|"
            << "Eg. X01 SUBCKT 1 2 3     vs.     X01 1 2 3 SUBCKT" << std::endl;
  std::cout << std::setw(13) << std::left << "  " << std::setw(3) << std::left
            << "|" << std::endl;

  std::cout << std::setw(13) << std::left << "-h" << std::setw(3) << std::left
            << "|"
            << "Displays this help menu" << std::endl;
  std::cout << std::setw(13) << std::left << "--help" << std::setw(3)
            << std::left << "|"
            << " " << std::endl;
  std::cout << std::setw(13) << std::left << "  " << std::setw(3) << std::left
            << "|" << std::endl;

  std::cout << std::setw(13) << std::left << "-o" << std::setw(3) << std::left
            << "|"
            << "Specify output file for simulation results (.csv)."
            << std::endl;
  std::cout << std::setw(13) << std::left << "--output=" << std::setw(3)
            << std::left << "|"
            << "Default will be output.csv if no file is specified."
            << std::endl;
  std::cout << std::setw(13) << std::left << "  " << std::setw(3) << std::left
            << "|" << std::endl;

  std::cout << std::setw(13) << std::left << "-p" << std::setw(3) << std::left
            << "|"
            << "(EXPERIMENTAL) Enables parallelization of certain functions."
            << std::endl;
  std::cout << std::setw(13) << std::left << "--parallel" << std::setw(3)
            << std::left << "|"
            << "Requires compilation with OPENMP switch enabled." << std::endl;
  std::cout
      << std::setw(13) << std::left << "  " << std::setw(3) << std::left << "|"
      << "Threshold applies, overhead on small circuits negates performance."
      << std::endl;
  std::cout << std::setw(13) << std::left << "  " << std::setw(3) << std::left
            << "|" << std::endl;

  std::cout << std::setw(13) << std::left << "-V" << std::setw(3) << std::left
            << "|"
            << "Runs JoSIM in verbose mode." << std::endl;
  std::cout << std::setw(13) << std::left << "--verbose" << std::setw(3)
            << std::left << "|"
            << " " << std::endl;
  std::cout << std::setw(13) << std::left << "  " << std::setw(3) << std::left
            << "|" << std::endl;

  std::cout << std::setw(13) << std::left << "-v" << std::setw(3) << std::left
            << "|"
            << "Displays the JoSIM version info only." << std::endl;
  std::cout << std::setw(13) << std::left << "--version" << std::setw(3)
            << std::left << "|"
            << " " << std::endl;
  std::cout << std::setw(13) << std::left << "  " << std::setw(3) << std::left
            << "|" << std::endl;

  std::cout << std::endl;
  std::cout << "Example command: josim -o ./output.csv test.cir" << std::endl;
  std::cout << std::endl;
  exit(0);
}

void CliOptions::version_info() {
  std::cout << std::endl;
  std::cout
      << "JoSIM: Josephson Junction Superconductive SPICE Circuit Simulator"
      << std::endl;
  std::cout << "Copyright (C) 2019 by Johannes Delport (jdelport@sun.ac.za)"
            << std::endl;
  std::cout << "v" << VERSION << "." << GIT_COMMIT_HASH << " compiled on " << __DATE__ << " at "
            << __TIME__ << std::endl;
  std::cout << std::endl;
}
