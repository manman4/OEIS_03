#!/usr/bin/env ruby
# frozen_string_literal: true

# OEIS A398328 -- direct implementation of the definition
#
# Irregular triangle read by rows: T(n,k) is the number of permutations of
# [n] having exactly k runs of consecutive integers in either direction,
# n >= 0 and 0 <= k <= floor(n/2).
#
# A run is a maximal contiguous block of length at least 2 whose successive
# entries differ by 1 in absolute value.  For example, 145623 has two runs:
# 456 and 23.
#
# This program follows the definition literally:
#
#   1. enumerate every permutation of [n];
#   2. examine every pair of adjacent entries;
#   3. start a run whenever their absolute difference becomes 1;
#   4. count the permutation in the column indexed by its number of runs.
#
# It uses no generating function, recurrence, inclusion-exclusion, reversal
# symmetry, or other pruning.  Its running time is therefore factorial in n.
#
# Usage:
#   ruby 398328_02.rb             # flattened rows n=0..8
#   ruby 398328_02.rb 9           # flattened rows n=0..9
#   ruby 398328_02.rb --rows 8    # one labeled row per line
#   ruby 398328_02.rb --term 8 2  # T(8,2)
#   ruby 398328_02.rb --check

module A398328Direct
  DEFAULT_MAX_N = 8
  MAX_SUPPORTED_N = 10

  KNOWN_ROWS = [
    [1],
    [1],
    [0, 2],
    [0, 6],
    [2, 16, 6],
    [14, 56, 50],
    [90, 286, 310, 34],
    [646, 1866, 2010, 518],
    [5242, 14_300, 14_976, 5540, 262]
  ].freeze

  class InputError < StandardError; end
  class CalculationError < StandardError; end

  module_function

  def parse_integer(text, label, minimum, maximum)
    value = Integer(text, 10)
    unless value.between?(minimum, maximum)
      raise InputError, "#{label} must be in #{minimum}..#{maximum}: #{text}"
    end

    value
  rescue ArgumentError
    raise InputError, "#{label} must be an integer: #{text}"
  end

  def validate_n(n)
    return if n.is_a?(Integer) && n.between?(0, MAX_SUPPORTED_N)

    raise InputError, "N must be in 0..#{MAX_SUPPORTED_N}: #{n.inspect}"
  end

  def run_count(permutation)
    runs = 0
    inside_run = false

    permutation.each_cons(2) do |left, right|
      consecutive = (left - right).abs == 1
      runs += 1 if consecutive && !inside_run
      inside_run = consecutive
    end

    runs
  end

  def row(n)
    validate_n(n)
    counts = Array.new(n / 2 + 1, 0)

    (1..n).to_a.permutation do |permutation|
      counts[run_count(permutation)] += 1
    end

    counts
  end

  def value(n, k)
    validate_n(n)
    unless k.is_a?(Integer) && k.between?(0, n / 2)
      raise InputError, "K must be in 0..floor(N/2): #{k.inspect}"
    end

    row(n)[k]
  end

  def rows(maximum_n)
    validate_n(maximum_n)
    0.upto(maximum_n).map { |n| row(n) }
  end

  def check
    actual = rows(KNOWN_ROWS.length - 1)
    unless actual == KNOWN_ROWS
      raise CalculationError,
            "check failed:\nexpected #{KNOWN_ROWS.inspect}\n" \
            "     got #{actual.inspect}"
    end

    warn 'ok: direct permutation enumeration agrees with A398328 for n=0..8'
  end

  def usage(program)
    <<~USAGE
      usage: #{program} [MAX_N]
             #{program} --upto MAX_N
             #{program} --rows MAX_N
             #{program} --term N K
             #{program} --check

      With no arguments, print the flattened rows n=0..#{DEFAULT_MAX_N}.
      N and MAX_N may be 0..#{MAX_SUPPORTED_N}.
      The calculation enumerates all n! permutations.
    USAGE
  end

  def run(arguments, program)
    if arguments == ['--help'] || arguments == ['-h']
      puts usage(program)
      return
    end

    if arguments == ['--check']
      check
      return
    end

    if arguments.first == '--term'
      raise InputError, usage(program) unless arguments.length == 3

      n = parse_integer(arguments[1], 'N', 0, MAX_SUPPORTED_N)
      k = parse_integer(arguments[2], 'K', 0, n / 2)
      puts value(n, k)
      return
    end

    mode, maximum_n =
      case arguments.length
      when 0
        [:flat, DEFAULT_MAX_N]
      when 1
        [:flat, parse_integer(arguments[0], 'MAX_N', 0, MAX_SUPPORTED_N)]
      when 2
        unless %w[--upto --rows].include?(arguments[0])
          raise InputError, usage(program)
        end

        selected_mode = arguments[0] == '--rows' ? :rows : :flat
        selected_n = parse_integer(arguments[1], 'MAX_N', 0, MAX_SUPPORTED_N)
        [selected_mode, selected_n]
      else
        raise InputError, usage(program)
      end

    triangle = rows(maximum_n)
    if mode == :rows
      triangle.each_with_index do |values, n|
        puts "n=#{n}: #{values.join(', ')}"
      end
    else
      puts triangle.flatten.join(', ')
    end
  end
end

if __FILE__ == $PROGRAM_NAME
  begin
    A398328Direct.run(ARGV, $PROGRAM_NAME)
  rescue A398328Direct::InputError, A398328Direct::CalculationError => error
    warn "error: #{error.message}"
    exit 1
  end
end
