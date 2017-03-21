// parser.cpp

#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <sstream>

#include "parser.h"

namespace po = boost::program_options;

using std::ifstream;
using std::cout;

using namespace Parser;

// Couldn't easily template cause function changes (stoi vs stod)
vector<int> Parser::string_to_int_vector(string string_v) {
    string delim {" "};
    size_t start_pos {0};
    size_t delim_pos {string_v.find(delim)};
    vector<int> v {};
    while (delim_pos != string::npos) {
        v.push_back(stoi(string_v.substr(start_pos, delim_pos)));
        start_pos = delim_pos + 1;
        delim_pos = string_v.find(delim, start_pos);
    }
    v.push_back(stod(string_v.substr(start_pos, string_v.size())));

    return v;
}

vector<double> Parser::string_to_double_vector(string string_v) {
    string delim {" "};
    size_t start_pos {0};
    size_t delim_pos {string_v.find(delim)};
    vector<double> v {};
    while (delim_pos != string::npos) {
        v.push_back(stod(string_v.substr(start_pos, delim_pos)));
        start_pos = delim_pos + 1;
        delim_pos = string_v.find(delim, start_pos);
    }
    v.push_back(stod(string_v.substr(start_pos, string_v.size())));

    return v;
}

template<typename Out>
void Parser::split(const string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

vector<string> Parser::split(const string &s, char delim) {
    vector<string> elems;
    Parser::split(s, delim, std::back_inserter(elems));
    return elems;
}

Fraction::Fraction(string unparsed_fraction) {
    string delimiter {"/"};
    auto delim_pos {unparsed_fraction.find(delimiter)};

    // Assume is real number
    if (delim_pos == string::npos) {
        m_numerator = stod(unparsed_fraction);
        m_denominator = 1;
    }
    else {
        auto end_pos {unparsed_fraction.size()};
        m_numerator = stod(unparsed_fraction.substr(0, delim_pos));
        m_denominator = stod(unparsed_fraction.substr(delim_pos + 1, end_pos));
    }
    m_double_fraction = m_numerator / m_denominator;
}

InputParameters::InputParameters(int argc, char* argv[]) {

    // Command line parser for getting config file name
    po::options_description cl_desc {"Allowed options"};
    cl_desc.add_options()
        ("parameter_filename,i", po::value<string>())
        ;

    po::variables_map cl_vm;
    po::store(po::parse_command_line(argc, argv, cl_desc), cl_vm);
    po::notify(cl_vm);

    if (not cl_vm.count("parameter_filename")) {
        cout << "Input parameter file must be provided.\n";
        exit(1);
    }
    ifstream parameter_file {cl_vm["parameter_filename"].as<string>()};

    // Setup parser
    po::options_description desc {};
    desc.add_options()

        // System input and parameters
        ("origami_input_filename", po::value<string>(), "Origami input filename")
        ("temp", po::value<double>(), "System temperature (K)")
        ("staple_M", po::value<double>(), "Staple concentration (mol/L)")
        ("cation_M", po::value<double>(), "Cation concentration (mol/L)")
        ("temp_for_staple_u", po::value<double>(), "Temperature to calculate chemical potential with")
        ("staple_u_mult", po::value<double>(), "Multiplier for staple u")
        ("lattice_site_volume", po::value<double>(), "Volume per lattice site (L)")
        ("cyclic", po::value<bool>(), "Cyclic scaffold")
        ("energy_filebase", po::value<string>(), "Filebase for read/write of energies")
        ("restart_traj_file", po::value<string>(), "Trajectory file to restart from")
        //("restart_traj_files", po::value<string>(), "Trajectory file to restart from")
        ("restart_step", po::value<int>(), "Step to restart from")

        // Order parameters
        ("distance_pairs", po::value<string>(), "Scaffold domains to restrain")
        ("distance_sum", po::value<bool>(), "Sum of distance pairs")

        // Bias functions
        ("distance_bias", po::value<bool>(), "Include domain pair distance bias")
        ("min_dist", po::value<int>(), "Distance at which bias switched off")
        ("max_dist", po::value<int>(), "Distance at which bias switched on")
        ("max_bias", po::value<double>(), "Value of bias at dist >= max_dist")
        ("bias_mult", po::value<double>(), "Total bias multiplier")
        ("grid_bias", po::value<bool>(), "Include a grid bias")

        // General simulation parameters
        ("simulation_type", po::value<string>(), "constant_temp, annealing, or parallel_tempering")
        ("steps", po::value<long int>(), "Number of MC steps")
        ("logging_freq", po::value<int>(), "Logging frequency")
        ("centering_freq", po::value<int>(), "Centering frequency")
        ("orientation_rotation", po::value<string>(), "Orientational rotation movetype probability")
        ("met_staple_exchange", po::value<string>(), "Met staple exchange movetype probability")
        ("met_staple_regrowth", po::value<string>(), "Met staple regrowth movetype probability")
        ("cb_staple_exchange", po::value<string>(), "CB staple exchange movetype probability")
        ("cb_staple_regrowth", po::value<string>(), "CB staple regrowth movetype probability")
        ("ctcb_scaffold_regrowth", po::value<string>(), "CTCB scaffold regrowth movetype probability")
        ("num_walks_filename", po::value<string>(), "Precalculated number of ideal random walks archive")
        ("exchange_mult", po::value<double>(), "Exchange acceptance probability multiplier")

        // Annealing simulation parameters
        ("max_temp", po::value<double>(), "Maximum temperature for annealing")
        ("min_temp", po::value<double>(), "Minimum temperature for annealing")
        ("temp_interval", po::value<double>(), "Temperature interval for annealing")
        ("steps_per_temp", po::value<int>(), "Steps per temperature in annealing")

        // Parallel tempering options 
        ("temps", po::value<string>(), "Temperature list")
        ("num_reps", po::value<int>(), "Number of replicas")
        ("exchange_interval", po::value<int>(), "Steps between exchange attempts")
        ("constant_staple_M", po::value<bool>(), "Hold staple concentration constant")
        ("chem_pot_mults", po::value<string>(), "Factor to multiply base chem pot for each rep")
        ("bias_mults", po::value<string>(), "Multiplier for system bias")

        // Umbrella sampling options
        ("order_params", po::value<string>(), "List of order parameters to apply to")
        ("num_iters", po::value<int>(), "Number of iterations")

        // Output options
        ("output_filebase", po::value<string>(), "Base name for output files")
        ("configs_output_freq", po::value<int>(), "Configuration output write frequency")
        ("counts_output_freq", po::value<int>(), "Coounts output write frequency")
        ;

    po::variables_map vm;
    po::store(po::parse_config_file(parameter_file, desc), vm);
    po::notify(vm);

    // Extract variables from variables map

    // System input and parameters
    if (vm.count("origami_input_filename")) {
        m_origami_input_filename = vm["origami_input_filename"].as<string>();
    }
    if (vm.count("temp")) {
        m_temp = vm["temp"].as<double>();
    }
    if (vm.count("staple_M")) {
        m_staple_M = vm["staple_M"].as<double>();
    }
    if (vm.count("cation_M")) {
        m_cation_M = vm["cation_M"].as<double>();
    }
    if (vm.count("temp_for_staple_u")) {
        m_temp_for_staple_u = vm["temp_for_staple_u"].as<double>();
    }
    if (vm.count("staple_u_mult")) {
        m_staple_u_mult = vm["staple_u_mult"].as<double>();
    }
    if (vm.count("lattice_site_volume")) {
        m_lattice_site_volume = vm["lattice_site_volume"].as<double>();
    }
    if (vm.count("cyclic")) {
        m_cyclic = vm["cyclic"].as<bool>();
    }
    if (vm.count("energy_filebase")) {
        m_energy_filebase = vm["energy_filebase"].as<string>();
    }
    if (vm.count("restart_traj_file")) {
        m_restart_traj_file = vm["restart_traj_file"].as<string>();
    }
    if (vm.count("restart_step")) {
        m_restart_step = vm["restart_step"].as<int>();
    }

    // Order parameters
    if (vm.count("distance_pairs")) {
        string distance_pairs_s= vm["distance_pairs"].as<string>();
        m_distance_pairs = string_to_int_vector(distance_pairs_s);
    }
    if (vm.count("distance_sum")) {
        m_distance_sum = vm["distance_sum"].as<bool>();
    }

    // Bias functions
    if (vm.count("distance_bias")) {
        m_distance_bias = vm["distance_bias"].as<bool>();
        m_biases_present = true;
    }
    if (vm.count("bias_mult")) {
        m_bias_mult = vm["bias_mult"].as<double>();
    }
    if (vm.count("min_dist")) {
        m_min_dist = vm["min_dist"].as<int>();
    }
    if (vm.count("max_dist")) {
        m_max_dist = vm["max_dist"].as<int>();
    }
    if (vm.count("max_bias")) {
        m_max_bias = vm["max_bias"].as<double>();
        m_biases_present = true;
    }
    if (vm.count("grid_bias")) {
        m_grid_bias = vm["grid_bias"].as<bool>();
    }

    // General simulation parameters
    if (vm.count("simulation_type")) {
        m_simulation_type = vm["simulation_type"].as<string>();
        set_default_sim_options();
    }
    if (vm.count("steps")) {
        m_steps = vm["steps"].as<long int>();
    }
    if (vm.count("logging_freq")) {
        m_logging_freq = vm["logging_freq"].as<int>();
    }
    if (vm.count("centering_freq")) {
        m_centering_freq = vm["centering_freq"].as<int>();
    }
    if (vm.count("orientation_rotation")) {
        string unparsed_fraction {vm["orientation_rotation"].as<string>()};
        Fraction prob {unparsed_fraction};
        m_movetype_probs.push_back(prob.to_double());
        m_movetypes.push_back(1);
    }
    if (vm.count("met_staple_exchange")) {
        string unparsed_fraction {vm["met_staple_exchange"].as<string>()};
        Fraction prob {unparsed_fraction};
        m_movetype_probs.push_back(prob.to_double());
        m_movetypes.push_back(2);
    }
    if (vm.count("met_staple_regrowth")) {
        string unparsed_fraction {vm["met_staple_regrowth"].as<string>()};
        Fraction prob {unparsed_fraction};
        m_movetype_probs.push_back(prob.to_double());
        m_movetypes.push_back(3);
    }
    if (vm.count("cb_staple_exchange")) {
        string unparsed_fraction {vm["cb_staple_exchange"].as<string>()};
        Fraction prob {unparsed_fraction};
        m_movetype_probs.push_back(prob.to_double());
        m_movetypes.push_back(4);
    }
    if (vm.count("cb_staple_regrowth")) {
        string unparsed_fraction {vm["cb_staple_regrowth"].as<string>()};
        Fraction prob {unparsed_fraction};
        m_movetype_probs.push_back(prob.to_double());
        m_movetypes.push_back(5);
    }
    if (vm.count("ctcb_scaffold_regrowth")) {
        string unparsed_fraction {vm["ctcb_scaffold_regrowth"].as<string>()};
        Fraction prob {unparsed_fraction};
        m_movetype_probs.push_back(prob.to_double());
        m_movetypes.push_back(6);
    }
    if (vm.count("num_walks_filename")) {
        m_num_walks_filename = vm["num_walks_filename"].as<string>();
    }

    if (vm.count("exchange_mult")) {
        m_exchange_mult = vm["exchange_mult"].as<double>();
    }

    // Annealing simulation parameters
    if (vm.count("max_temp")) {
        m_max_temp = vm["max_temp"].as<double>();
    }
    if (vm.count("min_temp")) {
        m_min_temp = vm["min_temp"].as<double>();
    }
    if (vm.count("temp_interval")) {
        m_temp_interval = vm["temp_interval"].as<double>();
    }
    if (vm.count("steps_per_temp")) {
        m_steps_per_temp = vm["steps_per_temp"].as<int>();
    }

    // Parallel tempering options 
    if (vm.count("temps")) {
        string temps_s = vm["temps"].as<string>();
        m_temps = string_to_double_vector(temps_s);
    }
    if (vm.count("num_reps")) {
        m_num_reps = vm["num_reps"].as<int>();
    }
    if (vm.count("exchange_interval")) {
        m_exchange_interval = vm["exchange_interval"].as<int>();
    }
    if (vm.count("constant_staple_M")) {
        m_constant_staple_M = vm["constant_staple_M"].as<bool>();
    }
    if (vm.count("chem_pot_mults")) {
        string chem_pot_mults_s = vm["chem_pot_mults"].as<string>();
        m_chem_pot_mults = string_to_double_vector(chem_pot_mults_s);
    }
    if (vm.count("bias_mults")) {
        string bias_mults_s {vm["bias_mults"].as<string>()};
        m_bias_mults = string_to_double_vector(bias_mults_s);
    }

    // Umbrella sampling options
    if (vm.count("order_params")) {
        string order_params_s {vm["order_params"].as<string>()};
        m_order_params = split(order_params_s, ' ');
    }
    if (vm.count("num_iters")) {
        m_num_iters = vm["num_iters"].as<int>();
    }

    // Output options
    if (vm.count("output_filebase")) {
        m_output_filebase = vm["output_filebase"].as<string>();
    }
    if (vm.count("configs_output_freq")) {
        m_configs_output_freq = vm["configs_output_freq"].as<int>();
    }
    if (vm.count("counts_output_freq")) {
        m_counts_output_freq = vm["counts_output_freq"].as<int>();
    }
}

void InputParameters::set_default_sim_options() {
    if (m_simulation_type == "umbrella_sampling") {
        m_biases_present = true;
        m_grid_bias = true;
    }
}
