// Copyright (c) 2019 Johannes Delport
// This code is licensed under MIT license (see LICENSE for details)
#include "JoSIM/j_simulation.h"

// Linear algebra include
#include "JoSIM/AnalysisType.hpp"
#include "JoSIM/j_components.h"
#include "JoSIM/j_globals.h"
#include "JoSIM/j_matrix.h"
#include "klu.h"

#include <cassert>

void Simulation::identify_simulation(const std::vector<std::string> &controls,
                                     double &prstep, double &tstop,
                                     double &tstart, double &maxtstep) {
  std::vector<std::string> simtokens;
  bool transFound = false;
  for (const auto &i : controls) {
    if (i.find("TRAN") != std::string::npos) {
      transFound = true;
      simtokens = Misc::tokenize_delimeter(i, " ,");
      if (simtokens.at(0).find("TRAN") != std::string::npos) {
        if (simtokens.size() < 2) {
          Errors::control_errors(TRANS_ERROR, "Too few parameters: " + i);
          maxtstep = 1E-12;
          tstop = 1E-9;
          tstart = 0;
        } else {
          prstep = Misc::modifier(simtokens[1]);
          if (simtokens.size() > 2) {
            tstop = Misc::modifier(simtokens[2]);
            if (simtokens.size() > 3) {
              tstart = Misc::modifier(simtokens[3]);
              if (simtokens.size() > 4) {
                maxtstep = Misc::modifier(simtokens[4]);
              } else
                maxtstep = 1E-12;
            } else {
              tstart = 0;
              maxtstep = 1E-12;
            }
          } else {
            tstop = 1E-9;
            tstart = 0;
            maxtstep = 1E-12;
          }
        }
      }
    }
  }
  if (!transFound) {
    Errors::control_errors(NO_SIM, "");
  }
}

// void Simulation::transient_voltage_simulation(Input &iObj, Matrix &mObj) {
//   results.xVect.clear();
//   results.timeAxis.clear();
//   std::vector<double> lhsValues(mObj.Nsize, 0.0),
//       RHS(mObj.columnNames.size(), 0.0), LHS_PRE;
//   int simSize = iObj.transSim.simsize();
//   for (int m = 0; m < mObj.relXInd.size(); m++) {
//     results.xVect.emplace_back(std::vector<double>(simSize, 0.0));
//   }
//   // std::vector<std::vector<std::string>>
//   // nodeConnectionVector(mObj.rowNames.size());
//   std::string txCurrent;
//   double VB, RHSvalue, inductance, z0voltage;
//   double hn_2_2e_hbar = (iObj.transSim.prstep / 2) * (2 * M_PI / PHI_ZERO);
//   int ok, rowCounter;
//   bool needsLU = false;
//   klu_symbolic *Symbolic;
//   klu_common Common;
//   klu_numeric *Numeric;
//   ok = klu_defaults(&Common);
//   assert(ok);
//   Symbolic = klu_analyze(mObj.Nsize, &mObj.rowptr.front(), &mObj.colind.front(),
//                          &Common);
//   Numeric = klu_factor(&mObj.rowptr.front(), &mObj.colind.front(),
//                        &mObj.nzval.front(), Symbolic, &Common);
//   rowCounter = 0;
//   // for (const auto &j : mObj.rowDesc) {

//   // // assert(j.size() >= 3);

//   // // 	if (j[2] == 'N') {
//   // // 		nodeConnectionVector[rowCounter] =
//   // mObj.nodeConnections[j];
//   // // 	}
//   // // 	rowCounter++;
//   // // }
//   if (sOutput)
//     std::cout << "Simulating:" << std::endl;
//   double increments = 100 / (double)simSize;
//   double progress_increments = 30 / (double)simSize;
//   double incremental_progress = 0.0;

//   int progress = 0;
//   int old_progress = 0;
//   int imintd = 0;
//   std::string pBar;
//   double mutualL = 0.0;
//   double CUR2 = 0.0;
//   for (int i = 0; i < simSize; i++) {
// #ifndef NO_PRINT
//     if (sOutput)
//       std::cout << '\r' << std::flush;
// #endif
//     RHS.clear();
//     rowCounter = 0;
//     for (auto j : mObj.rowDesc) {
//       RHSvalue = 0.0;
//       if (j.type == RowDescriptor::Type::VoltageNode) {
//         for (auto k : mObj.nodeConnections.at(j.index).connections) {
//           if (k.type == ComponentConnections::Type::JJP) {
//             RHSvalue += mObj.components.voltJJ.at(k.index).iS;
//           } else if (k.type == ComponentConnections::Type::JJN) {
//             RHSvalue -= mObj.components.voltJJ.at(k.index).iS;
//           } else if (k.type == ComponentConnections::Type::CSN) {
//             RHSvalue += mObj.sources.at(k.index).at(i);
//           } else if (k.type == ComponentConnections::Type::CSP) {
//             RHSvalue -= mObj.sources.at(k.index).at(i);
//           }
//         }
//       } else if (j.type == RowDescriptor::Type::VoltageInductor) {
//         if (mObj.components.voltInd.at(j.index).posNRow == -1)
//           VB = -lhsValues.at(mObj.components.voltInd.at(j.index).negNRow);
//         else if (mObj.components.voltInd.at(j.index).negNRow == -1)
//           VB = lhsValues.at(mObj.components.voltInd.at(j.index).posNRow);
//         else
//           VB = lhsValues.at(mObj.components.voltInd.at(j.index).posNRow) -
//                lhsValues.at(mObj.components.voltInd.at(j.index).negNRow);
//         RHSvalue =
//             (-2 * mObj.components.voltInd.at(j.index).value /
//              iObj.transSim.prstep) *
//                 lhsValues.at(mObj.components.voltInd.at(j.index).curNRow) -
//             VB;
//         for (const auto &m : mObj.components.voltInd.at(j.index).mut) {
//           RHSvalue =
//               RHSvalue -
//               (m.second *
//                lhsValues.at(mObj.components.voltInd.at(m.first).curNRow));
//         }
//       } else if (j.type == RowDescriptor::Type::VoltageJJ) {
//         RHSvalue = mObj.components.voltJJ.at(j.index).pn1 +
//                    hn_2_2e_hbar * mObj.components.voltJJ.at(j.index).vn1;
//       } else if (j.type == RowDescriptor::Type::VoltageVS) {
//         RHSvalue = mObj.sources.at(j.index).at(i);
//       } else if (j.type == RowDescriptor::Type::VoltageTX1) {
//         if (i >= mObj.components.txLine.at(j.index).k) {
//           if (mObj.components.txLine.at(j.index).posN2Row == -1)
//             VB = -results.xVect
//                       .at(std::distance(
//                           mObj.relXInd.begin(),
//                           std::find(
//                               mObj.relXInd.begin(), mObj.relXInd.end(),
//                               mObj.components.txLine.at(j.index).negN2Row)))
//                       .at(i - mObj.components.txLine.at(j.index).k);
//           else if (mObj.components.txLine.at(j.index).negN2Row == -1)
//             VB =
//                 results.xVect
//                     .at(std::distance(
//                         mObj.relXInd.begin(),
//                         std::find(mObj.relXInd.begin(), mObj.relXInd.end(),
//                                   mObj.components.txLine.at(j.index).posN2Row)))
//                     .at(i - mObj.components.txLine.at(j.index).k);
//           else
//             VB =
//                 results.xVect
//                     .at(std::distance(
//                         mObj.relXInd.begin(),
//                         std::find(mObj.relXInd.begin(), mObj.relXInd.end(),
//                                   mObj.components.txLine.at(j.index).posN2Row)))
//                     .at(i - mObj.components.txLine.at(j.index).k) -
//                 results.xVect
//                     .at(std::distance(
//                         mObj.relXInd.begin(),
//                         std::find(mObj.relXInd.begin(), mObj.relXInd.end(),
//                                   mObj.components.txLine.at(j.index).negN2Row)))
//                     .at(i - mObj.components.txLine.at(j.index).k);
//           RHSvalue =
//               mObj.components.txLine.at(j.index).value *
//                   results.xVect
//                       .at(std::distance(
//                           mObj.relXInd.begin(),
//                           std::find(
//                               mObj.relXInd.begin(), mObj.relXInd.end(),
//                               mObj.components.txLine.at(j.index).curN2Row)))
//                       .at(i - mObj.components.txLine.at(j.index).k) +
//               VB;
//         }
//       } else if (j.type == RowDescriptor::Type::VoltageTX2) {
//         if (i >= mObj.components.txLine.at(j.index).k) {
//           if (mObj.components.txLine.at(j.index).posNRow == -1)
//             VB =
//                 -results.xVect
//                      .at(std::distance(
//                          mObj.relXInd.begin(),
//                          std::find(mObj.relXInd.begin(), mObj.relXInd.end(),
//                                    mObj.components.txLine.at(j.index).negNRow)))
//                      .at(i - mObj.components.txLine.at(j.index).k);
//           else if (mObj.components.txLine.at(j.index).negNRow == -1)
//             VB = results.xVect
//                      .at(std::distance(
//                          mObj.relXInd.begin(),
//                          std::find(mObj.relXInd.begin(), mObj.relXInd.end(),
//                                    mObj.components.txLine.at(j.index).posNRow)))
//                      .at(i - mObj.components.txLine.at(j.index).k);
//           else
//             VB = results.xVect
//                      .at(std::distance(
//                          mObj.relXInd.begin(),
//                          std::find(mObj.relXInd.begin(), mObj.relXInd.end(),
//                                    mObj.components.txLine.at(j.index).posNRow)))
//                      .at(i - mObj.components.txLine.at(j.index).k) -
//                  results.xVect
//                      .at(std::distance(
//                          mObj.relXInd.begin(),
//                          std::find(mObj.relXInd.begin(), mObj.relXInd.end(),
//                                    mObj.components.txLine.at(j.index).negNRow)))
//                      .at(i - mObj.components.txLine.at(j.index).k);
//           RHSvalue =
//               mObj.components.txLine.at(j.index).value *
//                   results.xVect
//                       .at(std::distance(
//                           mObj.relXInd.begin(),
//                           std::find(
//                               mObj.relXInd.begin(), mObj.relXInd.end(),
//                               mObj.components.txLine.at(j.index).curN1Row)))
//                       .at(i - mObj.components.txLine.at(j.index).k) +
//               VB;
//         }
//       }
//       RHS.push_back(RHSvalue);
//       rowCounter++;
//     }

