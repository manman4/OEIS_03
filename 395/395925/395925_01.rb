#!/usr/bin/env ruby
# Number of rectangular plane partitions of n with exactly 3 rows.
# This implementation does not use MacMahon's product formula.
# It counts admissible 3-row rectangular arrays directly by recursive enumeration
# of columns (a_i, b_i, c_i) with:
#   a_i >= b_i >= c_i >= 1,
#   a_1 >= a_2 >= ...,
#   b_1 >= b_2 >= ...,
#   c_1 >= c_2 >= ...,
#   sum_i (a_i + b_i + c_i) = n.

def count_tails(sum_left, max_top, max_middle, max_bottom, memo)
  key = [sum_left, max_top, max_middle, max_bottom]
  return memo[key] if memo.key?(key)

  total = (sum_left == 0 ? 1 : 0)
  max_c = [max_bottom, sum_left / 3].min
  1.upto(max_c){|bottom|
    max_b = [max_middle, (sum_left - bottom) / 2].min
    bottom.upto(max_b){|middle|
      max_a = [max_top, sum_left - middle - bottom].min
      middle.upto(max_a){|top|
        total += count_tails(sum_left - top - middle - bottom, top, middle, bottom, memo)
      }
    }
  }

  memo[key] = total
end

def t(n)
  return 0 if n < 3

  memo = {}
  count_tails(n, n, n, n, memo)
end

def sequence(length)
  0.upto(length).map { |n| t(n) }
end

p sequence(30)

