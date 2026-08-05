#!/usr/bin/env ruby
# frozen_string_literal: true

# Square array A(n,k), n >= 0, k >= 0, read by antidiagonals
# downwards, where A(n,k) is the number of permutations p of [n]
# such that
#
#   |p(i+1) - p(i)| != k for every 1 <= i < n.
#
# Algorithm 03: Spahn--Zeilberger tiling-partition formula.
#
# Let C_alpha(n,d) be the number of tilings of {1,...,n} by d-step
# chains having tile-size partition alpha.  For k >= 1,
#
#   A(n,k) = sum_alpha C_alpha(n,1) * C_alpha(n,k)
#                * (-1)^(number_of_tiles-n)
#                * product_j a_j! * 2^(a_2+a_3+...).
#
# Store h_l=a_{l+1}, the number of non-singleton tiles with l edges.
# If r=sum_l l*h_l and c=sum_l h_l, then a_1=n-r-c.  A fixed
# histogram occurs on a path of m vertices
#
#   binomial(m-r,c) * c! / product_l h_l!
#
# times.  Convolution over the k residue paths constructs C_alpha(n,k);
# C_alpha(n,1) is then evaluated directly from the displayed expression.
# This preserves the algorithm of 398599_03.c and does not construct the
# q(H) quantity used by algorithm 02.
#
# Ruby Integer supplies exact arbitrary-precision arithmetic.  The packed
# histogram uses five bits per count.  Since n<=30 here, every h_l<=15, so
# addition of valid local histograms cannot carry between packed fields.
# Direct permutation enumeration is used by --check only.
#
# Reference:
#   G. Spahn and D. Zeilberger,
#   "Counting Permutations Where The Difference Between Entries Located
#    r Places Apart Can never be s",
#   https://doi.org/10.54550/ECA2023V3S2R10
#
# Antidiagonal d is emitted in the order
#
#   A(0,d), A(1,d-1), ..., A(d,0).
#
# Usage:
#   ./398634_03.rb
#   ./398634_03.rb --upto 5
#   ./398634_03.rb 5
#   ./398634_03.rb --term 6 2
#   ./398634_03.rb --check
#
# --upto D prints all antidiagonals d=0..D on one comma-separated line.
# No b-file is created.

