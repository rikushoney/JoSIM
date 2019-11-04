// Copyright (c) 2019 Johannes Delport
// This code is licensed under MIT license (see LICENSE for details)

#include "JoSIM/TransmissionLine.hpp"
#include "JoSIM/Misc.hpp"
#include "JoSIM/Errors.hpp"
#include "JoSIM/Constants.hpp"

#include <iostream>
#include <string>
#include <algorithm>
#include <locale>
#include <functional>

void TransmissionLine::create_transmissionline(
    const std::pair<std::string, std::string> &s,
    std::vector<TransmissionLine> &transmissionlines, 
    const std::unordered_map<std::string, int> &nm, 
    std::vector<int> &nc,
    const std::unordered_map<JoSIM::ParameterName, Parameter> &p,
    const int &antyp,
    const double &timestep) {
  std::vector<std::string> tokens = Misc::tokenize_space(s.first);
  // Ensure the device has at least 6 parts: LABEL PNODE1 NNODE1 PNODE2 NNODE2 VALUE
  if(tokens.size() < 6) {
    Errors::invalid_component_errors(static_cast<int>(ComponentErrors::INVALID_COMPONENT_DECLARATION), s.first);
  }
  // Ensure no device duplication occurs
  for(auto &i : transmissionlines) {
    if(i.get_label() == tokens.at(0)) {
      Errors::invalid_component_errors(static_cast<int>(ComponentErrors::DUPLICATE_LABEL), tokens.at(0));
    }
  }
  TransmissionLine temp;
  temp.set_label(tokens.at(0));
  std::string strippedLine = s.first;
  strippedLine.erase(std::remove_if(strippedLine.begin(), strippedLine.end(), std::bind(std::isspace<char>,
									std::placeholders::_1,
									std::locale::classic()
							)), strippedLine.end());
  auto impedance = strippedLine.find("Z0=");
  if(impedance == std::string::npos) {
    Errors::invalid_component_errors(static_cast<int>(ComponentErrors::INVALID_TX_DEFINED), s.first);
  }
  auto timeDelay = strippedLine.find("TD=");
  if(timeDelay == std::string::npos) {
    Errors::invalid_component_errors(static_cast<int>(ComponentErrors::INVALID_TX_DEFINED), s.first);
  }
  std::string impedanceValue;
  std::string timeDelayValue;
  if(strippedLine.substr(timeDelay).size() > strippedLine.substr(impedance).size()) {
    impedanceValue = strippedLine.substr(impedance+3);
    timeDelayValue = strippedLine.substr(timeDelay+3, impedance);
  } else {
    timeDelayValue = strippedLine.substr(timeDelay+3);
    impedanceValue = strippedLine.substr(impedance+3, timeDelay);
  }
  if(impedanceValue.find("{") != std::string::npos) {
    if(impedanceValue.find("}") != std::string::npos) {
      impedanceValue = impedanceValue.substr(impedanceValue.find("{")+1, impedanceValue.find("}") - impedanceValue.find("{"));
    } else {
      Errors::invalid_component_errors(static_cast<int>(ComponentErrors::INVALID_EXPR), s.first);
    }
  }
  if(timeDelayValue.find("{") != std::string::npos) {
    if(timeDelayValue.find("}") != std::string::npos) {
      timeDelayValue = timeDelayValue.substr(timeDelayValue.find("{")+1, timeDelayValue.find("}") - timeDelayValue.find("{"));
    } else {
      Errors::invalid_component_errors(static_cast<int>(ComponentErrors::INVALID_EXPR), s.first);
    }
  }
  temp.set_value(std::make_pair(impedanceValue, s.second), p, antyp, timestep);
  temp.set_timestepDelay(std::make_pair(timeDelayValue, s.second), p, timestep);
  temp.set_nonZeros_and_columnIndex(std::make_pair(tokens.at(1), tokens.at(2)), std::make_pair(tokens.at(3), tokens.at(4)), nm, s.first, nc);
  temp.set_indices(std::make_pair(tokens.at(1), tokens.at(2)), std::make_pair(tokens.at(3), tokens.at(4)), nm, nc);
  temp.set_currentIndex1(nc.size() - 1);
  temp.set_currentIndex2(nc.size());
  transmissionlines.emplace_back(temp);
}

