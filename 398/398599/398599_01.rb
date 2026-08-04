#!/usr/bin/env ruby
# frozen_string_literal: true

# Exact computation of b_{1,s}(n):
#
#   #{p in S_n : |p(i+1)-p(i)| != s for every 1 <= i < n}.
#
# This evaluates the Spahn--Zeilberger tiling-partition formula
#
#   b_{r,s}(n) = sum_alpha C_alpha(n,r) C_alpha(n,s)
#                  * (-1)^(number_of_tiles-n)
#                  * product_j a_j! * 2^(a_2+a_3+...).
#
# Here r=1, while s is selected on the command line.  Ruby Integer gives
# exact arbitrary-precision arithmetic.  No b-file is created; --upto
# prints a(0)..a(MAX_N) as one comma-separated line.
#
# Reference:
#   G. Spahn and D. Zeilberger,
#   "Counting Permutations Where The Difference Between Entries Located
#    r Places Apart Can never be s",
#   https://doi.org/10.54550/ECA2023V3S2R10
#
# Usage:
#   ./398599_01.rb
#   ./398599_01.rb --upto 30
#   ./398599_01.rb --s 2 --upto 20
#   ./398599_01.rb --term 20
#   ./398599_01.rb --check

module B1S
  DEFAULT_S = 3
  DEFAULT_MAX_N = 20
  MAX_SUPPORTED_N = 30
  MAX_SUPPORTED_S = 128
  BRUTE_CHECK_MAX_N = 11
  GENERAL_CHECK_MAX_S = 8

  # For n<=30 there are at most fifteen non-singleton tiles, so five bits
  # per histogram count are sufficient.  Packed histograms can be combined
  # by ordinary Integer addition without inter-field carries.
  HISTOGRAM_BITS = 5
  HISTOGRAM_MASK = (1 << HISTOGRAM_BITS) - 1

  class CalculationError < StandardError; end

  # OEIS A002464 = b_{1,1}(n), n=0..20.
  KNOWN_B11 = %w[
    1 1 0 0 2 14 90 646 5242 47622 479306 5296790 63779034
    831283558 11661506218 175203184374 2806878055610
    47767457130566 860568917787402 16362838542699862
    327460573946510746
  ].map!(&:to_i).freeze

  module_function

  def parse_integer(text, label, minimum, maximum)
    value = Integer(text, 10)
    unless value.between?(minimum, maximum)
      raise CalculationError,
            "#{label} must be in #{minimum}..#{maximum}: #{text}"
    end
    value
  rescue ArgumentError
    raise CalculationError,
          "#{label} must be in #{minimum}..#{maximum}: #{text}"
  end

  def parse_n(text, label)
    parse_integer(text, label, 0, MAX_SUPPORTED_N)
  end

  def parse_s(text)
    parse_integer(text, 'S', 1, MAX_SUPPORTED_S)
  end

  def binomial(n, k)
    return 0 if k.negative? || k > n

    k = [k, n - k].min
    value = 1
    1.upto(k) { |i| value = value * (n - k + i) / i }
    value
  end

  # Enumerate all possible non-singleton tile histograms for one path.
  # edge_count is sum_l l*h_l and component_count is sum_l h_l.
  def generate_local_recursive(path_size, length_limit, length_index,
                               edge_count, component_count,
                               component_orders, packed_histogram, output)
    if length_index == length_limit
      gap_choices = binomial(path_size - edge_count, component_count)
      output << [packed_histogram, gap_choices * component_orders]
      return
    end

    length = length_index + 1
    remaining_vertices = path_size - edge_count - component_count
    maximum = remaining_vertices / (length + 1)
    0.upto(maximum) do |count|
      next_histogram =
        packed_histogram | (count << (HISTOGRAM_BITS * length_index))
      next_orders = component_orders *
                    binomial(component_count + count, count)
      generate_local_recursive(
        path_size, length_limit, length_index + 1,
        edge_count + length * count, component_count + count,
        next_orders, next_histogram, output
      )
    end
  end

  def generate_local_patterns(path_size)
    raise CalculationError, 'invalid residue path size' if path_size < 1

    output = []
    generate_local_recursive(
      path_size, path_size - 1, 0, 0, 0, 1, 0, output
    )
    output.freeze
  end

  # Return H -> C_H(n,step), together with convolution statistics.
  def tiling_coefficients(n, step)
    path_sizes = 1.upto(step).map { |k| (n - k) / step + 1 }
    current = { 0 => 1 }
    cache = {}
    convolution_pairs = 0

    path_sizes.each do |path_size|
      patterns = cache[path_size] ||= generate_local_patterns(path_size)
      following = Hash.new(0)
      current.each do |histogram, coefficient|
        patterns.each do |local_histogram, local_coefficient|
          following[histogram + local_histogram] +=
            coefficient * local_coefficient
          convolution_pairs += 1
        end
      end
      current = following
    end

    coefficient_sum = current.values.sum
    expected_sum = 1 << (n - step)
    unless coefficient_sum == expected_sum
      raise CalculationError,
            "step-#{step} tiling coefficients do not sum to 2^(n-step)"
    end
    [current, convolution_pairs]
  end

  def factorial_table(n)
    factorial = Array.new(n + 1, 1)
    1.upto(n) { |i| factorial[i] = factorial[i - 1] * i }
    factorial
  end

  def compute_formula(n, s)
    unless n.between?(0, MAX_SUPPORTED_N) &&
           s.between?(1, MAX_SUPPORTED_S)
      raise CalculationError, 'unsupported n or s'
    end

    started = Process.clock_gettime(Process::CLOCK_MONOTONIC)
    factorial = factorial_table(n)
    if n <= s
      seconds = Process.clock_gettime(Process::CLOCK_MONOTONIC) - started
      return [
        factorial[n],
        {
          input_histograms: 1,
          output_histograms: 1,
          convolution_pairs: 0,
          seconds: seconds
        }
      ]
    end

    input, input_pairs = tiling_coefficients(n, 1)
    output, output_pairs = tiling_coefficients(n, s)
    result = 0

    # The step-1 path may contain tiles of every size up to n.
    input.each do |packed_histogram, input_coefficient|
      output_coefficient = output[packed_histogram]
      next if output_coefficient.nil?

      histogram = packed_histogram
      edge_count = 0
      component_count = 0
      histogram_factorial = 1
      0.upto(n - 2) do |length_index|
        count = histogram & HISTOGRAM_MASK
        unless count.zero?
          edge_count += (length_index + 1) * count
          component_count += count
          histogram_factorial *= factorial[count] if count > 1
        end
        histogram >>= HISTOGRAM_BITS
      end

      singleton_count = n - edge_count - component_count
      if singleton_count.negative?
        raise CalculationError, 'invalid common tile histogram'
      end

      term = input_coefficient * output_coefficient
      term *= factorial[singleton_count] * histogram_factorial
      term <<= component_count # two orientations per non-singleton tile
      result += edge_count.even? ? term : -term
    end

    unless result.between?(0, factorial[n])
      raise CalculationError, 'formula result is outside 0..n!'
    end

    seconds = Process.clock_gettime(Process::CLOCK_MONOTONIC) - started
    [
      result,
      {
        input_histograms: input.length,
        output_histograms: output.length,
        convolution_pairs: input_pairs + output_pairs,
        seconds: seconds
      }
    ]
  end

  def verify_known_value(value, n, s)
    return unless s == 1 && n < KNOWN_B11.length
    return if value == KNOWN_B11[n]

    raise CalculationError,
          "A002464 mismatch at n=#{n}: " \
          "got #{value}, expected #{KNOWN_B11[n]}"
  end

  def compute_value(n, s, verbose:)
    value, stats = compute_formula(n, s)
    verify_known_value(value, n, s)
    if verbose
      warn format(
        '%s: r=1, s=%d, n=%d, input histograms=%d, ' \
        'output histograms=%d, convolution pairs=%d, %.3f s',
        File.basename($PROGRAM_NAME), s, n,
        stats[:input_histograms], stats[:output_histograms],
        stats[:convolution_pairs], stats[:seconds]
      )
    end
    value
  end

  # Independent subset DP: build the permutation from left to right and
  # remember only the used values and the last value.
  def brute_recursive(n, s, full_mask, memo, used, last_value)
    return 1 if used == full_mask

    index = used * (n + 1) + last_value
    cached = memo[index]
    return cached unless cached.nil?

    total = 0
    available = full_mask ^ used
    until available.zero?
      bit = available & -available
      available ^= bit
      value = bit.bit_length - 1
      next if last_value < n && (value - last_value).abs == s

      total += brute_recursive(n, s, full_mask, memo,
                               used | bit, value)
    end
    memo[index] = total
  end

  def brute_count(n, s)
    unless n.between?(0, BRUTE_CHECK_MAX_N)
      raise CalculationError,
            'independent check n is outside its supported range'
    end

    full_mask = (1 << n) - 1
    memo = Array.new((1 << n) * (n + 1))
    brute_recursive(n, s, full_mask, memo, 0, n)
  end

  def check_implementation(maximum_n)
    0.upto(maximum_n) do |n|
      value, = compute_formula(n, 1)
      verify_known_value(value, n, 1)
    end

    brute_maximum = [maximum_n, BRUTE_CHECK_MAX_N].min
    1.upto(GENERAL_CHECK_MAX_S) do |s|
      0.upto(brute_maximum) do |n|
        value, = compute_formula(n, s)
        independent = brute_count(n, s)
        next if value == independent

        raise CalculationError,
              "independent DP mismatch at s=#{s}, n=#{n}: " \
              "formula=#{value}, DP=#{independent}"
      end
    end

    known_maximum = [maximum_n, KNOWN_B11.length - 1].min
    puts "ok: A002464=b_{1,1}(n) verified through n=#{known_maximum}; " \
         "formula evaluated through n=#{maximum_n}; " \
         "independent DP for s=1..#{GENERAL_CHECK_MAX_S} " \
         "through n=#{brute_maximum}"
    0
  end

  def print_sequence(maximum_n, s)
    values = 0.upto(maximum_n).map do |n|
      compute_value(n, s, verbose: false)
    end
    puts values.join(', ')
    0
  end

  def usage(program)
    warn <<~USAGE
      usage: #{program} [MAX_N]
             #{program} --term N
             #{program} --upto MAX_N
             #{program} --s S [MAX_N]
             #{program} --s S --term N
             #{program} --s S --upto MAX_N
             #{program} --check [MAX_N]

      S defaults to #{DEFAULT_S} and may be 1..#{MAX_SUPPORTED_S}.
      MAX_N defaults to #{DEFAULT_MAX_N} and may be 0..#{MAX_SUPPORTED_N}.
      --upto prints b_{1,S}(0)..b_{1,S}(MAX_N), comma-separated.
      No b-file is created.
    USAGE
  end

  def main(arguments)
    program = File.basename($PROGRAM_NAME)
    if arguments.length == 1 && %w[--help -h].include?(arguments[0])
      usage(program)
      return 0
    end
    if arguments.first == '--check'
      if arguments.length > 2
        usage(program)
        return 1
      end
      maximum_n = arguments.length == 2 ?
        parse_n(arguments[1], 'MAX_N') : DEFAULT_MAX_N
      return check_implementation(maximum_n)
    end

    mode = :upto
    s = DEFAULT_S
    n = nil
    have_s = false
    argument = 0
    while argument < arguments.length
      text = arguments[argument]
      case text
      when '--s'
        if have_s || argument + 1 >= arguments.length
          usage(program)
          return 1
        end
        argument += 1
        s = parse_s(arguments[argument])
        have_s = true
      when '--term', '--upto'
        if !n.nil? || argument + 1 >= arguments.length
          usage(program)
          return 1
        end
        mode = text == '--term' ? :term : :upto
        argument += 1
        n = parse_n(arguments[argument], mode == :term ? 'N' : 'MAX_N')
      else
        if text.start_with?('-') || !n.nil?
          usage(program)
          return 1
        end
        n = parse_n(text, 'MAX_N')
        mode = :upto
      end
      argument += 1
    end

    n ||= DEFAULT_MAX_N
    if mode == :term
      value = compute_value(n, s, verbose: true)
      puts "#{n} #{value}"
      0
    else
      print_sequence(n, s)
    end
  end
end

begin
  exit B1S.main(ARGV)
rescue B1S::CalculationError => error
  warn "error: #{error.message}"
  exit 1
end
