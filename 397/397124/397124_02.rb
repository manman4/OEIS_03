#!/usr/bin/env ruby
# frozen_string_literal: true

require "etc"

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
# Parallel workers merely partition the possible first square; they do not
# identify, discard, or merge symmetry-related placements.

KNOWN_FREE = {
  1 => 0,   2 => 0,   3 => 0,   4 => 1,   5 => 3,
  6 => 5,   7 => 7,   8 => 11,  9 => 18, 10 => 22,
  11 => 30, 12 => 36, 13 => 47, 14 => 56, 15 => 72,
  16 => 82, 17 => 97, 18 => 111, 19 => 132, 20 => 145,
  21 => 170, 22 => 186, 23 => 216, 24 => 240, 25 => 260,
  26 => 290, 27 => 324, 28 => 360, 29 => 381, 30 => 420
}.freeze

DEFAULT_JOBS = Process.respond_to?(:fork) ? [[Etc.nprocessors, 8].min, 1].max : 1

class DirectNonDominatingQueens
  Result = Struct.new(:n, :free, :all_solutions, :nodes, keyword_init: true)
  Attempt = Struct.new(:best_attacked, :all_solutions, :nodes, :found, keyword_init: true)

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

  def solve(jobs: 1)
    first_squares = (0..@size - @n).to_a
    attack_limit = @known_free.nil? ? @size : @size - @known_free
    attempt = run_attempt(first_squares, attack_limit, jobs)

    # A target that is too high is safe for verification, but it cannot reveal
    # the actual lower optimum.  Retry without a target if that ever happens.
    unless attempt.found
      previous_nodes = attempt.nodes
      attempt = run_attempt(first_squares, @size, jobs)
      attempt.nodes += previous_nodes
    end

    Result.new(
      n: @n,
      free: @size - attempt.best_attacked,
      all_solutions: attempt.all_solutions,
      nodes: attempt.nodes
    )
  end

  private

  def reset_search(attack_limit)
    @best_attacked = attack_limit
    @all_solutions = 0
    @nodes = 0
    @found = false
  end

  def run_attempt(first_squares, attack_limit, jobs)
    worker_count = [[jobs, first_squares.length].min, 1].max
    return solve_first_squares(first_squares, attack_limit, include_root: true) if worker_count == 1

    groups = Array.new(worker_count) { [] }
    first_squares.each_with_index { |square, i| groups[i % worker_count] << square }
    readers = []
    pids = []

    groups.each do |group|
      reader, writer = IO.pipe
      pid = fork do
        reader.close
        payload = begin
          [:ok, solve_first_squares(group, attack_limit, include_root: false)]
        rescue Exception => error # process boundary: report even nonstandard failures
          [:error, error.class.name, error.message, error.backtrace]
        end
        Marshal.dump(payload, writer)
        writer.close
        exit! 0
      end
      writer.close
      readers << reader
      pids << pid
    end

    statuses = []
    attempts = begin
      readers.map do |reader|
        payload = Marshal.load(reader)
        reader.close
        if payload[0] == :error
          raise "worker failed: #{payload[1]}: #{payload[2]}\n#{payload[3].join("\n")}"
        end
        payload[1]
      end
    ensure
      readers.each { |reader| reader.close unless reader.closed? }
      pids.each do |pid|
        _finished_pid, status = Process.wait2(pid)
        statuses << status
      rescue Errno::ECHILD
        # Already reaped; nothing remains to clean up.
      end
    end
    raise "worker process failed" unless statuses.all?(&:success?)

    merge_attempts(attempts)
  end

  def solve_first_squares(first_squares, attack_limit, include_root:)
    reset_search(attack_limit)
    @nodes = 1 if include_root

    first_squares.each do |first|
      attacked = @attack_masks[first]
      attacked_count = popcount_up_to(attacked, @best_attacked)
      next if attacked_count > @best_attacked

      search(first + 1, 1, attacked, attacked_count)
    end

    Attempt.new(
      best_attacked: @best_attacked,
      all_solutions: @all_solutions,
      nodes: @nodes,
      found: @found
    )
  end

  def merge_attempts(attempts)
    found_attempts = attempts.select(&:found)
    return Attempt.new(best_attacked: @size, all_solutions: 0,
                       nodes: 1 + attempts.sum(&:nodes), found: false) if found_attempts.empty?

    best_attacked = found_attempts.map(&:best_attacked).min
    best_attempts = found_attempts.select { |attempt| attempt.best_attacked == best_attacked }
    Attempt.new(
      best_attacked: best_attacked,
      all_solutions: best_attempts.sum(&:all_solutions),
      nodes: 1 + attempts.sum(&:nodes),
      found: true
    )
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
  warn "       ruby #{File.basename($PROGRAM_NAME)} --jobs J N"
  warn "       ruby #{File.basename($PROGRAM_NAME)} --jobs J --upto N"
  exit 1
end

arguments = ARGV.dup
jobs = DEFAULT_JOBS
if arguments.length >= 2 && arguments[0] == "--jobs" &&
   arguments[1].match?(/\A[1-9]\d*\z/)
  jobs = Integer(arguments[1])
  arguments.shift(2)
end
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
  result = DirectNonDominatingQueens.new(n).solve(jobs: jobs)
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
