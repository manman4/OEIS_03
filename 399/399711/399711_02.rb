#!/usr/bin/env ruby
# frozen_string_literal: true

# Erdős problem #896, finite exact values.
#
# For A,B subsets of [N], let r(m) be the number of ordered pairs
# (a,b) in A x B with a*b=m, and put
#
#   F(A,B) = |{m : r(m)=1}|.
#
# This program computes
#
#   e896(N) = max F(A,B).
#
# It enumerates the smaller-cardinality side A.  For fixed A it chooses B by
# branch-and-bound.  For every product m the search maintains
#
#   selected[m]  = representations already selected in B,
#   remaining[m] = representations using an undecided element of B.
#
# Product m can still contribute only if selected[m] <= 1 and
# selected[m]+remaining[m] >= 1.  Counting all such products independently is
# therefore a rigorous upper bound.  No asymptotic estimate is used.
#
# The running time is exponential.  Ruby is suitable for initial terms, not
# for large N.  --method gray is a simpler independent exhaustive algorithm.

require "optparse"

class Erdos896
  MAX_N = 30

  attr_reader :best_a, :best_b, :nodes

  def initialize(n, progress: false)
    raise ArgumentError, "N must be in 1..#{MAX_N}" unless (1..MAX_N).cover?(n)

    @n = n
    @all = (1 << n) - 1
    @progress = progress
  end

  def score(a_mask, b_mask)
    count = Array.new(@n * @n + 1, 0)
    a = a_mask & @all
    until a.zero?
      abit = a & -a
      avalue = abit.bit_length
      b = b_mask & @all
      until b.zero?
        bbit = b & -b
        count[avalue * bbit.bit_length] += 1
        b ^= bbit
      end
      a ^= abit
    end
    count.count(1)
  end

  def solve_bnb(seed_a: 0, seed_b: 0)
    begin_solve("branch-and-bound")
    seed_candidate(seed_a, seed_b)
    seed_candidate(seed_b, seed_a)
    seed_candidate(1, @all)

    limit = 1 << @n
    (1...limit).each do |a_mask|
      @outer = a_mask
      a_values = mask_values(a_mask)
      a_size = a_values.length

      @products_by_b = Array.new(@n + 1) do |b|
        b.zero? ? nil : a_values.map { |a| a * b }
      end
      @remaining = Array.new(@n * @n + 1, 0)
      (1..@n).each do |b|
        @products_by_b[b].each { |product| @remaining[product] += 1 }
      end
      root_upper = @remaining.count { |representations| representations.positive? }
      next if root_upper <= @best

      greedy_order = greedy_seed(a_mask, a_size)
      # Products occurring for many different b's decide the upper bound most
      # strongly.  This changes only variable order, never which branches exist.
      rest = (1..@n).reject { |b| greedy_order.include?(b) }
      rest.sort_by! do |b|
        -@products_by_b[b].sum { |product| @remaining[product] - 1 }
      end
      @order = greedy_order + rest

      @selected = Array.new(@n * @n + 1, 0)
      @current_unique = 0
      @upper = root_upper
      search_b(0, 0, 0, a_size)
      outer_heartbeat(a_mask, limit)
    end

    finish_solve
  end

  # Independent exhaustive implementation.  For each A, B runs in Gray-code
  # order, so exactly one membership changes and representation counts can be
  # updated incrementally.  Symmetry permits checking only |A| <= |B|.
  def solve_gray(seed_a: 0, seed_b: 0)
    begin_solve("Gray-code exhaustive")
    seed_candidate(seed_a, seed_b)
    seed_candidate(seed_b, seed_a)
    seed_candidate(1, @all)

    limit = 1 << @n
    (1...limit).each do |a_mask|
      @outer = a_mask
      a_values = mask_values(a_mask)
      a_size = a_values.length
      representations = Array.new(@n * @n + 1, 0)
      unique = 0
      previous = 0
      b_size = 0

      (1...limit).each do |index|
        b_mask = index ^ (index >> 1)
        changed = b_mask ^ previous
        b_value = changed.bit_length
        if (b_mask & changed).zero?
          b_size -= 1
          a_values.each do |a|
            product = a * b_value
            old = representations[product]
            unique -= 1 if old == 1
            representations[product] = old - 1
            unique += 1 if old == 2
          end
        else
          b_size += 1
          a_values.each do |a|
            product = a * b_value
            old = representations[product]
            unique -= 1 if old == 1
            representations[product] = old + 1
            unique += 1 if old.zero?
          end
        end
        @nodes += 1
        node_heartbeat
        accept(a_mask, b_mask, unique) if b_size >= a_size && unique > @best
        previous = b_mask
      end
      outer_heartbeat(a_mask, limit)
    end
    finish_solve
  end

  private

  def popcount(value)
    count = 0
    until value.zero?
      value &= value - 1
      count += 1
    end
    count
  end

  def mask_values(mask)
    result = []
    until mask.zero?
      bit = mask & -mask
      result << bit.bit_length
      mask ^= bit
    end
    result
  end

  def monotonic_time
    Process.clock_gettime(Process::CLOCK_MONOTONIC)
  end

  def begin_solve(method)
    @best = 0
    @best_a = 0
    @best_b = 0
    @nodes = 0
    @started = monotonic_time
    @next_heartbeat = 10.0
    warn("erdos896_01: start N=#{@n} method=#{method}") if @progress
  end

  def finish_solve
    actual = score(@best_a, @best_b)
    raise "internal witness verification failed: #{actual} != #{@best}" unless actual == @best

    if @progress
      warn(format("erdos896_01: N=%d exact=%d nodes=%d %.3f s",
                  @n, @best, @nodes, monotonic_time - @started))
    end
    @best
  end

  def seed_candidate(a_mask, b_mask)
    a_mask &= @all
    b_mask &= @all
    return if a_mask.zero? || b_mask.zero?

    value = score(a_mask, b_mask)
    accept(a_mask, b_mask, value) if value > @best
  end

  def accept(a_mask, b_mask, value)
    # This direct check is deliberately retained at every improvement.  It
    # protects the reported witness from a counter-update bookkeeping error.
    actual = score(a_mask, b_mask)
    raise "internal score mismatch: #{value} != #{actual}" unless value == actual

    @best = value
    @best_a = a_mask
    @best_b = b_mask
    warn("erdos896_01: improved N=#{@n} lower_bound=#{@best} " \
         "|A|=#{popcount(a_mask)} |B|=#{popcount(b_mask)}") if @progress
  end

  # Produce a valid B of size at least |A|.  Its elements are also tried first
  # by the exact search, which tends to obtain a useful lower bound early.
  def greedy_seed(a_mask, minimum_size)
    counts = Array.new(@n * @n + 1, 0)
    chosen = []
    available = (1..@n).to_a
    unique = 0

    until available.empty?
      best_b = nil
      best_delta = -(@n * @n + 1)
      available.each do |b|
        delta = 0
        @products_by_b[b].each do |product|
          delta += 1 if counts[product].zero?
          delta -= 1 if counts[product] == 1
        end
        if delta > best_delta
          best_delta = delta
          best_b = b
        end
      end
      break if chosen.length >= minimum_size && best_delta <= 0

      chosen << best_b
      available.delete(best_b)
      @products_by_b[best_b].each do |product|
        unique -= 1 if counts[product] == 1
        counts[product] += 1
        unique += 1 if counts[product] == 1
      end
    end

    b_mask = chosen.sum { |b| 1 << (b - 1) }
    accept(a_mask, b_mask, unique) if chosen.length >= minimum_size && unique > @best
    chosen
  end

  def can_still_be_unique?(selected, remaining)
    selected <= 1 && selected + remaining >= 1
  end

  def adjust_product(product, selected_delta, remaining_delta)
    old_selected = @selected[product]
    old_remaining = @remaining[product]
    @current_unique -= 1 if old_selected == 1
    @upper -= 1 if can_still_be_unique?(old_selected, old_remaining)

    new_selected = old_selected + selected_delta
    new_remaining = old_remaining + remaining_delta
    raise "negative representation counter" if new_selected.negative? || new_remaining.negative?

    @selected[product] = new_selected
    @remaining[product] = new_remaining
    @current_unique += 1 if new_selected == 1
    @upper += 1 if can_still_be_unique?(new_selected, new_remaining)
  end

  def search_b(index, b_mask, b_size, minimum_size)
    @nodes += 1
    node_heartbeat

    if b_size >= minimum_size && @current_unique > @best
      accept(@outer, b_mask, @current_unique)
    end
    return if @upper <= @best
    return if b_size + @n - index < minimum_size
    return if index == @n

    b = @order[index]
    products = @products_by_b[b]

    # Include b.
    products.each { |product| adjust_product(product, 1, -1) }
    search_b(index + 1, b_mask | (1 << (b - 1)), b_size + 1, minimum_size)
    products.reverse_each { |product| adjust_product(product, -1, 1) }

    # Exclude b.
    products.each { |product| adjust_product(product, 0, -1) }
    search_b(index + 1, b_mask, b_size, minimum_size)
    products.reverse_each { |product| adjust_product(product, 0, 1) }
  end

  def outer_heartbeat(a_mask, limit)
    return unless @progress

    elapsed = monotonic_time - @started
    return if elapsed < @next_heartbeat

    warn(format("erdos896_01: progress N=%d best=%d A=%d/%d nodes=%d elapsed=%.1f s",
                @n, @best, a_mask, limit - 1, @nodes, elapsed))
    @next_heartbeat = elapsed + 10.0
  end

  # Avoid reading the clock at every node, while still reporting during one
  # unusually expensive fixed-A search.
  def node_heartbeat
    return unless @progress && (@nodes & ((1 << 18) - 1)).zero?

    outer_heartbeat(@outer, (1 << @n))
  end
