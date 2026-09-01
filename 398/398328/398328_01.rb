#!/usr/bin/env ruby
# frozen_string_literal: true

# OEIS A398328
#
# Irregular triangle read by rows: T(n,k) is the number of permutations of
# [n] having exactly k runs of consecutive integers in either direction,
# n >= 0 and 0 <= k <= floor(n/2).
#
# A run is a maximal contiguous block of length at least 2 whose successive
# entries differ by 1 in absolute value.  For example, 145623 has two runs:
# 456 and 23.
#
# Let q = 1-y.  The bivariate ordinary generating function is
#
#   G(x,y) = sum(m >= 0, m! * B(x,y)^m),
#   B(x,y) = x * (1 - 2*q*x + q*x^2) / (1 - q*x^2),
#
# where T(n,k) = [x^n y^k] G(x,y).
#
# To extract the coefficients without a symbolic algebra library, expand
#
#   B^m = x^m * (1 - 2*q*x + q*x^2)^m * (1-q*x^2)^(-m).
#
# Choose a of the m numerator factors to contribute -2*q*x, choose b of
# the remaining factors to contribute q*x^2, and take the c-th term of the
# negative-binomial series.  The resulting monomial has
#
#   x-degree m+a+2*b+2*c,  q-degree a+b+c.
#
# Finally, [y^k]q^j = (-1)^k*binomial(j,k), since q=1-y.  All calculations
# below are therefore finite exact-integer sums.
#
# Usage:
#   ruby 398328_01.rb             # flattened rows n=0..10
#   ruby 398328_01.rb 20          # flattened rows n=0..20
#   ruby 398328_01.rb --rows 10   # one labeled row per line
#   ruby 398328_01.rb --term 8 2  # T(8,2)
#   ruby 398328_01.rb --check

module A398328
  DEFAULT_MAX_N = 10
  MAX_SUPPORTED_N = 150

  KNOWN_ROWS = [
    [1],
    [1],
    [0, 2],
    [0, 6],
    [2, 16, 6],
    [14, 56, 50],
    [90, 286, 310, 34],
    [646, 1866, 2010, 518],
    [5242, 14_300, 14_976, 5540, 262],
    [47_622, 124_620, 128_328, 55_956, 6354],
    [479_306, 1_214_890, 1_239_972, 589_204, 102_866, 2562]
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

  def binomial_table(n)
    table = Array.new(n + 1) { Array.new(n + 1, 0) }
    table[0][0] = 1

    1.upto(n) do |i|
      table[i][0] = 1
      table[i][i] = 1
      1.upto(i - 1) do |j|
        table[i][j] = table[i - 1][j - 1] + table[i - 1][j]
      end
    end

    table
  end

  def row(n)
    validate_n(n)
    return [1] if n.zero?

    choose = binomial_table(n)
    maximum_runs = n / 2
    q_coefficients = Array.new(maximum_runs + 1, 0)
    factorial = 1

    1.upto(n) do |m|
      factorial *= m

      0.upto(m) do |a|
        0.upto(m - a) do |b|
          remainder = n - m - a - 2 * b
          next if remainder.negative? || remainder.odd?

          c = remainder / 2
          q_power = a + b + c
          multinomial = choose[m][a] * choose[m - a][b]
          negative_binomial = choose[m + c - 1][c]

          q_coefficients[q_power] +=
            factorial * multinomial * ((-2)**a) * negative_binomial
        end
      end
    end

    counts = Array.new(maximum_runs + 1, 0)
    0.upto(maximum_runs) do |runs|
      coefficient = 0
      runs.upto(maximum_runs) do |q_power|
        coefficient += q_coefficients[q_power] * choose[q_power][runs]
      end
      counts[runs] = runs.odd? ? -coefficient : coefficient
    end

    unless counts.none?(&:negative?) && counts.sum == factorial
      raise CalculationError, "invalid row for n=#{n}"
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

  # Independent factorial-time implementation of the definition, used only
  # by --check for small n.
  def direct_row(n)
    counts = Array.new(n / 2 + 1, 0)

    (1..n).to_a.permutation do |permutation|
      runs = 0
      inside_run = false

      permutation.each_cons(2) do |left, right|
        consecutive = (left - right).abs == 1
        runs += 1 if consecutive && !inside_run
        inside_run = consecutive
      end

      counts[runs] += 1
    end

    counts
  end

  def check
    actual = rows(KNOWN_ROWS.length - 1)
    unless actual == KNOWN_ROWS
      raise CalculationError,
            "known-row check failed:\nexpected #{KNOWN_ROWS.inspect}\n" \
            "     got #{actual.inspect}"
    end

    0.upto(8) do |n|
      formula = row(n)
      direct = direct_row(n)
      next if formula == direct

      raise CalculationError,
            "direct check failed for n=#{n}: expected #{direct.inspect}, " \
            "got #{formula.inspect}"
    end

    warn 'ok: A398328 known rows n=0..10 and direct counts n=0..8 agree'
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
      cnt = 0
      triangle.each{|row|
        row.each{|i|
          break if i.to_s.size > 1000
          print cnt
          print ' '
          puts i
          cnt += 1
        }
      }
    end
  end
end

if __FILE__ == $PROGRAM_NAME
  begin
    A398328.run(ARGV, $PROGRAM_NAME)
  rescue A398328::InputError, A398328::CalculationError => error
    warn "error: #{error.message}"
    exit 1
  end
end
