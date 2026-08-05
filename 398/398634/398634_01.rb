#!/usr/bin/env ruby
# frozen_string_literal: true

# Square array A(n,k), n >= 0, k >= 0, read by antidiagonals
# downwards, where A(n,k) is the number of permutations p of [n]
# such that
#
#   |p(i+1) - p(i)| != k for every 1 <= i < n.
#
# This is deliberately a naive implementation of the definition.  It
# enumerates every permutation and tests every adjacent pair; it does not
# use inclusion-exclusion, a recurrence, or any closed formula.
#
# Antidiagonal d is emitted in the order
#
#   A(0,d), A(1,d-1), ..., A(d,0).
#
# Usage:
#   ./398634_01.rb
#   ./398634_01.rb --upto 5
#   ./398634_01.rb 5
#   ./398634_01.rb --term 6 2
#   ./398634_01.rb --check
#
# --upto D prints all antidiagonals d=0..D on one comma-separated line.
# No b-file is created.

module A398634Naive
  DEFAULT_MAX_DIAGONAL = 5
  MAX_DIAGONAL = 10

  KNOWN_THROUGH_5 = [
    1,
    1, 1,
    1, 1, 2,
    1, 1, 0, 6,
    1, 1, 2, 0, 24,
    1, 1, 2, 2, 2, 120
  ].freeze

  class InputError < StandardError; end

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

  # Directly enumerate S_n and apply the defining condition.
  def value(n, k)
    (1..n).to_a.permutation.count do |permutation|
      permutation.each_cons(2).all? do |left, right|
        (right - left).abs != k
      end
    end
  end

  def antidiagonals(maximum_diagonal)
    terms = []
    0.upto(maximum_diagonal) do |diagonal|
      0.upto(diagonal) do |n|
        k = diagonal - n
        terms << value(n, k)
      end
    end
    terms
  end

  def check
    actual = antidiagonals(5)
    unless actual == KNOWN_THROUGH_5
      raise InputError,
            "check failed:\nexpected #{KNOWN_THROUGH_5.inspect}\n" \
            "     got #{actual.inspect}"
    end

    warn 'ok: direct permutation enumeration agrees through antidiagonal 5'
  end

  def usage(program)
    <<~USAGE
      usage: #{program} [MAX_DIAGONAL]
             #{program} --upto MAX_DIAGONAL
             #{program} --term N K
             #{program} --check

      MAX_DIAGONAL defaults to #{DEFAULT_MAX_DIAGONAL} and may be 0..#{MAX_DIAGONAL}.
      The calculation is factorial-time direct permutation enumeration.
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

      puts value(n, k)
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
    A398634Naive.run(ARGV, $PROGRAM_NAME)
  rescue A398634Naive::InputError => error
    warn "error: #{error.message}"
    exit 1
  end
end