//     LHS_PRE = RHS;
//     ok =
//         klu_tsolve(Symbolic, Numeric, mObj.Nsize, 1, &LHS_PRE.front(), &Common);
//     if (!ok) {
//       Errors::simulation_errors(MATRIX_SINGULAR, "");
//     }

//     lhsValues = LHS_PRE;
//     for (int m = 0; m < mObj.relXInd.size(); m++) {
//       results.xVect.at(m).at(i) = lhsValues.at(mObj.relXInd.at(m));
//     }

//     /* Guess next junction voltage */
//     for (int j = 0; j < mObj.components.voltJJ.size(); j++) {
//       jj_volt &thisJunction = mObj.components.voltJJ.at(j);
//       if (thisJunction.posNRow == -1)
//         thisJunction.vn1 = (-lhsValues.at(thisJunction.negNRow));
//       else if (thisJunction.negNRow == -1)
//         thisJunction.vn1 = (lhsValues.at(thisJunction.posNRow));
//       else
//         thisJunction.vn1 = (lhsValues.at(thisJunction.posNRow) -
//                             lhsValues.at(thisJunction.negNRow));
//       if (i <= 3)
//         thisJunction.dVn1 = 0;
//       else
//         thisJunction.dVn1 =
//             (2 / iObj.transSim.prstep) * (thisJunction.vn1 - thisJunction.vn2) -
//             thisJunction.dVn2;
//       thisJunction.v0 =
//           thisJunction.vn1 + iObj.transSim.prstep * thisJunction.dVn1;
//       if (thisJunction.rType == 1) {
//         if (fabs(thisJunction.v0) < thisJunction.lowerB) {
//           thisJunction.iT = 0.0;
//           if (thisJunction.ppPtr != -1) {
//             if (mObj.mElements.at(thisJunction.ppPtr).value !=
//                 thisJunction.subCond) {
//               mObj.mElements.at(thisJunction.ppPtr).value =
//                   thisJunction.subCond;
//               needsLU = true;
//             }
//           }
//           if (thisJunction.nnPtr != -1) {
//             if (mObj.mElements.at(thisJunction.nnPtr).value !=
//                 thisJunction.subCond) {
//               mObj.mElements.at(thisJunction.nnPtr).value =
//                   thisJunction.subCond;
//               needsLU = true;
//             }
//           }
//           if (thisJunction.pnPtr != -1) {
//             if (mObj.mElements.at(thisJunction.pnPtr).value !=
//                 -thisJunction.subCond) {
//               mObj.mElements.at(thisJunction.pnPtr).value =
//                   -thisJunction.subCond;
//               needsLU = true;
//             }
//           }
//           if (thisJunction.npPtr != -1) {
//             if (mObj.mElements.at(thisJunction.npPtr).value !=
//                 -thisJunction.subCond) {
//               mObj.mElements.at(thisJunction.npPtr).value =
//                   -thisJunction.subCond;
//               needsLU = true;
//             }
//           }
//         } else if (fabs(thisJunction.v0) < thisJunction.upperB) {
//           if (thisJunction.v0 < 0)
//             thisJunction.iT = -thisJunction.lowerB *
//                               ((1 / thisJunction.r0) - thisJunction.gLarge);
//           else
//             thisJunction.iT = thisJunction.lowerB *
//                               ((1 / thisJunction.r0) - thisJunction.gLarge);
//           if (thisJunction.ppPtr != -1) {
//             if (mObj.mElements.at(thisJunction.ppPtr).value !=
//                 thisJunction.transCond) {
//               mObj.mElements.at(thisJunction.ppPtr).value =
//                   thisJunction.transCond;
//               needsLU = true;
//             }
//           }
//           if (thisJunction.nnPtr != -1) {
//             if (mObj.mElements.at(thisJunction.nnPtr).value !=
//                 thisJunction.transCond) {
//               mObj.mElements.at(thisJunction.nnPtr).value =
//                   thisJunction.transCond;
//               needsLU = true;
//             }
//           }
//           if (thisJunction.pnPtr != -1) {
//             if (mObj.mElements.at(thisJunction.pnPtr).value !=
//                 thisJunction.transCond) {
//               mObj.mElements.at(thisJunction.pnPtr).value =
//                   -thisJunction.transCond;
//               needsLU = true;
//             }
//           }
//           if (thisJunction.npPtr != -1) {
//             if (mObj.mElements.at(thisJunction.npPtr).value !=
//                 thisJunction.transCond) {
//               mObj.mElements.at(thisJunction.npPtr).value =
//                   -thisJunction.transCond;
//               needsLU = true;
//             }
//           }
//         } else {
//           if (thisJunction.v0 < 0)
//             thisJunction.iT = -(thisJunction.iC / thisJunction.iCFact +
//                                 thisJunction.vG * (1 / thisJunction.r0) -
//                                 thisJunction.lowerB * (1 / thisJunction.rN));
//           else
//             thisJunction.iT = (thisJunction.iC / thisJunction.iCFact +
//                                thisJunction.vG * (1 / thisJunction.r0) -
//                                thisJunction.lowerB * (1 / thisJunction.rN));
//           if (thisJunction.ppPtr != -1) {
//             if (mObj.mElements.at(thisJunction.ppPtr).value !=
//                 thisJunction.normalCond) {
//               mObj.mElements.at(thisJunction.ppPtr).value =
//                   thisJunction.normalCond;
//               needsLU = true;
//             }
//           }
//           if (thisJunction.nnPtr != -1) {
//             if (mObj.mElements.at(thisJunction.nnPtr).value !=
//                 thisJunction.normalCond) {
//               mObj.mElements.at(thisJunction.nnPtr).value =
//                   thisJunction.normalCond;
//               needsLU = true;
//             }
//           }
//           if (thisJunction.pnPtr != -1) {
//             if (mObj.mElements.at(thisJunction.pnPtr).value !=
//                 thisJunction.normalCond) {
//               mObj.mElements.at(thisJunction.pnPtr).value =
//                   -thisJunction.normalCond;
//               needsLU = true;
//             }
//           }
//           if (thisJunction.npPtr != -1) {
//             if (mObj.mElements.at(thisJunction.npPtr).value !=
//                 thisJunction.normalCond) {
//               mObj.mElements.at(thisJunction.npPtr).value =
//                   -thisJunction.normalCond;
//               needsLU = true;
//             }
//           }
//         }
//       }
//       thisJunction.pn1 = lhsValues.at(thisJunction.phaseNRow);
//       thisJunction.phi0 = thisJunction.pn1 +
//                           (hn_2_2e_hbar) * (thisJunction.vn1 + thisJunction.v0);
//       // thisJunction.iS = -thisJunction.iC * sin(thisJunction.phi0) + (((2 *
//       // thisJunction.C) / iObj.transSim.prstep)*thisJunction.vn1) +
//       // (thisJunction.C * thisJunction.dVn1) - thisJunction.iT;
//       thisJunction.iS =
//           -((M_PI * thisJunction.Del) / (2 * EV * thisJunction.rNCalc)) *
//               (sin(thisJunction.phi0) /
//                sqrt(1 - thisJunction.D * (sin(thisJunction.phi0 / 2) *
//                                           sin(thisJunction.phi0 / 2)))) *
//               tanh((thisJunction.Del) / (2 * BOLTZMANN * thisJunction.T) *
//                    sqrt(1 - thisJunction.D * (sin(thisJunction.phi0 / 2) *
//                                               sin(thisJunction.phi0 / 2)))) +
//           (((2 * thisJunction.C) / iObj.transSim.prstep) * thisJunction.vn1) +
//           (thisJunction.C * thisJunction.dVn1) - thisJunction.iT;
//       thisJunction.vn2 = thisJunction.vn1;
//       thisJunction.dVn2 = thisJunction.dVn1;
//       thisJunction.pn2 = thisJunction.pn1;
//       // mObj.components.voltJJ.at(j.first) = thisJunction;
//       mObj.components.voltJJ.at(j).jjCur.push_back(thisJunction.iS);
//     }
//     if (needsLU) {
//       mObj.create_CSR();