void TransmissionLine::set_nonZeros_and_columnIndex(const std::pair<std::string, std::string> &n1, 
        const std::pair<std::string, std::string> &n2,
        const std::unordered_map<std::string, int> &nm, 
        const std::string &s, std::vector<int> &nc) {
  nonZeros_.clear();
  columnIndex_.clear();
  // 0
  if(n1.first.find("GND") != std::string::npos || n1.first == "0")  {
    // 0 0
    if(n1.second.find("GND") != std::string::npos || n1.second == "0")  {
      // 0 0 0
      if(n2.first.find("GND") != std::string::npos || n2.first == "0")  {
        // 0 0 0 0  
        if(n2.second.find("GND") != std::string::npos || n2.second == "0")  {
          Errors::invalid_component_errors(static_cast<int>(ComponentErrors::BOTH_GROUND), s);
          nonZeros_.emplace_back(-value_);
          nc.emplace_back(1);
          nonZeros_.emplace_back(-value_);
          nc.emplace_back(1);
          columnIndex_.emplace_back(nc.size() - 2);
          columnIndex_.emplace_back(nc.size() - 1);
        // 0 0 0 1
        } else {
          Errors::invalid_component_errors(static_cast<int>(ComponentErrors::BOTH_GROUND), s);
          nonZeros_.emplace_back(-1);
          nonZeros_.emplace_back(-value_);
          nc.emplace_back(1);
          nonZeros_.emplace_back(-1);
          nonZeros_.emplace_back(-value_);
          nc.emplace_back(2);
          columnIndex_.emplace_back(nc.size() - 1);
          columnIndex_.emplace_back(nc.size() - 2);
          columnIndex_.emplace_back(nm.at(n2.second));
          columnIndex_.emplace_back(nc.size() - 1);
        }
      // 0 0 1 0  
      } else if(n2.second.find("GND") != std::string::npos || n2.second == "0")  {
        Errors::invalid_component_errors(static_cast<int>(ComponentErrors::BOTH_GROUND), s);
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(-value_);
        nc.emplace_back(1);
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(-value_);
        nc.emplace_back(2);
        columnIndex_.emplace_back(nc.size() - 1);
        columnIndex_.emplace_back(nc.size() - 2);
        columnIndex_.emplace_back(nm.at(n2.first));
        columnIndex_.emplace_back(nc.size() - 1);
      // 0 0 1 1
      } else {
        Errors::invalid_component_errors(static_cast<int>(ComponentErrors::BOTH_GROUND), s);
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(-1);
        nonZeros_.emplace_back(-value_);
        nc.emplace_back(1);
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(-1);
        nonZeros_.emplace_back(-value_);
        nc.emplace_back(3);
        columnIndex_.emplace_back(nc.size() - 1);
        columnIndex_.emplace_back(nc.size() - 1);
        columnIndex_.emplace_back(nc.size() - 2);
        columnIndex_.emplace_back(nm.at(n2.first));
        columnIndex_.emplace_back(nm.at(n2.second));
        columnIndex_.emplace_back(nc.size() - 1);
      }
    // 0 1  
    } else if(n2.first.find("GND") != std::string::npos || n2.first == "0")  {
      // 0 1 0
      if(n2.first.find("GND") != std::string::npos || n2.first == "0")  {
        // 0 1 0 0  
        if(n2.second.find("GND") != std::string::npos || n2.second == "0")  {
          Errors::invalid_component_errors(static_cast<int>(ComponentErrors::BOTH_GROUND), s);
          nonZeros_.emplace_back(-1);
          nonZeros_.emplace_back(-1);
          nonZeros_.emplace_back(-value_);
          nc.emplace_back(2);
          nonZeros_.emplace_back(-value_);
          nc.emplace_back(1);
          columnIndex_.emplace_back(nc.size() - 2);
          columnIndex_.emplace_back(nm.at(n1.second));
          columnIndex_.emplace_back(nc.size() - 2);
          columnIndex_.emplace_back(nc.size() - 1);
        // 0 1 0 1
        } else {
          nonZeros_.emplace_back(-1);
          nonZeros_.emplace_back(-1);
          nonZeros_.emplace_back(-1);
          nonZeros_.emplace_back(-value_);
          nc.emplace_back(2);
          nonZeros_.emplace_back(-1);
          nonZeros_.emplace_back(-value_);
          nc.emplace_back(2);
          columnIndex_.emplace_back(nc.size() - 2);
          columnIndex_.emplace_back(nc.size() - 1);
          columnIndex_.emplace_back(nm.at(n1.second));
          columnIndex_.emplace_back(nc.size() - 2);
          columnIndex_.emplace_back(nm.at(n2.second));
          columnIndex_.emplace_back(nc.size() - 1);
        }
      // 0 1 1 0  
      } else if(n2.second.find("GND") != std::string::npos || n2.second == "0")  {
        nonZeros_.emplace_back(-1);
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(-1);
        nonZeros_.emplace_back(-value_);
        nc.emplace_back(2);
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(-value_);
        nc.emplace_back(2);
        columnIndex_.emplace_back(nc.size() - 2);
        columnIndex_.emplace_back(nc.size() - 1);
        columnIndex_.emplace_back(nm.at(n1.second));
        columnIndex_.emplace_back(nc.size() - 2);
        columnIndex_.emplace_back(nm.at(n2.first));
        columnIndex_.emplace_back(nc.size() - 1);
      // 0 1 1 1
      } else {
        nonZeros_.emplace_back(-1);
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(-1);
        nonZeros_.emplace_back(-1);
        nonZeros_.emplace_back(-value_);
        nc.emplace_back(2);
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(-1);
        nonZeros_.emplace_back(-value_);
        nc.emplace_back(3);
        columnIndex_.emplace_back(nc.size() - 2);
        columnIndex_.emplace_back(nc.size() - 1);
        columnIndex_.emplace_back(nc.size() - 1);
        columnIndex_.emplace_back(nm.at(n1.second));
        columnIndex_.emplace_back(nc.size() - 2);
        columnIndex_.emplace_back(nm.at(n2.first));
        columnIndex_.emplace_back(nm.at(n2.second));
        columnIndex_.emplace_back(nc.size() - 1);
      }
    }
  // 1
  } else {
    // 1 0
    if(n1.second.find("GND") != std::string::npos || n1.second == "0")  {
      // 1 0 0
      if(n2.first.find("GND") != std::string::npos || n2.first == "0")  {
        // 1 0 0 0  
        if(n2.second.find("GND") != std::string::npos || n2.second == "0")  {
          Errors::invalid_component_errors(static_cast<int>(ComponentErrors::BOTH_GROUND), s);
        // 1 0 0 1
        } else {
          nonZeros_.emplace_back(1);
          nonZeros_.emplace_back(-1);
          nonZeros_.emplace_back(1);
          nonZeros_.emplace_back(-value_);
          nc.emplace_back(2);
          nonZeros_.emplace_back(-1);
          nonZeros_.emplace_back(-value_);
          nc.emplace_back(2);
          columnIndex_.emplace_back(nc.size() - 2);
          columnIndex_.emplace_back(nc.size() - 1);
          columnIndex_.emplace_back(nm.at(n1.first));
          columnIndex_.emplace_back(nc.size() - 2);
          columnIndex_.emplace_back(nm.at(n2.second));
          columnIndex_.emplace_back(nc.size() - 1);
        }
      // 1 0 1 0  
      } else if(n2.second.find("GND") != std::string::npos || n2.second == "0")  {
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(-value_);
        nc.emplace_back(2);
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(-value_);
        nc.emplace_back(2);
        columnIndex_.emplace_back(nc.size() - 2);
        columnIndex_.emplace_back(nc.size() - 1);
        columnIndex_.emplace_back(nm.at(n1.first));
        columnIndex_.emplace_back(nc.size() - 2);
        columnIndex_.emplace_back(nm.at(n2.first));
        columnIndex_.emplace_back(nc.size() - 1);
      // 1 0 1 1
      } else {
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(-1);
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(-value_);
        nc.emplace_back(2);
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(-1);
        nonZeros_.emplace_back(-value_);
        nc.emplace_back(3);
        columnIndex_.emplace_back(nc.size() - 2);
        columnIndex_.emplace_back(nc.size() - 1);
        columnIndex_.emplace_back(nc.size() - 1);
        columnIndex_.emplace_back(nm.at(n1.first));
        columnIndex_.emplace_back(nc.size() - 2);
        columnIndex_.emplace_back(nm.at(n2.first));
        columnIndex_.emplace_back(nm.at(n2.second));
        columnIndex_.emplace_back(nc.size() - 1);
      }
    // 1 1  
    } else if(n2.first.find("GND") != std::string::npos || n2.first == "0")  {
      // 1 1 0
      if(n2.first.find("GND") != std::string::npos || n2.first == "0")  {
        // 1 1 0 0  
        if(n2.second.find("GND") != std::string::npos || n2.second == "0")  {
          Errors::invalid_component_errors(static_cast<int>(ComponentErrors::BOTH_GROUND), s);
          nonZeros_.emplace_back(1);
          nonZeros_.emplace_back(-1);
          nonZeros_.emplace_back(1);
          nonZeros_.emplace_back(-1);
          nonZeros_.emplace_back(-value_);
          nc.emplace_back(3);
          nonZeros_.emplace_back(-value_);
          nc.emplace_back(1);
          columnIndex_.emplace_back(nc.size() - 2);
          columnIndex_.emplace_back(nc.size() - 2);
          columnIndex_.emplace_back(nm.at(n1.first));
          columnIndex_.emplace_back(nm.at(n1.second));
          columnIndex_.emplace_back(nc.size() - 2);
          columnIndex_.emplace_back(nc.size() - 1);
        // 1 1 0 1
        } else {
          nonZeros_.emplace_back(1);
          nonZeros_.emplace_back(-1);
          nonZeros_.emplace_back(-1);
          nonZeros_.emplace_back(1);
          nonZeros_.emplace_back(-1);
          nonZeros_.emplace_back(-value_);
          nc.emplace_back(3);
          nonZeros_.emplace_back(-1);
          nonZeros_.emplace_back(-value_);
          nc.emplace_back(2);
          columnIndex_.emplace_back(nc.size() - 2);
          columnIndex_.emplace_back(nc.size() - 2);
          columnIndex_.emplace_back(nc.size() - 1);
          columnIndex_.emplace_back(nm.at(n1.first));
          columnIndex_.emplace_back(nm.at(n1.second));
          columnIndex_.emplace_back(nc.size() - 2);
          columnIndex_.emplace_back(nm.at(n2.second));
          columnIndex_.emplace_back(nc.size() - 1);
        }
      // 1 1 1 0  
      } else if(n2.second.find("GND") != std::string::npos || n2.second == "0")  {
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(-1);
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(-1);
        nonZeros_.emplace_back(-value_);
        nc.emplace_back(3);
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(-value_);
        nc.emplace_back(3);
        columnIndex_.emplace_back(nc.size() - 2);
        columnIndex_.emplace_back(nc.size() - 2);
        columnIndex_.emplace_back(nc.size() - 1);
        columnIndex_.emplace_back(nm.at(n1.first));
        columnIndex_.emplace_back(nm.at(n1.second));
        columnIndex_.emplace_back(nc.size() - 2);
        columnIndex_.emplace_back(nm.at(n2.first));
        columnIndex_.emplace_back(nc.size() - 1);
      // 1 1 1 1
      } else {
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(-1);
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(-1);
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(-1);
        nonZeros_.emplace_back(-value_);
        nc.emplace_back(3);
        nonZeros_.emplace_back(1);
        nonZeros_.emplace_back(-1);
        nonZeros_.emplace_back(-value_);
        nc.emplace_back(3);
        columnIndex_.emplace_back(nc.size() - 2);
        columnIndex_.emplace_back(nc.size() - 2);
        columnIndex_.emplace_back(nc.size() - 1);
        columnIndex_.emplace_back(nc.size() - 1);
        columnIndex_.emplace_back(nm.at(n1.first));
        columnIndex_.emplace_back(nm.at(n1.second));
        columnIndex_.emplace_back(nc.size() - 2);
        columnIndex_.emplace_back(nm.at(n2.first));
        columnIndex_.emplace_back(nm.at(n2.second));
        columnIndex_.emplace_back(nc.size() - 1);
      }
    }
  }
}

