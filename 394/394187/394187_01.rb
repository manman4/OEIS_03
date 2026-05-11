#!/usr/bin/env ruby
# Triangle T(n,k), n >= 1, 1 <= k <= n, read by rows,
# where T(n,k) is the number of rectangular plane partitions of n
# so that the rows and columns are strictly decreasing with exactly k rows.
#
# This implementation does not use MacMahon's product formula.
# It counts admissible k-row rectangular arrays directly by recursive
# enumeration of columns c_i = (c_i[0], ..., c_i[k-1]) with:
#   c_i[0] > c_i[1] > ... > c_i[k-1] >= 1,
#   c_1[j] > c_2[j] > ... for each row index j,
#   sum over all entries = n.

def count_tails(sum_left, max_column, memo)
  key = [sum_left, max_column]
  return memo[key] if memo.key?(key)

  total = (sum_left == 0 ? 1 : 0)
  build_columns(sum_left, max_column, 0, max_column[0], [], &lambda { |column, rest|
    total += count_tails(rest, column, memo)
  })

  memo[key] = total
end

def build_columns(sum_left, max_column, index, prev_value, column, &block)
  if index == max_column.length
    yield column.freeze, sum_left
    return
  end

  remaining = max_column.length - index - 1
  min_rest = remaining * (remaining + 1) / 2
  upper = [max_column[index] - 1, prev_value - 1, sum_left - min_rest].min
  lower = remaining + 1
  return if upper < lower

  lower.upto(upper) do |value|
    next_sum = sum_left - value
    next if next_sum < min_rest
    build_columns(next_sum, max_column, index + 1, value, column + [value], &block)
  end
end

def t(n, k)
  return 0 if k < 1

  min_sum = k * (k + 1) / 2
  return 0 if n < min_sum

  memo = {}
  count_tails(n, Array.new(k, n + 1), memo)
end

def triangle(rows)
  1.upto(rows).flat_map{|n|
    kmax = ((Math.sqrt(8 * n + 1) - 1) / 2).floor
    1.upto(kmax).map{|k| t(n, k)}
  }
end

p triangle(50)

# n列のT(n,k)合計を出力
# p (1..30).map{|n| (1..n).map{|k| t(n, k)}.sum}