//       // TODO: Maybe use refactor
//       klu_free_numeric(&Numeric, &Common);
//       Numeric = klu_factor(&mObj.rowptr.front(), &mObj.colind.front(),
//                            &mObj.nzval.front(), Symbolic, &Common);
//       needsLU = false;
//     }
//     results.timeAxis.push_back(i * iObj.transSim.prstep);
//     old_progress = progress;
//     incremental_progress = incremental_progress + increments;
//     progress = (int)(incremental_progress);
// #ifndef NO_PRINT
//     if (progress > old_progress) {
//       if (sOutput)
//         std::cout << std::setw(3) << std::right << std::fixed
//                   << std::setprecision(0) << progress << "%";
//       pBar = "[";
//       for (int p = 0; p <= (int)(progress_increments * i); p++) {
//         pBar.append("=");
//       }
//       if (sOutput)
//         std::cout << std::setw(31) << std::left << pBar << "]" << std::flush;
//     }
// #endif
//   }
// #ifndef NO_PRINT
//   if (sOutput)
//     std::cout << "\r" << std::setw(3) << std::right << std::fixed
//               << std::setprecision(0) << 100 << "%" << std::setw(31)
//               << std::left << pBar << "]\n" << std::flush;
// #else
//   if (sOutput)
//     std::cout << " done" << std::flush << std::endl;
// #endif

//   klu_free_symbolic(&Symbolic, &Common);
//   klu_free_numeric(&Numeric, &Common);
// }

// void Simulation::transient_phase_simulation(Input &iObj, Matrix &mObj) {
//   results.xVect.clear();
//   results.timeAxis.clear();
//   std::vector<double> lhsValues(mObj.Nsize, 0.0),
//       RHS(mObj.columnNames.size(), 0.0), LHS_PRE;
//   int simSize = iObj.transSim.simsize();
//   for (int m = 0; m < mObj.relXInd.size(); m++) {
//     results.xVect.emplace_back(std::vector<double>(simSize, 0.0));
//   }
//   // std::vector<std::vector<std::string>> nodeConnectionVector(
//   //     mObj.rowNames.size());
//   std::string currentLabel;
//   double RHSvalue;
//   double hn_2_2e_hbar = (iObj.transSim.prstep / 2) * (2 * M_PI / PHI_ZERO);
//   int ok, rowCounter;
//   bool needsLU = false;
//   klu_symbolic *Symbolic;
//   klu_common Common;
//   klu_numeric *Numeric;
//   ok = klu_defaults(&Common);
//   assert(ok);
//   Symbolic = klu_analyze(mObj.Nsize, &mObj.rowptr.front(), &mObj.colind.front(),
//                          &Common);
//   Numeric = klu_factor(&mObj.rowptr.front(), &mObj.colind.front(),
//                        &mObj.nzval.front(), Symbolic, &Common);
//   rowCounter = 0;
//   // for (auto j : mObj.rowNames) {
//   // 	if (j[2] == 'N') {
//   // 		nodeConnectionVector[rowCounter] = mObj.nodeConnections[j];
//   // 	}
//   // 	rowCounter++;
//   // }
//   if (sOutput)
//     std::cout << "Simulating:" << std::endl;
//   double increments = 100 / (double)simSize;
//   double progress_increments = 30 / (double)simSize;
//   double incremental_progress = 0.0;