module A398634TilingFormula
  DEFAULT_MAX_DIAGONAL = 5
  MAX_DIAGONAL = 30
  CHECK_MAX_DIAGONAL = 8

  HISTOGRAM_BITS = 5
  HISTOGRAM_MASK = (1 << HISTOGRAM_BITS) - 1

  KNOWN_THROUGH_5 = [
    1,
    1, 1,
    1, 1, 2,
    1, 1, 0, 6,
    1, 1, 2, 0, 24,
    1, 1, 2, 2, 2, 120
  ].freeze

  # A002464 = A(n,1), n=0..20.
  KNOWN_K1 = %w[
    1 1 0 0 2 14 90 646 5242 47622 479306 5296790 63779034
    831283558 11661506218 175203184374 2806878055610
    47767457130566 860568917787402 16362838542699862
    327460573946510746
  ].map!(&:to_i).freeze

  class InputError < StandardError; end
  class CalculationError < StandardError; end

  @factorials = [1]
  @local_pattern_cache = {}

  module_function

  def parse_nonnegative_integer(text, label, maximum = nil)
    value = Integer(text, 10)
    raise InputError, "#{label} must be nonnegative: #{text}" if value.negative?
    if maximum && value > maximum
      raise InputError, "#{label} must be at most #{maximum}: #{text}"
    end
    value
  rescue ArgumentError
    raise InputError, "#{label} must be a nonnegative integer: #{text}"
  end

  def factorial(n)
    while @factorials.length <= n
      @factorials << @factorials[-1] * @factorials.length
    end
    @factorials[n]
  end

  def binomial(n, k)
    return 0 if k.negative? || k > n

    k = [k, n - k].min
    value = 1
    1.upto(k) { |i| value = value * (n - k + i) / i }
    value
  end

  # binomial(available,c)*c!/product_l h_l!
  def tiling_multiplicity(available, component_count, histogram_factor)
    numerator = binomial(available, component_count) * factorial(component_count)
    quotient, remainder = numerator.divmod(histogram_factor)
    unless remainder.zero?
      raise CalculationError, 'nonintegral tiling multiplicity'
    end
    quotient
  end

  def generate_local_recursive(path_size, length, edge_count,
                               component_count, component_orders,
                               packed_histogram, output)
    if length >= path_size
      gap_choices = binomial(path_size - edge_count, component_count)
      output << [packed_histogram, gap_choices * component_orders].freeze
      return
    end

    remaining_vertices = path_size - edge_count - component_count
    maximum_count = remaining_vertices / (length + 1)
    0.upto(maximum_count) do |count|
      next_histogram =
        packed_histogram | (count << (HISTOGRAM_BITS * (length - 1)))
      next_orders = component_orders *
                    binomial(component_count + count, count)
      generate_local_recursive(
        path_size,
        length + 1,
        edge_count + length * count,
        component_count + count,
        next_orders,
        next_histogram,
        output
      )
    end
  end

  # Local histogram polynomial for one path of path_size vertices.
  def local_patterns(path_size)
    cached = @local_pattern_cache[path_size]
    return cached if cached

    output = []
    generate_local_recursive(path_size, 1, 0, 0, 1, 0, output)
    @local_pattern_cache[path_size] = output.freeze
  end

  # Return H -> C_H(n,step).
  def tiling_coefficients(n, step)
    current = { 0 => 1 }
    1.upto(step) do |residue|
      path_size = (n - residue) / step + 1
      # A one-vertex path contributes only the multiplicative identity.
      next if path_size == 1

      following = Hash.new(0)
      patterns = local_patterns(path_size)
      current.each do |histogram, coefficient|
        patterns.each do |local_histogram, local_coefficient|
          following[histogram + local_histogram] +=
            coefficient * local_coefficient
        end
      end
      current = following
    end

    expected_sum = 1 << (n - step)
    unless current.values.sum == expected_sum
      raise CalculationError,
            "step-#{step} tiling coefficients do not sum to 2^(n-step)"
    end
    current
  end

  def histogram_statistics(packed_histogram, n)
    histogram = packed_histogram
    edge_count = 0
    component_count = 0
    histogram_factor = 1

    1.upto(n - 1) do |length|
      count = histogram & HISTOGRAM_MASK
      unless count.zero?
        edge_count += length * count
        component_count += count
        histogram_factor *= factorial(count)
      end
      histogram >>= HISTOGRAM_BITS
    end
    unless histogram.zero?
      raise CalculationError, 'packed histogram exceeds its declared length'
    end

    [edge_count, component_count, histogram_factor]
  end

  def formula_value(n, k)
    # Difference zero cannot occur between distinct permutation entries;
    # if n<=k, no two values in [n] have difference k.
    return factorial(n) if k.zero? || n <= k

    result = 0
    tiling_coefficients(n, k).each do |histogram, output_coefficient|
      edge_count, component_count, histogram_factor =
        histogram_statistics(histogram, n)
      singleton_count = n - edge_count - component_count
      raise CalculationError, 'negative singleton count' if singleton_count.negative?

      # This is C_alpha(n,1), evaluated directly rather than by another
      # histogram convolution.
      input_coefficient = tiling_multiplicity(
        n - edge_count, component_count, histogram_factor
      )

      term = input_coefficient * output_coefficient
      term *= factorial(singleton_count) * histogram_factor
      term *= 2**component_count
      result += edge_count.even? ? term : -term
    end

    unless result.between?(0, factorial(n))
      raise CalculationError, "A(#{n},#{k}) is outside 0..#{n}!"
    end
    result
  end

  def antidiagonals(maximum_diagonal)
    terms = []
    0.upto(maximum_diagonal) do |diagonal|
      0.upto(diagonal) do |n|
        terms << formula_value(n, diagonal - n)
      end
    end
    terms
  end

  # Independent direct implementation of the definition, used only by
  # --check and never by the normal calculation path.
  def brute_value(n, k)
    (1..n).to_a.permutation.count do |permutation|
      permutation.each_cons(2).all? do |left, right|
        (right - left).abs != k
      end
    end
  end

  def check
    actual = antidiagonals(5)
    unless actual == KNOWN_THROUGH_5
      raise CalculationError,
            "known-prefix mismatch:\nexpected #{KNOWN_THROUGH_5.inspect}\n" \
            "     got #{actual.inspect}"
    end

    KNOWN_K1.each_with_index do |expected, n|
      value = formula_value(n, 1)
      next if value == expected

      raise CalculationError,
            "A002464 mismatch at A(#{n},1): got #{value}, expected #{expected}"
    end

    0.upto(CHECK_MAX_DIAGONAL) do |diagonal|
      0.upto(diagonal) do |n|
        k = diagonal - n
        formula = formula_value(n, k)
        direct = brute_value(n, k)
        next if formula == direct

        raise CalculationError,
              "direct-enumeration mismatch at A(#{n},#{k}): " \
              "formula=#{formula}, direct=#{direct}"
      end
    end

    warn "ok: tiling-partition formula verifies A(n,1) through n=20 " \
         "and agrees with direct enumeration through antidiagonal " \
         "#{CHECK_MAX_DIAGONAL}"
  end

  def usage(program)
    <<~USAGE
      usage: #{program} [MAX_DIAGONAL]
             #{program} --upto MAX_DIAGONAL
             #{program} --term N K
             #{program} --check

      MAX_DIAGONAL defaults to #{DEFAULT_MAX_DIAGONAL} and may be 0..#{MAX_DIAGONAL}.
      Normal calculation uses the Spahn--Zeilberger tiling-partition formula.
      No b-file is created.
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

      n = parse_nonnegative_integer(arguments[1], 'N', MAX_DIAGONAL)
      k = parse_nonnegative_integer(arguments[2], 'K', MAX_DIAGONAL)
      raise InputError, "N+K must be at most #{MAX_DIAGONAL}" if n + k > MAX_DIAGONAL

      puts formula_value(n, k)
      return
    end

    maximum_diagonal =
      case arguments.length
      when 0
        DEFAULT_MAX_DIAGONAL
      when 1
        parse_nonnegative_integer(arguments[0], 'MAX_DIAGONAL', MAX_DIAGONAL)
      when 2
        raise InputError, usage(program) unless arguments[0] == '--upto'

        parse_nonnegative_integer(arguments[1], 'MAX_DIAGONAL', MAX_DIAGONAL)
      else
        raise InputError, usage(program)
      end

    puts antidiagonals(maximum_diagonal).join(', ')
  end
end

if __FILE__ == $PROGRAM_NAME
  begin
    A398634TilingFormula.run(ARGV, $PROGRAM_NAME)
  rescue A398634TilingFormula::InputError,
         A398634TilingFormula::CalculationError => error
    warn "error: #{error.message}"
    exit 1
  end
end
