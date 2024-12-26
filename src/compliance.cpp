#include "compliance.h"

void Compliance::initializeThresholds() {
  thresholds = {
      // Fluorinated compounds
      {"PFAS", {0.07, 0.1}},
      {"PFOS", {0.07, 0.1}},
      {"PFOA", {0.07, 0.1}},
      {"PFBS", {0.07, 0.1}},
      {"PFHxS", {0.07, 0.1}},

      // Chlorinated compounds
      {"1,1,2-Trichloroethane", {0.15, 0.2}},
      {"Chloroform", {0.25, 0.3}},
      {"Carbon Tet", {0.8, 1.2}},
      {"TetClEthene", {0.8, 1.0}},
      {"Trichloroeth", {0.8, 1.0}},

      // Persistent Organic Pollutants (POPs)
      {"PCBs", {0.03, 0.05}},
      {"Aldrin", {0.005, 0.01}},
      {"Dieldrin", {0.005, 0.01}},
      {"Endrin", {0.005, 0.01}},
      {"DDT", {0.015, 0.025}},
      {"Hexachlorbnz", {0.005, 0.01}},
      {"HCH Alpha", {0.002, 0.004}},
      {"HCH Beta", {0.002, 0.004}},
      {"HCH Gamma", {0.002, 0.004}},
      {"HCH Delta", {0.002, 0.004}},
      {"Heptachlor", {0.0001, 0.0003}},

      // Heavy Metals (Filtered)
      {"Pb Filtered", {1.2, 1.5}},
      {"Hg Filtered", {0.04, 0.07}},
      {"Cd Filtered", {0.08, 0.15}},
      {"Cr- Filtered", {3.0, 5.0}},
      {"Cu Filtered", {1.5, 2.0}},
      {"Zn- Filtered", {8.0, 10.0}},
      {"As-Filtered", {5.0, 7.5}},
      {"Al- Filtered", {15.0, 20.0}},
      {"Ni- Filtered", {4.0, 6.0}},

      // Aromatic Compounds
      {"Benzene", {0.8, 1.0}},
      {"Toluene", {2.0, 3.0}},
      {"Ethylbenzene", {2.0, 3.0}},
      {"o-Xylene", {2.0, 3.0}},
      {"m-p-Xylene", {2.0, 3.0}},

      // PAHs (Polycyclic Aromatic Hydrocarbons)
      {"Naphthalene", {1.0, 2.0}},
      {"Anthracene", {0.1, 0.15}},
      {"Fluoranthene", {0.06, 0.12}},
      {"B-[a]-pyrene", {0.05, 0.1}},

      // Nutrients
      {"Nitrogen - N", {2000, 2500}},
      {"Nitrate-N", {1500, 2000}},
      {"Nitrite-N", {30, 50}},
      {"Orthophospht", {20, 35}},
      {"Phosphorus-P", {25, 40}},
      {"NH3 filt N", {15, 25}},
      {"N Oxidised", {1500, 2000}},

      // Pesticides and Herbicides
      {"Simazine", {0.5, 1.0}},
      {"Atrazine", {0.5, 1.0}},
      {"Diuron", {0.2, 0.4}},
      {"Isoproturon", {0.3, 0.6}},
      {"Chlortoluron", {0.1, 0.2}},
      {"Linuron", {0.1, 0.2}},

      // Phenols
      {"Phenol", {7.7, 10.0}},
      {"4-nonPhenol", {0.3, 0.4}},

      // Industrial Chemicals
      {"Chlorobenzene", {1.0, 2.0}},
      {"1,2-DCB", {1.0, 2.0}},
      {"1,3-DCB", {1.0, 2.0}},
      {"1,4Dichlrbnzn", {1.0, 2.0}},

      // Brominated Flame Retardants
      {"PBDE 28", {0.0001, 0.0002}},
      {"PBDE 47", {0.0001, 0.0002}},
      {"PBDE 99", {0.0001, 0.0002}},
      {"PBDE 100", {0.0001, 0.0002}},
      {"PBDE 153", {0.0001, 0.0002}},
      {"PBDE 154", {0.0001, 0.0002}},

      {"pH", {9.0, 10.0}},

      {"O Diss \%sat", {120.0, 130.0}},

      {"Temp Water", {10000.0, 10000.0}},  // not a pollutant so no compliant just setting to a high value

      // Cyanide
      {"CN Free", {1.0, 4.0}},
      {"Cyanide - CN", {1.0, 4.0}}};
}

double Compliance::getWarningThreshold(const QString& pollutant) const {
  return thresholds.value(pollutant).warningLevel;
}

double Compliance::getCriticalThreshold(const QString& pollutant) const {
  double criticalLevel = thresholds.value(pollutant).criticalLevel;
  return criticalLevel == 0 ? 0.5 : criticalLevel;  // Return 0.1 if the critical level is 0
}

bool Compliance::isCompliant(const QString& pollutant, double value) const {
  auto info = thresholds.value(pollutant);
  return value <= info.criticalLevel;
}