end

def format_set(mask, n)
  "{#{(1..n).select { |v| ((mask >> (v - 1)) & 1) == 1 }.join(',')}}"
end

def run_self_check
  previous_a = 0
  previous_b = 0
  (1..10).each do |n|
    bnb = Erdos896.new(n)
    a = bnb.solve_bnb(seed_a: previous_a, seed_b: previous_b)
    gray = Erdos896.new(n)
    b = gray.solve_gray
    raise "self-check mismatch at N=#{n}: bnb=#{a}, gray=#{b}" unless a == b

    previous_a = bnb.best_a
    previous_b = bnb.best_b
    warn("erdos896_01: checked N=#{n} exact=#{a}")
  end
  warn("erdos896_01: self-check passed through N=10")
end

options = {
  start: 1,
  start_given: false,
  term: nil,
  progress: false,
  witness: false,
  method: "bnb",
  check: false
}

parser = OptionParser.new do |opts|
  opts.banner = "Usage: ruby erdos896_01.rb [N] [options]"
  opts.on("--start N", Integer, "start at N when computing a range") do |n|
    options[:start] = n
    options[:start_given] = true
  end
  opts.on("--term N", Integer, "compute only e896(N)") { |n| options[:term] = n }
  opts.on("--method NAME", %w[bnb gray], "bnb (default) or gray") { |m| options[:method] = m }
  opts.on("--progress", "print progress about every 10 seconds") { options[:progress] = true }
  opts.on("--witness", "print maximizing A and B") { options[:witness] = true }
  opts.on("--check", "compare two independent algorithms for N=1..10") { options[:check] = true }
  opts.on("-h", "--help", "show this help") do
    puts opts
    exit
  end