void TransmissionLine::set_indices(const std::pair<std::string, std::string> &n1, 
        const std::pair<std::string, std::string> &n2, 
        const std::unordered_map<std::string, int> &nm, std::vector<int> &nc) {
  if(n1.second.find("GND") != std::string::npos || n1.second == "0") {
    posIndex1_ = nm.at(n1.first);
    ++nc.at(nm.at(n1.first));
  } else if(n1.first.find("GND") != std::string::npos || n1.first == "0") {
    negIndex1_ = nm.at(n1.second);
    ++nc.at(nm.at(n1.second));
  } else {
    posIndex1_ = nm.at(n1.first);
    negIndex1_ = nm.at(n1.second);
    ++nc.at(nm.at(n1.first));
    ++nc.at(nm.at(n1.second));
  }
  if(n2.second.find("GND") != std::string::npos || n2.second == "0") {
    posIndex2_ = nm.at(n2.first);
    ++nc.at(nm.at(n2.first));
  } else if(n2.first.find("GND") != std::string::npos || n2.first == "0") {
    negIndex2_ = nm.at(n2.second);
    ++nc.at(nm.at(n2.second));
  } else {
    posIndex2_ = nm.at(n2.first);
    negIndex2_ = nm.at(n2.second);
    ++nc.at(nm.at(n2.first));
    ++nc.at(nm.at(n2.second));
  }
}

void TransmissionLine::set_value(const std::pair<std::string, std::string> &s, 
        const std::unordered_map<JoSIM::ParameterName, Parameter> &p,
        const int &antyp, const double &timestep) {
  if (antyp == 0) value_ = Parameters::parse_param(s.first, p, s.second);
  else if (antyp == 1) value_ = (timestep * Parameters::parse_param(s.first, p, s.second)) / (2 * JoSIM::Constants::SIGMA);
}

void TransmissionLine::set_timestepDelay(const std::pair<std::string, std::string> &s, 
        const std::unordered_map<JoSIM::ParameterName, Parameter> &p,
        const double &timestep) {
  timestepDelay_ = (int)(Parameters::parse_param(s.first, p, s.second) / timestep);
}