#!/usr/bin/env ruby
# frozen_string_literal: true

# OEIS A398331 -- direct implementation of the definition
#
# a(n) is one-half of the number of permutations p of [n] having exactly
# k runs of adjacent entries differing by 1.  For A398331, k = 5 and
# n >= 2*k = 10.
#
# A run of a permutation p is a maximal contiguous block
# p(i), p(i+1), ..., p(j), where j > i, such that
# |p(h+1) - p(h)| = 1 for every i <= h < j.
#
# This program computes directly from the definition:
#
#   1. append an unused entry to a permutation prefix;
#   2. start a run exactly when the new adjacent difference becomes 1;
#   3. count completions having exactly k runs and divide by 2.
#
# Prefixes with the same used entries, last entry, run count, and run state
# have the same possible completions, so their completion count is memoized.
# A prefix is also discarded when its remaining positions cannot produce k
# runs.  No generating function, inclusion-exclusion, or reversal symmetry
# is used.
#
# Terms are printed as a comma-separated sequence.
#
# Usage:
#   ruby 398330_02.rb                 # k=4, n=8
#   ruby 398330_02.rb 10              # k=5, n=10
#   ruby 398330_02.rb --runs 4 9      # k=4, n=8..9
#   ruby 398330_02.rb --check

module A398330Direct
  DEFAULT_RUNS = 4
  DEFAULT_MAX_N = 8
  MAX_SUPPORTED_N = 18

  # KNOWN_TERMS = {
  #   [4, 8] => 131,
  #   [4, 9] => 3_177,
  #   [5, 10] => 1_281
  # }.freeze

  class InputError < StandardError; end
  class CalculationError < StandardError; end

  module_function

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

  def completion_count(n, k, mask, last, runs, inside_run, depth, all_mask, memo)
    return runs == k ? 1 : 0 if mask == all_mask
    return 0 if runs > k

    remaining_positions = n - depth
    maximum_additional_runs =
      inside_run ? remaining_positions / 2 : (remaining_positions + 1) / 2
    return 0 if runs + maximum_additional_runs < k

    key = (((mask * n + last - 1) * (k + 1) + runs) << 1) |
          (inside_run ? 1 : 0)
    cached = memo[key]
    return cached unless cached.nil?

    total = 0
    available = all_mask ^ mask
    until available.zero?
      bit = available & -available
      candidate = bit.bit_length
      consecutive = (last - candidate).abs == 1
      next_runs = runs + (consecutive && !inside_run ? 1 : 0)

      total += completion_count(
        n, k, mask | bit, candidate, next_runs, consecutive,
        depth + 1, all_mask, memo
      )
      available ^= bit
    end

    memo[key] = total
  end

  def value(n, k)
    all_mask = (1 << n) - 1
    memo = {}
    count = 0

    1.upto(n) do |first|
      count += completion_count(
        n, k, 1 << (first - 1), first, 0, false, 1, all_mask, memo
      )
    end

    unless count.even?
      raise CalculationError, "the permutation count is odd for n=#{n}, k=#{k}: #{count}"
    end

    count / 2
  end

  # Independent factorial-time implementation, used only for small checks.
  def brute_force_value(n, k)
    count = 0
    (1..n).to_a.permutation do |permutation|
      count += 1 if run_count(permutation) == k
    end
    count / 2
  end

  def each_value(k, maximum_n)
    validate_range(k, maximum_n)
    return enum_for(__method__, k, maximum_n) unless block_given?

    (2 * k).upto(maximum_n) do |n|
      yield n, value(n, k)
    end
  end

  def validate_range(k, maximum_n)
    unless k.is_a?(Integer) && k.positive?
      raise InputError, "K must be a positive integer: #{k.inspect}"
    end
    unless maximum_n.is_a?(Integer) && maximum_n.between?(2 * k, MAX_SUPPORTED_N)
      raise InputError,
            "N must be in #{2 * k}..#{MAX_SUPPORTED_N}: #{maximum_n.inspect}"
    end
  end

  def parse_integer(text, label)
    Integer(text, 10)
  rescue ArgumentError
    raise InputError, "#{label} must be an integer: #{text}"
  end

  def check
    KNOWN_TERMS.each do |(k, n), expected|
      actual = value(n, k)
      next if actual == expected

      raise CalculationError,
            "check failed for n=#{n}, k=#{k}: expected #{expected}, got #{actual}"
    end

    1.upto(4) do |k|
      n = 2 * k
      expected = brute_force_value(n, k)
      actual = value(n, k)
      next if actual == expected

      raise CalculationError,
            "brute-force check failed for n=#{n}, k=#{k}: " \
            "expected #{expected}, got #{actual}"
    end

    warn 'ok: known terms and independent brute-force checks agree'
  end

  def usage(program)
    <<~USAGE
      usage: #{program} [MAX_N]
             #{program} --runs K MAX_N
             #{program} --check

      The default is K=#{DEFAULT_RUNS}, MAX_N=#{DEFAULT_MAX_N}.
      Terms a(n) are printed for n=2*K..MAX_N, separated by commas.
      The direct state search is exponential, so N is limited to #{MAX_SUPPORTED_N}.
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

    k, maximum_n =
      case arguments.length
      when 0
        [DEFAULT_RUNS, DEFAULT_MAX_N]
      when 1
        [DEFAULT_RUNS, parse_integer(arguments[0], 'N')]
      when 3
        raise InputError, usage(program) unless arguments[0] == '--runs'

        [parse_integer(arguments[1], 'K'), parse_integer(arguments[2], 'N')]
      else
        raise InputError, usage(program)
      end

    terms = each_value(k, maximum_n).map { |_n, value| value }
    puts terms.join(', ')
  end
end

if __FILE__ == $PROGRAM_NAME
  begin
    A398330Direct.run(ARGV, $PROGRAM_NAME)
  rescue A398330Direct::InputError, A398330Direct::CalculationError => error
    warn "error: #{error.message}"
    exit 1
  end
end