end

begin
  parser.parse!(ARGV)
  if options[:check]
    unless ARGV.empty? && options[:term].nil? && !options[:start_given]
      raise OptionParser::InvalidArgument, "--check takes no N, --term, or --start"
    end
    run_self_check
    exit
  end
  raise OptionParser::InvalidArgument, "at most one positional N is allowed" if ARGV.length > 1
  if options[:term] && (!ARGV.empty? || options[:start_given])
    raise OptionParser::InvalidArgument, "do not combine --term with positional N or --start"
  end

  last = options[:term] || (ARGV.empty? ? 10 : Integer(ARGV[0], 10))
  first = options[:term] || options[:start]
  unless (1..Erdos896::MAX_N).cover?(first) &&
         (1..Erdos896::MAX_N).cover?(last) && first <= last
    raise OptionParser::InvalidArgument,
          "range must satisfy 1 <= start <= N <= #{Erdos896::MAX_N}"
  end

  $stdout.sync = true
  seed_a = 0
  seed_b = 0
  (first..last).each do |n|
    solver = Erdos896.new(n, progress: options[:progress])
    answer = if options[:method] == "gray"
               solver.solve_gray(seed_a: seed_a, seed_b: seed_b)
             else
               solver.solve_bnb(seed_a: seed_a, seed_b: seed_b)
             end
    puts "#{n} #{answer}"
    if options[:witness]
      warn("erdos896_01: N=#{n} A=#{format_set(solver.best_a, n)} " \
           "B=#{format_set(solver.best_b, n)}")
    end
    seed_a = solver.best_a
    seed_b = solver.best_b
  end
rescue OptionParser::ParseError, ArgumentError => e
  warn("error: #{e.message}")
  warn(parser)
  exit(false)
end