//   int progress = 0;
//   int old_progress = 0;
//   int imintd = 0;
//   std::string pBar;
//   double mutualL = 0.0;
//   double CUR2 = 0.0;
//   for (int i = 0; i < simSize; i++) {
// #ifndef NO_PRINT
//     if (sOutput)
//       std::cout << '\r' << std::flush;
// #endif
//     RHS.clear();
//     rowCounter = 0;
//     for (auto j : mObj.rowDesc) {
//       RHSvalue = 0.0;
//       if (j.type == RowDescriptor::Type::PhaseNode) {
//         for (auto k : mObj.nodeConnections.at(j.index).connections) {
//           if (k.type == ComponentConnections::Type::JJP) {
//             RHSvalue += mObj.components.phaseJJ.at(k.index).iS;
//           } else if (k.type == ComponentConnections::Type::JJN) {
//             RHSvalue -= mObj.components.phaseJJ.at(k.index).iS;
//           } else if (k.type == ComponentConnections::Type::CSN) {
//             RHSvalue += mObj.sources.at(k.index).at(i);
//           } else if (k.type == ComponentConnections::Type::CSP) {
//             RHSvalue -= mObj.sources.at(k.index).at(i);
//           }
//         }
//       } else if (j.type == RowDescriptor::Type::PhaseResistor) {
//         if (mObj.components.phaseRes.at(j.index).posNRow == -1) {
//           mObj.components.phaseRes.at(j.index).pn1 =
//               -lhsValues.at(mObj.components.phaseRes.at(j.index).negNRow);
//         } else if (mObj.components.phaseRes.at(j.index).negNRow == -1) {
//           mObj.components.phaseRes.at(j.index).pn1 =
//               lhsValues.at(mObj.components.phaseRes.at(j.index).posNRow);
//         } else {
//           mObj.components.phaseRes.at(j.index).pn1 =
//               lhsValues.at(mObj.components.phaseRes.at(j.index).posNRow) -
//               lhsValues.at(mObj.components.phaseRes.at(j.index).negNRow);
//         }
//         mObj.components.phaseRes.at(j.index).IRn1 =
//             lhsValues.at(mObj.components.phaseRes.at(j.index).curNRow);
//         RHSvalue += ((M_PI * mObj.components.phaseRes.at(j.index).value *
//                       iObj.transSim.prstep) /
//                      PHI_ZERO) *
//                         mObj.components.phaseRes.at(j.index).IRn1 +
//                     mObj.components.phaseRes.at(j.index).pn1;
//       } else if (j.type == RowDescriptor::Type::PhaseJJ) {
//         RHSvalue = mObj.components.phaseJJ.at(j.index).pn1 +
//                    hn_2_2e_hbar * mObj.components.phaseJJ.at(j.index).vn1;
//       } else if (j.type == RowDescriptor::Type::PhaseCapacitor) {
//         RHSvalue =
//             -((2 * M_PI * iObj.transSim.prstep * iObj.transSim.prstep) /
//               (4 * PHI_ZERO * mObj.components.phaseCap.at(j.index).value)) *
//                 mObj.components.phaseCap.at(j.index).ICn1 -
//             mObj.components.phaseCap.at(j.index).pn1 -
//             (iObj.transSim.prstep * mObj.components.phaseCap.at(j.index).dPn1);
//       } else if (j.type == RowDescriptor::Type::PhaseVS) {
//         if (mObj.components.phaseVs.at(j.index).posNRow == -1.0)
//           mObj.components.phaseVs.at(j.index).pn1 =
//               -lhsValues.at(mObj.components.phaseVs.at(j.index).negNRow);
//         else if (mObj.components.phaseVs.at(j.index).negNRow == -1.0)
//           mObj.components.phaseVs.at(j.index).pn1 =
//               lhsValues.at(mObj.components.phaseVs.at(j.index).posNRow);
//         else
//           mObj.components.phaseVs.at(j.index).pn1 =
//               lhsValues.at(mObj.components.phaseVs.at(j.index).posNRow) -
//               lhsValues.at(mObj.components.phaseVs.at(j.index).negNRow);
//         if (i >= 1)
//           RHSvalue =
//               mObj.components.phaseVs.at(j.index).pn1 +
//               ((iObj.transSim.prstep * M_PI) / PHI_ZERO) *
//                   (mObj.sources.at(mObj.components.phaseVs.at(j.index).sourceDex).at(i)
//                   + mObj.sources.at(mObj.components.phaseVs.at(j.index).sourceDex).at(i - 1));
//         else if (i == 0)
//           RHSvalue = mObj.components.phaseVs.at(j.index).pn1 +
//                      ((iObj.transSim.prstep * M_PI) / PHI_ZERO) *
//                          mObj.sources.at(mObj.components.phaseVs.at(j.index).sourceDex).at(i);
//       } else if (j.type == RowDescriptor::Type::PhasePS) {
//         RHSvalue = mObj.sources.at(j.index).at(i);
//       } else if (j.type == RowDescriptor::Type::PhaseTX1) {
//         if (i > mObj.components.txPhase.at(j.index).k) {
//           RHSvalue =
//               ((iObj.transSim.prstep * M_PI *
//                 mObj.components.txPhase.at(j.index).value) /
//                (PHI_ZERO)) *
//                   results.xVect
//                       .at(std::distance(
//                           mObj.relXInd.begin(),
//                           std::find(mObj.relXInd.begin(), mObj.relXInd.end(),
//                                     mObj.components.txPhase.at(j.index).curN2Row)))
//                       .at(i - mObj.components.txPhase.at(j.index).k) +
//               mObj.components.txPhase.at(j.index).p1n1 +
//               (iObj.transSim.prstep / 2) *
//                   (mObj.components.txPhase.at(j.index).dP1n1 +
//                    mObj.components.txPhase.at(j.index).dP2nk);
//         }
//       } else if (j.type == RowDescriptor::Type::PhaseTX2) {
//         if (i > mObj.components.txPhase.at(j.index).k) {
//           RHSvalue =
//               ((iObj.transSim.prstep * M_PI *
//                 mObj.components.txPhase.at(j.index).value) /
//                (PHI_ZERO)) *
//                   results.xVect
//                       .at(std::distance(
//                           mObj.relXInd.begin(),
//                           std::find(mObj.relXInd.begin(), mObj.relXInd.end(),
//                                     mObj.components.txPhase.at(j.index).curN1Row)))
//                       .at(i - mObj.components.txPhase.at(j.index).k) +
//               mObj.components.txPhase.at(j.index).p2n1 +
//               (iObj.transSim.prstep / 2) *
//                   (mObj.components.txPhase.at(j.index).dP2n1 +
//                    mObj.components.txPhase.at(j.index).dP1nk);
//         }
//       }
//       RHS.push_back(RHSvalue);
//       rowCounter++;
//     }
//     LHS_PRE = RHS;
//     ok =
//         klu_tsolve(Symbolic, Numeric, mObj.Nsize, 1, &LHS_PRE.front(), &Common);
//     if (!ok) {
//       Errors::simulation_errors(MATRIX_SINGULAR, "");
//     }

//     lhsValues = LHS_PRE;
//     for (int m = 0; m < mObj.relXInd.size(); m++) {
//       results.xVect.at(m).at(i) = lhsValues.at(mObj.relXInd.at(m));
//     }

//     for (int j = 0; j < mObj.components.phaseJJ.size(); j++) {
//       jj_phase &thisJJ = mObj.components.phaseJJ.at(j);
//       if (thisJJ.posNRow == -1)
//         thisJJ.pn1 = (-lhsValues.at(thisJJ.negNRow));
//       else if (thisJJ.negNRow == -1)
//         thisJJ.pn1 = (lhsValues.at(thisJJ.posNRow));
//       else
//         thisJJ.pn1 =
//             (lhsValues.at(thisJJ.posNRow) - lhsValues.at(thisJJ.negNRow));
      
