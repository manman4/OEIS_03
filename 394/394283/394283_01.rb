#!/usr/bin/env ruby
# Number of rectangular plane partitions of n with exactly 2 rows,
# with rows and columns strictly decreasing.
#
# This implementation does not use MacMahon's product formula.
# It counts admissible 2-row rectangular arrays directly by recursive enumeration
# of columns (a_i, b_i) with:
#   a_i > b_i >= 1,
#   a_1 > a_2 > ...,
#   b_1 > b_2 > ...,
#   sum_i (a_i + b_i) = n.

def count_tails(sum_left, max_top, max_bottom, memo)
  key = [sum_left, max_top, max_bottom]
  return memo[key] if memo.key?(key)

  total = (sum_left == 0 ? 1 : 0)
  max_b = [max_bottom - 1, (sum_left - 1) / 2].min
  1.upto(max_b) do |bottom|
    max_a = [max_top - 1, sum_left - bottom].min
    (bottom + 1).upto(max_a) do |top|
      total += count_tails(sum_left - top - bottom, top, bottom, memo)
    end
  end

  memo[key] = total
end

def t(n)
  return 0 if n < 3

  memo = {}
  count_tails(n, n + 1, n + 1, memo)
end

def sequence(length)
  1.upto(length).map { |n| t(n) }
end


p sequence(30)

