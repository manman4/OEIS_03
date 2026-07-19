#!/usr/bin/env ruby
# frozen_string_literal: true

# A397124
# Direct count of all optimal placements for the non-dominating queens problem.
#
# This version deliberately uses no board symmetry at all:
#   - no rotations or reflections,
#   - no canonical representative test,
#   - no symmetry-based prefix pruning,
#   - no orbit/stabilizer calculation.
#
# Every n-element subset of the n X n board that survives the attack-count
# bound is considered directly.  Therefore +all_solutions+ is an independent
# count with rotations and reflections distinguished.

KNOWN_FREE = {
  1 => 0,   2 => 0,   3 => 0,   4 => 1,   5 => 3,
  6 => 5,   7 => 7,   8 => 11,  9 => 18, 10 => 22,
  11 => 30, 12 => 36, 13 => 47, 14 => 56, 15 => 72,
  16 => 82, 17 => 97, 18 => 111, 19 => 132, 20 => 145,
  21 => 170, 22 => 186, 23 => 216, 24 => 240, 25 => 260,
  26 => 290, 27 => 324, 28 => 360, 29 => 381, 30 => 420
}.freeze

class DirectNonDominatingQueens
  Result = Struct.new(:n, :free, :all_solutions, :nodes, keyword_init: true)

  POPCOUNT_16 = Array.new(1 << 16, 0).tap do |table|
    1.upto(table.length - 1) { |x| table[x] = table[x >> 1] + (x & 1) }
  end.freeze

  def initialize(n, known_free: KNOWN_FREE[n])
    raise ArgumentError, "n must be positive" unless n.positive?

    @n = n
    @size = n * n
    @attack_masks = build_attack_masks
    @known_free = known_free
  end

  def solve
    reset_search(@known_free.nil? ? @size : @size - @known_free)
    search(0, 0, 0, 0)

    # A target that is too high is safe for verification, but it cannot reveal
    # the actual lower optimum.  Retry without a target if that ever happens.
    unless @found
      previous_nodes = @nodes
      reset_search(@size)
      search(0, 0, 0, 0)
      @nodes += previous_nodes
    end

    Result.new(
      n: @n,
      free: @size - @best_attacked,
      all_solutions: @all_solutions,
      nodes: @nodes
    )
  end

  private

  def reset_search(attack_limit)
    @best_attacked = attack_limit
    @all_solutions = 0
    @nodes = 0
    @found = false
  end

  def build_attack_masks
    Array.new(@size) do |queen|
      qr, qc = queen.divmod(@n)
      mask = 0

      @n.times do |r|
        @n.times do |c|
          if r == qr || c == qc || (r - qr).abs == (c - qc).abs
            mask |= 1 << (r * @n + c)
          end
        end
      end
      mask
    end
  end

  def search(start_square, depth, attacked, attacked_count)
    @nodes += 1

    if depth == @n
      if attacked_count < @best_attacked
        @best_attacked = attacked_count
        @all_solutions = 0
        @found = false
      end
      return unless attacked_count == @best_attacked

      @found = true
      @all_solutions += 1
      return
    end

    remaining = @n - depth
    last = @size - remaining
    not_attacked = ~attacked
    square = start_square

    while square <= last
      new_bits = @attack_masks[square] & not_attacked
      budget = @best_attacked - attacked_count

      if budget.zero?
        if new_bits.zero?
          search(square + 1, depth + 1, attacked, attacked_count)
        end
      else
        added = popcount_up_to(new_bits, budget)
        if added <= budget
          search(square + 1, depth + 1, attacked | new_bits, attacked_count + added)
        end
      end
      square += 1
    end
  end

  def popcount_up_to(value, limit)
    count = 0
    until value.zero?
      count += POPCOUNT_16[value & 0xffff]
      return limit + 1 if count > limit

      value >>= 16
    end
    count
  end
end

def usage
  warn "Usage: ruby #{File.basename($PROGRAM_NAME)} N"
  warn "       ruby #{File.basename($PROGRAM_NAME)} --upto N"
  exit 1
end

arguments = ARGV.dup
ns = if arguments.length == 1 && arguments[0].match?(/\A[1-9]\d*\z/)
       [Integer(arguments[0])]
     elsif arguments.length == 2 && arguments[0] == "--upto" &&
           arguments[1].match?(/\A[1-9]\d*\z/)
       1..Integer(arguments[1])
     else
       usage
     end

all_verified = true
ns.each do |n|
  result = DirectNonDominatingQueens.new(n).solve
  expected_free = KNOWN_FREE[n]
  matches = expected_free.nil? || result.free == expected_free
  all_verified &&= matches
  status = if expected_free.nil?
             ""
           elsif matches
             " A001366_OK"
           else
             " expected_A001366=#{expected_free}"
           end

  puts "n=#{n} A001366(n)=#{result.free}" \
       " all=#{result.all_solutions} nodes=#{result.nodes}#{status}"
end

exit 2 unless all_verified
