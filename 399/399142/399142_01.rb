#!/usr/bin/env ruby
# frozen_string_literal: true

# OEIS A399142
#
# Triangle read by rows: T(n,k) counts trees on [n] rooted at 1 together
# with a level assignment L:[n]->{0,...,k} that uses every level, has
# L(1)=0, and strictly increases from parent to child, 0<=k<n.
#
# This program implements the combinatorial definition directly.  Suppose
# level r contains d vertices and all earlier levels contain e vertices.
# Choose the d vertices in binomial(remaining,d) ways.  Each of them may
# independently choose any of the e earlier vertices as its parent, giving
# e^d choices.
#
# Requiring each level to be nonempty makes L surjective.  Since levels
# strictly decrease when parent edges are followed, every choice reaches
# the unique level-0 vertex, namely the root 1.  It therefore produces a
# rooted tree, and every pair counted by T(n,k) is obtained exactly once.
# No recurrence for T and no formal-series iteration are used.
#
# Usage:
#   ruby 399142_01.rb
#   ruby 399142_01.rb --upto 12
#   ruby 399142_01.rb --rows 9
#   ruby 399142_01.rb --term 4 2
#   ruby 399142_01.rb --check
#
# With no arguments, the program prints the flattened rows n=1..9.

module A399142
  DEFAULT_MAX_N = 9
  MAX_SUPPORTED_N = 100

  KNOWN_ROWS = [
    [1],
    [0, 1],
    [0, 1, 4],
    [0, 1, 21, 36],
    [0, 1, 102, 552, 576],
    [0, 1, 535, 6990, 19_560, 14_400],
    [0, 1, 3098, 89_250, 504_720, 920_160, 518_400],
    [0, 1, 19_691, 1_211_084, 12_395_250, 42_038_640,
     55_974_240, 25_401_600],
    [0, 1, 136_062, 17_730_412, 310_725_800, 1_747_569_600,
     4_127_114_880, 4_293_596_160, 1_625_702_400]
  ].freeze

  class InputError < StandardError; end

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

  def binomial(n, k)
    return 0 if k.negative? || k > n

    k = [k, n - k].min
    value = 1
    1.upto(k) { |j| value = value * (n - k + j) / j }
    value
  end

  # Count completions after some initial levels have already been filled.
  # remaining: vertices not yet assigned a positive level
  # levels_left: nonempty levels still to be filled
  # earlier: vertices already placed in earlier levels, including the root
  def count_completions(remaining, levels_left, earlier, memo)
    return remaining.zero? ? 1 : 0 if levels_left.zero?
    return 0 if remaining < levels_left

    key = [remaining, levels_left]
    cached = memo[key]
    return cached unless cached.nil?

    maximum_level_size = remaining - levels_left + 1
    total = 0
    1.upto(maximum_level_size) do |level_size|
      choose_vertices = binomial(remaining, level_size)
      choose_parents = earlier**level_size
      total += choose_vertices * choose_parents * count_completions(
        remaining - level_size,
        levels_left - 1,
        earlier + level_size,
        memo
      )
    end

    memo[key] = total
  end

  def value(n, k)
    unless n.between?(1, MAX_SUPPORTED_N) && k.between?(0, n - 1)
      raise InputError,
            "indices must satisfy 1<=N<=#{MAX_SUPPORTED_N} and 0<=K<N"
    end

    return n == 1 ? 1 : 0 if k.zero?

    count_completions(n - 1, k, 1, {})
  end

  def row(n)
    0.upto(n - 1).map { |k| value(n, k) }
  end

  def rows(maximum_n)
    1.upto(maximum_n).map { |n| row(n) }
  end

  def check
    actual = rows(KNOWN_ROWS.length)
    unless actual == KNOWN_ROWS
      raise InputError,
            "check failed:\nexpected #{KNOWN_ROWS.inspect}\n" \
            "     got #{actual.inspect}"
    end

    warn 'ok: the combinatorial definition agrees with A399142 through n=9'
  end

  def usage(program)
    <<~USAGE
      usage: #{program}
             #{program} --upto N
             #{program} --rows N
             #{program} --term N K
             #{program} --check

      With no arguments, print rows 1..#{DEFAULT_MAX_N} as one flattened,
      comma-separated line.  N may be 1..#{MAX_SUPPORTED_N}.
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

      n = parse_integer(arguments[1], 'N', 1, MAX_SUPPORTED_N)
      k = parse_integer(arguments[2], 'K', 0, n - 1)
      puts value(n, k)
      return
    end

    mode, maximum_n =
      case arguments.length
      when 0
        [:flat, DEFAULT_MAX_N]
      when 2
        unless %w[--upto --rows].include?(arguments[0])
          raise InputError, usage(program)
        end
        selected_mode = arguments[0] == '--rows' ? :rows : :flat
        selected_n = parse_integer(
          arguments[1], 'N', 1, MAX_SUPPORTED_N
        )
        [selected_mode, selected_n]
      else
        raise InputError, usage(program)
      end

    triangle = rows(maximum_n)
    if mode == :rows
      triangle.each_with_index do |values, index|
        puts "n=#{index + 1}: #{values.join(', ')}"
      end
    else
      puts triangle.flatten.join(', ')
    end
  end
end

if __FILE__ == $PROGRAM_NAME
  begin
    A399142.run(ARGV, $PROGRAM_NAME)
  rescue A399142::InputError => e
    warn e.message
    exit 1
  end
end