//       thisJJ.vn1 = (lhsValues.at(thisJJ.voltNRow));
//       thisJJ.dVn1 =
//           (2 / iObj.transSim.prstep) * (thisJJ.vn1 - thisJJ.vn2) - thisJJ.dVn2;
//       thisJJ.vn2 = thisJJ.vn1;
//       thisJJ.dVn2 = thisJJ.dVn1;
//       thisJJ.v0 = thisJJ.vn1 + iObj.transSim.prstep * thisJJ.dVn1;
//       thisJJ.dPn1 =
//           (2 / iObj.transSim.prstep) * (thisJJ.pn1 - thisJJ.pn2) - thisJJ.dPn2;
//       thisJJ.phi0 = thisJJ.pn1 + hn_2_2e_hbar * (thisJJ.vn1 + thisJJ.v0);
//       thisJJ.pn2 = thisJJ.pn1;
//       thisJJ.dPn2 = thisJJ.dPn1;
//       if (thisJJ.rType == 1) {
//         if (fabs(thisJJ.v0) < thisJJ.lowerB) {
//           thisJJ.iT = 0.0;
//           if (thisJJ.pPtr != -1) {
//             if (mObj.mElements[thisJJ.pPtr].value != thisJJ.subCond) {
//               mObj.mElements[thisJJ.pPtr].value = thisJJ.subCond;
//               needsLU = true;
//             }
//           }
//           if (thisJJ.nPtr != -1) {
//             if (mObj.mElements[thisJJ.nPtr].value != -thisJJ.subCond) {
//               mObj.mElements[thisJJ.nPtr].value = -thisJJ.subCond;
//               needsLU = true;
//             }
//           }
//         } else if (fabs(thisJJ.v0) < thisJJ.upperB) {
//           if (thisJJ.v0 < 0)
//             thisJJ.iT = -thisJJ.vG * ((1 / thisJJ.r0) - thisJJ.gLarge);
//           else
//             thisJJ.iT = thisJJ.vG * ((1 / thisJJ.r0) - thisJJ.gLarge);
//           if (thisJJ.pPtr != -1) {
//             if (mObj.mElements[thisJJ.pPtr].value != thisJJ.transCond) {
//               mObj.mElements[thisJJ.pPtr].value = thisJJ.transCond;
//               needsLU = true;
//             }
//           }
//           if (thisJJ.nPtr != -1) {
//             if (mObj.mElements[thisJJ.nPtr].value != -thisJJ.transCond) {
//               mObj.mElements[thisJJ.nPtr].value = -thisJJ.transCond;
//               needsLU = true;
//             }
//           }
//         } else {
//           if (thisJJ.v0 < 0)
//             thisJJ.iT = -((thisJJ.iC / thisJJ.iCFact) +
//                           ((1 / thisJJ.r0) * thisJJ.lowerB) -
//                           ((1 / thisJJ.rN) * thisJJ.upperB));
//           else
//             thisJJ.iT = ((thisJJ.iC / thisJJ.iCFact) +
//                          ((1 / thisJJ.r0) * thisJJ.lowerB) -
//                          ((1 / thisJJ.rN) * thisJJ.upperB));
//           if (thisJJ.pPtr != -1) {
//             if (mObj.mElements[thisJJ.pPtr].value != thisJJ.normalCond) {
//               mObj.mElements[thisJJ.pPtr].value = thisJJ.normalCond;
//               needsLU = true;
//             }
//           }
//           if (thisJJ.nPtr != -1) {
//             if (mObj.mElements[thisJJ.nPtr].value != -thisJJ.normalCond) {
//               mObj.mElements[thisJJ.nPtr].value = -thisJJ.normalCond;
//               needsLU = true;
//             }
//           }
//         }
//       }
//       // thisJJ.iS = -thisJJ.iC * sin(thisJJ.phi0) + ((2 * thisJJ.C) /
//       // iFile.tsim.prstep) * thisJJ.vn1 + thisJJ.C * thisJJ.dVn1 - thisJJ.It;
//       thisJJ.iS =
//           -((M_PI * thisJJ.Del) / (2 * EV * thisJJ.rNCalc)) *
//               (sin(thisJJ.phi0) / sqrt(1 - thisJJ.D * (sin(thisJJ.phi0 / 2) *
//                                                        sin(thisJJ.phi0 / 2)))) *
//               tanh((thisJJ.Del) / (2 * BOLTZMANN * thisJJ.T) *
//                    sqrt(1 - thisJJ.D * (sin(thisJJ.phi0 / 2) *
//                                         sin(thisJJ.phi0 / 2)))) +
//           (((2 * thisJJ.C) / iObj.transSim.prstep) * thisJJ.vn1) +
//           (thisJJ.C * thisJJ.dVn1) - thisJJ.iT;
//       thisJJ.jjCur.emplace_back(thisJJ.iS);
//     }
//     /* Calculate next Cap values */
//     for (int j = 0; j < mObj.components.phaseCap.size(); j++) {
//       cap_phase &thisCap = mObj.components.phaseCap.at(j);
//       thisCap.pn2 = thisCap.pn1;
//       if (thisCap.posNRow == -1)
//         thisCap.pn1 = (-lhsValues.at(thisCap.negNRow));
//       else if (thisCap.negNRow == -1)
//         thisCap.pn1 = (lhsValues.at(thisCap.posNRow));
//       else
//         thisCap.pn1 =
//             (lhsValues.at(thisCap.posNRow) - lhsValues.at(thisCap.negNRow));
//       thisCap.ICn1 = lhsValues.at(thisCap.curNRow);
//       thisCap.dPn1 = (2 / iObj.transSim.prstep) * (thisCap.pn1 - thisCap.pn2) -
//                      thisCap.dPn2;
//       thisCap.dPn2 = thisCap.dPn1;
//     }
//     /* Calculate next TL values */
//     for (int j = 0; j < mObj.components.txPhase.size(); j++) {
//       tx_phase &thisTL = mObj.components.txPhase.at(j);
//       if (thisTL.posNRow == -1) {
//         thisTL.p1n1 = (-lhsValues.at(thisTL.negNRow));
//       } else if (thisTL.negNRow == -1) {
//         thisTL.p1n1 = (lhsValues.at(thisTL.posNRow));
//       } else {
//         thisTL.p1n1 =
//             (lhsValues.at(thisTL.posNRow) - lhsValues.at(thisTL.negNRow));
//       }
//       if (thisTL.posN2Row == -1) {
//         thisTL.p2n1 = (-lhsValues.at(thisTL.negN2Row));
//       } else if (thisTL.negN2Row == -1) {
//         thisTL.p2n1 = (lhsValues.at(thisTL.posN2Row));
//       } else {
//         thisTL.p2n1 =
//             (lhsValues.at(thisTL.posN2Row) - lhsValues.at(thisTL.negN2Row));
//       }
//       thisTL.dP1n1 = (2 / iObj.transSim.prstep) * (thisTL.p1n1 - thisTL.p1n2) -
//                      thisTL.dP1n2;
//       thisTL.p1n2 = thisTL.p1n1;
//       thisTL.dP1n2 = thisTL.dP1n1;
//       thisTL.dP2n1 = (2 / iObj.transSim.prstep) * (thisTL.p2n1 - thisTL.p2n2) -
//                      thisTL.dP2n2;
//       thisTL.p2n2 = thisTL.p2n1;
//       thisTL.dP2n2 = thisTL.dP2n1;
//       if (i >= thisTL.k) {
//         if (thisTL.posNRow == -1) {
//           thisTL.p1nk = -results.xVect
//                              .at(std::distance(mObj.relXInd.begin(),
//                                                std::find(mObj.relXInd.begin(),
//                                                          mObj.relXInd.end(),
//                                                          thisTL.negNRow)))
//                              .at(i - thisTL.k);
//         } else if (thisTL.negNRow == -1) {
//           thisTL.p1nk = results.xVect
//                             .at(std::distance(mObj.relXInd.begin(),
//                                               std::find(mObj.relXInd.begin(),
//                                                         mObj.relXInd.end(),
//                                                         thisTL.posNRow)))
//                             .at(i - thisTL.k);
//         } else {
//           thisTL.p1nk = results.xVect
//                             .at(std::distance(mObj.relXInd.begin(),
//                                               std::find(mObj.relXInd.begin(),
//                                                         mObj.relXInd.end(),
//                                                         thisTL.posNRow)))
//                             .at(i - thisTL.k) -
//                         results.xVect
//                             .at(std::distance(mObj.relXInd.begin(),
//                                               std::find(mObj.relXInd.begin(),
//                                                         mObj.relXInd.end(),
//                                                         thisTL.negNRow)))
//                             .at(i - thisTL.k);
//         }
//         if (thisTL.posN2Row == -1) {
//           thisTL.p2nk = -results.xVect
//                              .at(std::distance(mObj.relXInd.begin(),
//                                                std::find(mObj.relXInd.begin(),
//                                                          mObj.relXInd.end(),
//                                                          thisTL.negN2Row)))
//                              .at(i - thisTL.k);
//         } else if (thisTL.negN2Row == -1) {
//           thisTL.p2nk = results.xVect
//                             .at(std::distance(mObj.relXInd.begin(),
//                                               std::find(mObj.relXInd.begin(),
//                                                         mObj.relXInd.end(),
//                                                         thisTL.posN2Row)))
//                             .at(i - thisTL.k);
//         } else {
//           thisTL.p2nk = results.xVect
//                             .at(std::distance(mObj.relXInd.begin(),
//                                               std::find(mObj.relXInd.begin(),
//                                                         mObj.relXInd.end(),
//                                                         thisTL.posN2Row)))
//                             .at(i - thisTL.k) -
//                         results.xVect
//                             .at(std::distance(mObj.relXInd.begin(),
//                                               std::find(mObj.relXInd.begin(),
//                                                         mObj.relXInd.end(),
//                                                         thisTL.negN2Row)))
//                             .at(i - thisTL.k);
//         }
//         thisTL.dP1nk =
//             (2 / iObj.transSim.prstep) * (thisTL.p1nk - thisTL.p1nk1) -
//             thisTL.dP1nk1;
//         thisTL.p1nk1 = thisTL.p1nk;
//         thisTL.dP1nk1 = thisTL.dP1nk;
//         thisTL.dP2nk =
//             (2 / iObj.transSim.prstep) * (thisTL.p2nk - thisTL.p2nk1) -
//             thisTL.dP2nk1;
//         thisTL.p2nk1 = thisTL.p2nk;
//         thisTL.dP2nk1 = thisTL.dP2nk;
//       }
//     }
//     if (needsLU) {
//       mObj.create_CSR();
//       // TODO: Maybe use refactor
//       klu_free_numeric(&Numeric, &Common);
//       Numeric = klu_factor(&mObj.rowptr.front(), &mObj.colind.front(),
//                            &mObj.nzval.front(), Symbolic, &Common);
//       needsLU = false;
//     }
//     /* Add the current time value to the time axis for plotting purposes */
//     results.timeAxis.push_back(i * iObj.transSim.prstep);
//     old_progress = progress;
//     incremental_progress = incremental_progress + increments;
//     progress = (int)(incremental_progress);
// #ifndef NO_PRINT
//     if (progress > old_progress) {
//       if (sOutput)
//         std::cout << std::setw(3) << std::right << std::fixed
//                   << std::setprecision(0) << progress << "%";
//       pBar = "[";
//       for (int p = 0; p <= (int)(progress_increments * i); p++) {
//         pBar.append("=");
//       }
//       if (sOutput)
//         std::cout << std::setw(31) << std::left << pBar << "]" << std::flush;
//     }
// #endif
//   }
// #ifndef NO_PRINT
//   if (sOutput)
//     std::cout << "\r" << std::setw(3) << std::right << std::fixed
//               << std::setprecision(0) << 100 << "%" << std::setw(31)
//               << std::left << pBar << "]\n" << std::flush;
// #else
//   if (sOutput)
//     std::cout << " done" << std::flush << std::endl;
// #endif

