#!/usr/bin/env ruby
# frozen_string_literal: true

# Square array A(n,k), n >= 0, k >= 0, read by antidiagonals
# downwards, where A(n,k) is the number of permutations p of [n]
# such that
#
#   |p(i+1) - p(i)| != k for every 1 <= i < n.
#
# Algorithm 02: q target-assignment convolution.
#
# Apply inclusion-exclusion to selected adjacency constraints.  If h_l is
# the number of selected components having l edges, put
#
#   r = sum_l l*h_l,  c = sum_l h_l,
#   W(H) = (n-r)_c / product_l h_l!.
#
# Split the values 1,...,n into their k residue paths.  For a target path
# with m vertices, a local histogram x has multiplicity
#
#   (m-sum_l l*x_l)_(sum_l x_l) / product_l x_l!.
#
# Convolution over the residue paths gives Q(H).  Tauraso's quantity is
#
#   q(H) = product_l h_l! * Q(H),
#
# and the answer is
#
#   A(n,k) = sum_H (-1)^r * 2^c * (n-r-c)! * W(H) * q(H).
#
# This is the same q target-assignment algorithm as 398599_02.c, translated
# to Ruby Integer arithmetic.  It does not enumerate permutations.  The
# direct enumeration used by --check is independent verification only.
#
# References:
#   R. Tauraso, "The Dinner Table Problem: The Rectangular Case",
#   https://math.colgate.edu/~integers/g11/g11.pdf
#   G. Spahn and D. Zeilberger,
#   https://doi.org/10.54550/ECA2023V3S2R10
#
# Antidiagonal d is emitted in the order
#
#   A(0,d), A(1,d-1), ..., A(d,0).
#
# Usage:
#   ./398634_02.rb
#   ./398634_02.rb --upto 5
#   ./398634_02.rb 5
#   ./398634_02.rb --term 6 2
#   ./398634_02.rb --check
#
# --upto D prints all antidiagonals d=0..D on one comma-separated line.
# No b-file is created.

module A398634TargetConvolution
  DEFAULT_MAX_DIAGONAL = 5
  MAX_DIAGONAL = 30
  CHECK_MAX_DIAGONAL = 8

  KNOWN_THROUGH_5 = [
    1,
    1, 1,
    1, 1, 2,
    1, 1, 0, 6,
    1, 1, 2, 0, 24,
    1, 1, 2, 2, 2, 120
  ].freeze

  class InputError < StandardError; end
  class CalculationError < StandardError; end

  @factorials = [1]
  @target_pattern_cache = {}

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

  def falling_factorial(available, count)
    value = 1
    count.times { |offset| value *= available - offset }
    value
  end

  # (available)_component_count / product_l h_l!
  def assignment_multiplicity(available, component_count, histogram_factor)
    numerator = falling_factorial(available, component_count)
    quotient, remainder = numerator.divmod(histogram_factor)
    unless remainder.zero?
      raise CalculationError, 'nonintegral component-assignment multiplicity'
    end
    quotient
  end

  def generate_target_recursive(path_size, length, edge_count,
                                component_count, histogram, output)
    if length >= path_size
      histogram_factor = histogram.reduce(1) do |product, count|
        product * factorial(count)
      end
      coefficient = assignment_multiplicity(
        path_size - edge_count, component_count, histogram_factor
      )
      output << [histogram.dup.freeze, coefficient].freeze
      return
    end

    remaining_vertices = path_size - edge_count - component_count
    maximum_count = remaining_vertices / (length + 1)
    0.upto(maximum_count) do |count|
      histogram[length - 1] = count
      generate_target_recursive(
        path_size,
        length + 1,
        edge_count + length * count,
        component_count + count,
        histogram,
        output
      )
    end
  end

  # Return all local histograms and their unlabeled placement coefficients
  # for one residue path of the given number of vertices.
  def target_patterns(path_size)
    cached = @target_pattern_cache[path_size]
    return cached if cached

    output = []
    histogram = Array.new([path_size - 1, 0].max, 0)
    generate_target_recursive(path_size, 1, 0, 0, histogram, output)
    @target_pattern_cache[path_size] = output.freeze
  end

  def convolve_target_path(current, patterns)
    following = Hash.new(0)
    current.each do |histogram, coefficient|
      patterns.each do |local_histogram, local_coefficient|
        combined = histogram.dup
        local_histogram.each_index do |index|
          combined[index] += local_histogram[index]
        end
        following[combined.freeze] += coefficient * local_coefficient
      end
    end
    following
  end

  # Q(H): assign all component histograms among the k value-residue paths.
  def target_coefficients(n, k)
    key_length = n - 1
    current = { Array.new(key_length, 0).freeze => 1 }

    1.upto(k) do |residue|
      path_size = (n - residue) / k + 1
      # A one-vertex path contributes only the multiplicative identity.
      next if path_size == 1

      current = convolve_target_path(current, target_patterns(path_size))
    end
    current
  end

  def histogram_statistics(histogram)
    edge_count = 0
    component_count = 0
    histogram_factor = 1
    histogram.each_with_index do |count, index|
      next if count.zero?

      edge_count += (index + 1) * count
      component_count += count
      histogram_factor *= factorial(count)
    end
    [edge_count, component_count, histogram_factor]
  end

  def formula_value(n, k)
    # Difference zero cannot occur between distinct permutation entries;
    # if n<=k, no two values in [n] have difference k.
    return factorial(n) if k.zero? || n <= k

    result = 0
    target_coefficients(n, k).each do |histogram, target_coefficient|
      edge_count, component_count, histogram_factor =
        histogram_statistics(histogram)
      singleton_count = n - edge_count - component_count
      raise CalculationError, 'negative singleton count' if singleton_count.negative?

      source_multiplicity = assignment_multiplicity(
        n - edge_count, component_count, histogram_factor
      )
      tauraso_q = histogram_factor * target_coefficient
      term = 2**component_count
      term *= factorial(singleton_count)
      term *= source_multiplicity * tauraso_q
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

    warn "ok: q target-assignment convolution agrees with direct " \
         "enumeration through antidiagonal #{CHECK_MAX_DIAGONAL}"
  end

  def usage(program)
    <<~USAGE
      usage: #{program} [MAX_DIAGONAL]
             #{program} --upto MAX_DIAGONAL
             #{program} --term N K
             #{program} --check

      MAX_DIAGONAL defaults to #{DEFAULT_MAX_DIAGONAL} and may be 0..#{MAX_DIAGONAL}.
      Normal calculation uses q target-assignment convolution, not permutation enumeration.
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
    A398634TargetConvolution.run(ARGV, $PROGRAM_NAME)
  rescue A398634TargetConvolution::InputError,
         A398634TargetConvolution::CalculationError => error
    warn "error: #{error.message}"
    exit 1
  end
end
