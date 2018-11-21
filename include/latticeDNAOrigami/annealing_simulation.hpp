#ifndef ANNEALING_SIMULATION_H
#define ANNEALING_SIMULATION_H

#include "bias_functions.hpp"
#include "order_params.hpp"
#include "origami_system.hpp"
#include "simulation.hpp"

namespace annealing {

using biasFunctions::SystemBiases;
using orderParams::SystemOrderParams;
using origami::OrigamiSystem;
using parser::InputParameters;
using simulation::GCMCSimulation;

class AnnealingGCMCSimulation: public GCMCSimulation {
  public:
    AnnealingGCMCSimulation(
            OrigamiSystem& origami_system,
            SystemOrderParams& ops,
            SystemBiases& biases,
            InputParameters& params);
    void run();

  private:
    double m_max_temp;
    double m_min_temp;
    double m_temp_interval;
    unsigned long long m_steps_per_temp;
    double m_staple_M;
    bool m_constant_staple_M;

    void update_internal(unsigned long long) {};
};
} // namespace annealing

#endif // ANNEALING_SIMULATION_H