//   klu_free_symbolic(&Symbolic, &Common);
//   klu_free_numeric(&Numeric, &Common);
// }

template<JoSIM::AnalysisType AnalysisTypeValue>
void Simulation::trans_sim(Input &iObj, Matrix &mObj) {
  std::vector<double> lhsValues(mObj.Nsize, 0.0),
      RHS(mObj.Nsize, 0.0), LHS_PRE(mObj.Nsize, 0.0);
  int simSize = iObj.transSim.simsize();
  for (int m = 0; m < mObj.relXInd.size(); m++) {
    results.xVect.emplace_back(std::vector<double>(simSize, 0.0));
  }
  double hn_2_2e_hbar = (iObj.transSim.prstep / 2) * (2 * M_PI / PHI_ZERO);
  int ok, jjcount;
  bool needsLU = false;
  klu_symbolic *Symbolic;
  klu_common Common;
  klu_numeric *Numeric;
  ok = klu_defaults(&Common);
  assert(ok);
  Symbolic = klu_analyze(mObj.Nsize, &mObj.rowptr.front(), &mObj.colind.front(),
                         &Common);
  Numeric = klu_factor(&mObj.rowptr.front(), &mObj.colind.front(),
                       &mObj.nzval.front(), Symbolic, &Common);

  auto& jj_vector = [&]() -> auto& {
    if constexpr(AnalysisTypeValue == JoSIM::AnalysisType::Phase)
      return mObj.components.phaseJJ;
    else
      return mObj.components.voltJJ;
  }();
  
  for(int i = 0; i < simSize; i++) {
    for (auto j : mObj.rowDesc) {
      switch (j.type){
        case RowDescriptor::Type::VoltageNode: 
        case RowDescriptor::Type::PhaseNode: 
          for (auto k : mObj.nodeConnections.at(j.index).connections) {
            RHS.emplace_back(0.0);
            switch(k.type) {
              case ComponentConnections::Type::JJP: 
                  RHS.back() += jj_vector.at(k.index).iS;
                break;
              case ComponentConnections::Type::JJN: 
                  RHS.back() -= jj_vector.at(k.index).iS;
                break;
              case ComponentConnections::Type::CSP:
                RHS.back() += mObj.sources.at(k.index).at(i);
                break;
              case ComponentConnections::Type::CSN:
                RHS.back() -= mObj.sources.at(k.index).at(i);
                break;
            }
          }
          break;
        case RowDescriptor::Type::VoltageInductor:
          RHS.emplace_back(0.0);
          if (mObj.components.voltInd.at(j.index).posNRow == -1)
            RHS.back() += lhsValues.at(mObj.components.voltInd.at(j.index).negNRow);
          else if (mObj.components.voltInd.at(j.index).negNRow == -1)
            RHS.back() -= lhsValues.at(mObj.components.voltInd.at(j.index).posNRow);
          else
            RHS.back() -= (lhsValues.at(mObj.components.voltInd.at(j.index).posNRow) -
                lhsValues.at(mObj.components.voltInd.at(j.index).negNRow));
          RHS.back() += (-2 * mObj.components.voltInd.at(j.index).value /
              iObj.transSim.prstep) *
                  lhsValues.at(mObj.components.voltInd.at(j.index).curNRow);
          for (const auto &m : mObj.components.voltInd.at(j.index).mut) {
            RHS.back() -= (m.second *
                lhsValues.at(mObj.components.voltInd.at(m.first).curNRow));
          }
          break;
        case RowDescriptor::Type::VoltageJJ:
          RHS.emplace_back(mObj.components.voltJJ.at(j.index).pn1 +
                   hn_2_2e_hbar * mObj.components.voltJJ.at(j.index).vn1);
          break;
        case RowDescriptor::Type::VoltageVS:
        case RowDescriptor::Type::PhasePS:
          RHS.emplace_back(mObj.sources.at(j.index).at(i));
          break;
        case RowDescriptor::Type::VoltageTX1: {
          const auto &txline = mObj.components.txLine.at(j.index);
          if (i >= txline.k) {
            if (txline.posN2Row == -1)
              RHS.emplace_back(-results.xVect.at(mObj.relToXMap.at(txline.negNode2R)).at(i - txline.k));
            else if (txline.negN2Row == -1)
              RHS.emplace_back(results.xVect.at(mObj.relToXMap.at(txline.posNode2R)).at(i - txline.k));
            else
              RHS.emplace_back(results.xVect.at(mObj.relToXMap.at(txline.posNode2R)).at(i - txline.k)
                - results.xVect.at(mObj.relToXMap.at(txline.negNode2R)).at(i - txline.k));
            RHS.back() +=
                txline.value *
                    results.xVect
                        .at(mObj.relToXMap.at(txline.curNode2R))
                        .at(i - txline.k);
          }
          break; }
        case RowDescriptor::Type::VoltageTX2: {
          const auto &txline = mObj.components.txLine.at(j.index);
          if (i >= txline.k) {
            if (txline.posNRow == -1)
              RHS.emplace_back(-results.xVect.at(mObj.relToXMap.at(txline.negNodeR)).at(i - txline.k));
            else if (txline.negNRow == -1)
              RHS.emplace_back(results.xVect.at(mObj.relToXMap.at(txline.posNodeR)).at(i - txline.k));
            else
              RHS.emplace_back(results.xVect.at(mObj.relToXMap.at(txline.posNodeR)).at(i - txline.k)
                - results.xVect.at(mObj.relToXMap.at(txline.negNodeR)).at(i - txline.k));
            RHS.back() +=
                txline.value *
                    results.xVect
                        .at(mObj.relToXMap.at(txline.curNode1R))
                        .at(i - txline.k);
          }
          break; }
        case RowDescriptor::Type::PhaseResistor: {
          auto &presis = mObj.components.phaseRes.at(j.index);
          if(presis.posNRow == -1) presis.pn1 = -lhsValues.at(presis.negNRow);
          else if (presis.negNRow == -1) presis.pn1 = lhsValues.at(presis.posNRow);
          else presis.pn1 = lhsValues.at(presis.posNRow) - lhsValues.at(presis.negNRow);
          presis.IRn1 = lhsValues.at(presis.curNRow);
          RHS.emplace_back(((presis.value * iObj.transSim.prstep) / PHI_ZERO) * presis.IRn1 + presis.pn1);
          break; }
        case RowDescriptor::Type::PhaseJJ:
          RHS.emplace_back(mObj.components.phaseJJ.at(j.index).pn1 + hn_2_2e_hbar * mObj.components.phaseJJ.at(j.index).vn1);
          break;
        case RowDescriptor::Type::PhaseCapacitor:
          RHS.emplace_back(
            -((2 * M_PI * iObj.transSim.prstep * iObj.transSim.prstep) /
              (4 * PHI_ZERO * mObj.components.phaseCap.at(j.index).value)) *
                mObj.components.phaseCap.at(j.index).ICn1 -
            mObj.components.phaseCap.at(j.index).pn1 -
            (iObj.transSim.prstep * mObj.components.phaseCap.at(j.index).dPn1));
          break;
        case RowDescriptor::Type::PhaseVS: {
          auto &pvs = mObj.components.phaseVs.at(j.index);
          if (pvs.posNRow == -1.0) pvs.pn1 = -lhsValues.at(pvs.negNRow);
          else if (pvs.negNRow == -1.0) pvs.pn1 = lhsValues.at(pvs.posNRow);
          else pvs.pn1 = lhsValues.at(pvs.posNRow) - lhsValues.at(pvs.negNRow);
          if (i >= 1) 
            RHS.emplace_back(pvs.pn1 + ((iObj.transSim.prstep * M_PI) / PHI_ZERO) *
              (mObj.sources.at(pvs.sourceDex).at(i) + mObj.sources.at(pvs.sourceDex).at(i - 1)));
          else if (i == 0)
            RHS.emplace_back(pvs.pn1 + ((iObj.transSim.prstep * M_PI) / PHI_ZERO) *
              mObj.sources.at(pvs.sourceDex).at(i));
          break; }
        case RowDescriptor::Type::PhaseTX1: {
          const auto &txline = mObj.components.txPhase.at(j.index);
          if (i > txline.k)
            RHS.emplace_back(((iObj.transSim.prstep * M_PI * txline.value) / PHI_ZERO) *
              results.xVect.at(mObj.relToXMap.at(txline.curNode2R)).at(i - txline.k) +
              txline.p1n1 + (iObj.transSim.prstep / 2) * (txline.dP1n1 + txline.dP2nk));
          break; }
        case RowDescriptor::Type::PhaseTX2: {
          const auto &txline = mObj.components.txPhase.at(j.index);
          if (i > txline.k)
            RHS.emplace_back(((iObj.transSim.prstep * M_PI * txline.value) / PHI_ZERO) *
              results.xVect.at(mObj.relToXMap.at(txline.curNode1R)).at(i - txline.k) +
              txline.p2n1 + (iObj.transSim.prstep / 2) * (txline.dP2n1 + txline.dP1nk));
          break; }
      }
    }

    LHS_PRE = RHS;
    ok =
        klu_tsolve(Symbolic, Numeric, mObj.Nsize, 1, &LHS_PRE.front(), &Common);
    if (!ok)
      Errors::simulation_errors(MATRIX_SINGULAR, "");

    lhsValues = LHS_PRE;
    for (int m = 0; m < mObj.relXInd.size(); m++)
      results.xVect.at(m).at(i) = lhsValues.at(mObj.relXInd.at(m));

    
    for (int j = 0; j < jj_vector.size(); j++) {
      auto &jj = jj_vector.at(j);
      // V_n-1 or P_n-1
      if(jj.posNRow == -1) {
        if(AnalysisTypeValue == JoSIM::AnalysisType::Voltage) jj.vn1 = -lhsValues.at(jj.negNRow);
        else jj.pn1 = -lhsValues.at(jj.negNRow);
      } else if (jj.negNRow == -1) {
        if(AnalysisTypeValue == JoSIM::AnalysisType::Voltage) jj.vn1 = lhsValues.at(jj.posNRow);
        else jj.pn1 = lhsValues.at(jj.posNRow);
      } else {
        if(AnalysisTypeValue == JoSIM::AnalysisType::Voltage) jj.vn1 = lhsValues.at(jj.posNRow) - lhsValues.at(jj.negNRow);
        else jj.pn1 = lhsValues.at(jj.posNRow) - lhsValues.at(jj.negNRow);
      }
      // Prevent initial large derivitive when V_n-1 = 0
      // Otherwise: trapezoidal find dV_n-1
      if (i <= 3) jj.dVn1 = 0;
      else jj.dVn1 = (2 / iObj.transSim.prstep) * (jj.vn1 - jj.vn2) - jj.dVn2;
      // Guess voltage (V0)
      jj.v0 = jj.vn1 + iObj.transSim.prstep * jj.dVn1;
      // Handle Rtype=1
      if (jj.rType == 1) {
        if (fabs(jj.v0) < jj.lowerB) {
          jj.iT = 0.0;
          if (jj.ppPtr != -1) {
            if (mObj.mElements.at(jj.ppPtr).value != jj.subCond) {
              mObj.mElements.at(jj.ppPtr).value = jj.subCond;
              needsLU = true;
            }
          }
          if (jj.nnPtr != -1) {
            if (mObj.mElements.at(jj.nnPtr).value != jj.subCond) {
              mObj.mElements.at(jj.nnPtr).value = jj.subCond;
              needsLU = true;
            }
          }
          if (jj.pnPtr != -1) {
            if (mObj.mElements.at(jj.pnPtr).value != -jj.subCond) {
              mObj.mElements.at(jj.pnPtr).value = -jj.subCond;
              needsLU = true;
            }
          }
          if (jj.npPtr != -1) {
            if (mObj.mElements.at(jj.npPtr).value != -jj.subCond) {
              mObj.mElements.at(jj.npPtr).value = -jj.subCond;
              needsLU = true;
            }
          }
          if (jj.pPtr != -1) {
            if (mObj.mElements.at(jj.pPtr).value != jj.subCond) {
              mObj.mElements.at(jj.pPtr).value = jj.subCond;
              needsLU = true;
            }
          }
          if (jj.nPtr != -1) {
            if (mObj.mElements.at(jj.nPtr).value != -jj.subCond) {
              mObj.mElements.at(jj.nPtr).value = -jj.subCond;
              needsLU = true;
            }
          }
        } else if (fabs(jj.v0) < jj.upperB) {
          if (jj.v0 < 0) jj.iT = -jj.lowerB * ((1 / jj.r0) - jj.gLarge);
          else jj.iT = jj.lowerB * ((1 / jj.r0) - jj.gLarge);
          if (jj.ppPtr != -1) {
            if (mObj.mElements.at(jj.ppPtr).value != jj.transCond) {
              mObj.mElements.at(jj.ppPtr).value = jj.transCond;
              needsLU = true;
            }
          }
          if (jj.nnPtr != -1) {
            if (mObj.mElements.at(jj.nnPtr).value != jj.transCond) {
              mObj.mElements.at(jj.nnPtr).value = jj.transCond;
              needsLU = true;
            }
          }
          if (jj.pnPtr != -1) {
            if (mObj.mElements.at(jj.pnPtr).value != jj.transCond) {
              mObj.mElements.at(jj.pnPtr).value = -jj.transCond;
              needsLU = true;
            }
          }
          if (jj.npPtr != -1) {
            if (mObj.mElements.at(jj.npPtr).value != jj.transCond) {
              mObj.mElements.at(jj.npPtr).value = -jj.transCond;
              needsLU = true;
            }
          }
          if (jj.pPtr != -1) {
            if (mObj.mElements.at(jj.pPtr).value != jj.transCond) {
              mObj.mElements.at(jj.pPtr).value = jj.transCond;
              needsLU = true;
            }
          }
          if (jj.nPtr != -1) {
            if (mObj.mElements.at(jj.pnPtr).value != -jj.transCond) {
              mObj.mElements.at(jj.pnPtr).value = -jj.transCond;
              needsLU = true;
            }
          }
        } else {
          if (jj.v0 < 0) 
            jj.iT = -(jj.iC / jj.iCFact + jj.vG * (1 / jj.r0) - jj.lowerB * (1 / jj.rN));
          else 
            jj.iT = (jj.iC / jj.iCFact + jj.vG * (1 / jj.r0) - jj.lowerB * (1 / jj.rN));
          if (jj.ppPtr != -1) {
            if (mObj.mElements.at(jj.ppPtr).value != jj.normalCond) {
              mObj.mElements.at(jj.ppPtr).value = jj.normalCond;
              needsLU = true;
            }
          }
          if (jj.nnPtr != -1) {
            if (mObj.mElements.at(jj.nnPtr).value != jj.normalCond) {
              mObj.mElements.at(jj.nnPtr).value = jj.normalCond;
              needsLU = true;
            }
          }
          if (jj.pnPtr != -1) {
            if (mObj.mElements.at(jj.pnPtr).value != jj.normalCond) {
              mObj.mElements.at(jj.pnPtr).value = -jj.normalCond;
              needsLU = true;
            }
          }
          if (jj.npPtr != -1) {
            if (mObj.mElements.at(jj.npPtr).value != jj.normalCond) {
              mObj.mElements.at(jj.npPtr).value = -jj.normalCond;
              needsLU = true;
            }
          }
          if (jj.pPtr != -1) {
            if (mObj.mElements.at(jj.pPtr).value != jj.normalCond) {
              mObj.mElements.at(jj.pPtr).value = jj.normalCond;
              needsLU = true;
            }
          }
          if (jj.nPtr != -1) {
            if (mObj.mElements.at(jj.nPtr).value != -jj.normalCond) {
              mObj.mElements.at(jj.nPtr).value = -jj.normalCond;
              needsLU = true;
            }
          }
        }
      }
      // Phase_n-1 or V_n-1
      if(AnalysisTypeValue == JoSIM::AnalysisType::Voltage) jj.pn1 = lhsValues.at(jj.phaseNRow);
      else jj.vn1 = lhsValues.at(jj.voltNRow);
      // Phase guess (P0)
      jj.phi0 = jj.pn1 + (hn_2_2e_hbar) * (jj.vn1 + jj.v0);
      // Junction current (Is)
      jj.iS = -((M_PI * jj.Del) / (2 * EV * jj.rNCalc)) *
              (sin(jj.phi0) / sqrt(1 - jj.D * (sin(jj.phi0 / 2) *
                sin(jj.phi0 / 2)))) * tanh((jj.Del) / (2 * BOLTZMANN * jj.T) *
                sqrt(1 - jj.D * (sin(jj.phi0 / 2) * sin(jj.phi0 / 2)))) +
              (((2 * jj.C) / iObj.transSim.prstep) * jj.vn1) + (jj.C * jj.dVn1) - jj.iT;
      // Set previous values
      jj.vn2 = jj.vn1;
      jj.dVn2 = jj.dVn1;
      jj.pn2 = jj.pn1;
      // Store current
      jj.jjCur.push_back(jj.iS);
    }

    for (int j = 0; j < mObj.components.phaseCap.size(); j++) {
      cap_phase &c = mObj.components.phaseCap.at(j);
      c.pn2 = c.pn1;
      if (c.posNRow == -1) c.pn1 = (-lhsValues.at(c.negNRow));
      else if (c.negNRow == -1) c.pn1 = (lhsValues.at(c.posNRow));
      else c.pn1 = lhsValues.at(c.posNRow) - lhsValues.at(c.negNRow);
      c.ICn1 = lhsValues.at(c.curNRow);
      c.dPn1 = (2 / iObj.transSim.prstep) * (c.pn1 - c.pn2) - c.dPn2;
      c.dPn2 = c.dPn1;
    }

    for (int j = 0; j < mObj.components.txPhase.size(); j++) {
      tx_phase &tl = mObj.components.txPhase.at(j);
       
      if (tl.posNRow == -1) tl.p1n1 = -lhsValues.at(tl.negNRow);
      else if (tl.negNRow == -1) tl.p1n1 = lhsValues.at(tl.posNRow);
      else tl.p1n1 = lhsValues.at(tl.posNRow) - lhsValues.at(tl.negNRow);
      
      if (tl.posN2Row == -1) tl.p2n1 = -lhsValues.at(tl.negN2Row);
      else if (tl.negN2Row == -1) tl.p2n1 = lhsValues.at(tl.posN2Row);
      else tl.p2n1 = lhsValues.at(tl.posN2Row) - lhsValues.at(tl.negN2Row);

      tl.dP1n1 = (2 / iObj.transSim.prstep) * (tl.p1n1 - tl.p1n2) - tl.dP1n2;
      tl.p1n2 = tl.p1n1;
      tl.dP1n2 = tl.dP1n1;

      tl.dP2n1 = (2 / iObj.transSim.prstep) * (tl.p2n1 - tl.p2n2) - tl.dP2n2;
      tl.p2n2 = tl.p2n1;
      tl.dP2n2 = tl.dP2n1;
      
      if (i >= tl.k) {
        if (tl.posNRow == -1) tl.p1nk = -results.xVect.at(mObj.relToXMap.at(tl.negNodeR)).at(i - tl.k);
        else if (tl.negNRow == -1) tl.p1nk = results.xVect.at(mObj.relToXMap.at(tl.posNodeR)).at(i - tl.k);
        else tl.p1nk = results.xVect.at(mObj.relToXMap.at(tl.posNodeR)).at(i - tl.k)
          - results.xVect.at(mObj.relToXMap.at(tl.negNodeR)).at(i - tl.k);

        if (tl.posN2Row == -1) tl.p1nk = -results.xVect.at(mObj.relToXMap.at(tl.negNode2R)).at(i - tl.k);
        else if (tl.negN2Row == -1) tl.p1nk = results.xVect.at(mObj.relToXMap.at(tl.posNode2R)).at(i - tl.k);
        else tl.p1nk = results.xVect.at(mObj.relToXMap.at(tl.posNode2R)).at(i - tl.k)
          - results.xVect.at(mObj.relToXMap.at(tl.negNode2R)).at(i - tl.k);

        tl.dP1nk = (2 / iObj.transSim.prstep) * (tl.p1nk - tl.p1nk1) - tl.dP1nk1;
        tl.p1nk1 = tl.p1nk;
        tl.dP1nk1 = tl.dP1nk;

        tl.dP2nk = (2 / iObj.transSim.prstep) * (tl.p2nk - tl.p2nk1) - tl.dP2nk1;
        tl.p2nk1 = tl.p2nk;
        tl.dP2nk1 = tl.dP2nk;
      }
    }

    if (needsLU) {
      mObj.create_CSR();
      klu_free_numeric(&Numeric, &Common);
      Numeric = klu_factor(&mObj.rowptr.front(), &mObj.colind.front(),
                           &mObj.nzval.front(), Symbolic, &Common);
      needsLU = false;
    }

    results.timeAxis.push_back(i * iObj.transSim.prstep);
  }
  klu_free_symbolic(&Symbolic, &Common);
  klu_free_numeric(&Numeric, &Common);
}

// Compile template for classes
template void Simulation::trans_sim<JoSIM::AnalysisType::Voltage>(Input &iObj, Matrix &mObj);
template void Simulation::trans_sim<JoSIM::AnalysisType::Phase>(Input &iObj, Matrix &mObj);
