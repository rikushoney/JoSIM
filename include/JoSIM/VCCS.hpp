// Copyright (c) 2020 Johannes Delport
// This code is licensed under MIT license (see LICENSE for details)
#ifndef JOSIM_VCCS_HPP
#define JOSIM_VCCS_HPP

#include "JoSIM/BasicComponent.hpp"
#include "JoSIM/ParameterName.hpp"
#include "JoSIM/Parameters.hpp"
#include "JoSIM/AnalysisType.hpp"
#include "JoSIM/Input.hpp"

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>

namespace JoSIM {

 /*
  Glabel Vo⁺ Vo⁻ Vc⁺ Vc⁻ G
  
  Io = GVc
  
  ⎡ 0  0  0  0    1⎤ ⎡Vo⁺⎤   ⎡ 0⎤
  ⎜ 0  0  0  0   -1⎟ ⎜Vo⁻⎟   ⎜ 0⎟
  ⎜ 0  0  0  0    0⎟ ⎜Vc⁺⎟ = ⎜ 0⎟
  ⎜ 0  0  0  0    0⎟ ⎜Vc⁻⎟   ⎜ 0⎟
  ⎣ 0  0  1 -1 -1/G⎦ ⎣Io ⎦   ⎣ 0⎦

  (PHASE)
  φ - (2e/hbar)(2h/3G)Io = (4/3)φn-1 - (1/3)φn-2

  ⎡ 0  0  0  0                 1⎤ ⎡φo⁺⎤   ⎡                     0⎤
  ⎜ 0  0  0  0                -1⎟ ⎜φo⁻⎟   ⎜                     0⎟
  ⎜ 0  0  0  0                 0⎟ ⎜φc⁺⎟ = ⎜                     0⎟
  ⎜ 0  0  0  0                 0⎟ ⎜φc⁻⎟   ⎜                     0⎟
  ⎣ 0  0  1 -1 -(2e/hbar)(2h/3G)⎦ ⎣Io ⎦   ⎣ (4/3)φn-1 - (1/3)φn-2⎦  
 */ 


class VCCS : public BasicComponent {
  private:
  JoSIM::AnalysisType at_;
  public:
  NodeConfig nodeConfig2_;
  int_o posIndex2_, negIndex2_;
  double pn1_ = 0.0, pn2_ = 0.0, pn3_ = 0.0, pn4_ = 0.0;

  VCCS(
    const std::pair<tokens_t, string_o> &s, const NodeConfig &ncon,
    const std::optional<NodeConfig> &ncon2, const nodemap &nm, 
    std::unordered_set<std::string> &lm, nodeconnections &nc,
    const param_map &pm, int &bi, const AnalysisType &at, const double &h);

  void set_node_indices(
    const tokens_t &t, const nodemap &nm, nodeconnections &nc);
  void set_matrix_info();

  void update_timestep(const double &factor) override;

  void step_back() override {
    pn2_ = pn4_;
  }
}; // class VCCS

} // namespace JoSIM
#endif