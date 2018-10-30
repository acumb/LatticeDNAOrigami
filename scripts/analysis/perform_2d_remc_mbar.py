#!/usr/bin/env python

"""Carry out standard MBAR analysis on 2D REMC simulation output.

The exchange variables are assumed to be temperature and stacking multiplier,
in that order.
"""

import argparse

from origamipy import conditions
from origamipy import biases
from origamipy import outputs


def main():
    args = parse_args()
    conditions_map = construct_conditions_map(args)
    fileformatter = construct_fileformatter()
    all_conditions = conditions.AllSimConditions(conditions_map, fileformatter)
    inp_filebase = create_input_filepathbase(args)
    sims_collection = outputs.MultiStateSimCollection(inp_filebase, all_conditions)

    sims_collection.perform_decorrelation()
    sims_collection.perform_mbar()

    out_filebase = create_output_filepathbase(args)
    sims_collection.calculate_all_expectations(out_filebase)


def construct_conditions_map(args):
    stack_biases = []
    for stack_mult in args.stack_mults:
        stack_bias = biases.StackingBias(args.stack_ene, stack_mult)
        stack_biases.append(stack_bias)

    conditions_map = {'temp': args.temps,
                     'staple_m': [args.staple_m],
                     'bias': stack_biases}

    return conditions_map


def construct_fileformatter():
    spec = conditions.ConditionsFileformatSpec(('temp', '{:d}'), ('bias', '{:.1f}'))
    return conditions.ConditionsFileformatter(spec)


def create_input_filepathbase(args):
    return '{}/{}'.format(args.input_dir, args.filebase)


def create_output_filepathbase(args):
    return '{}/{}'.format(args.output_dir, args.filebase)


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument(
            'filebase',
            type=str,
            help='Base name for files')
    parser.add_argument(
            'input_dir',
            type=str,
            help='Directory of inputs')
    parser.add_argument(
            'output_dir',
            type=str,
            help='Directory to output to')
    parser.add_argument(
            'staple_m',
            type=float,
            help='Staple molarity (mol/V)')
    parser.add_argument(
            'stack_ene',
            type=float,
            help='Stacking energy (kb K)')
    parser.add_argument(
            '--temps',
            nargs='+',
            type=int,
            help='Temperatures')
    parser.add_argument(
            '--stack_mults',
            nargs='+',
            type=float,
            help='Stacking energy multipliers')
    parser.add_argument('--tags',
            nargs='+',
            type=str,
            help='Order parameter tags')
    parser.add_argument('--tag_pairs',
            nargs='+',
            type=str,
            help='Tags to calculate 2D pmf for (comma delim)')

    return parser.parse_args()


if __name__ == '__main__':
    main()