#!/usr/bin/env ruby
# Triangle T(n,k), n >= 1, 1 <= k <= n, read by rows,
# where T(n,k) is the number of rectangular plane partitions of n
# with exactly k rows.
#
# This implementation does not use MacMahon's product formula.
# It counts admissible k-row rectangular arrays directly by recursive
# enumeration of columns c_i = (c_i[0], ..., c_i[k-1]) with:
#   c_i[0] >= c_i[1] >= ... >= c_i[k-1] >= 1,
#   c_1[j] >= c_2[j] >= ... for each row index j,
#   sum over all entries = n.

def count_tails(sum_left, max_column, memo)
  key = [sum_left, max_column]
  return memo[key] if memo.key?(key)

  total = (sum_left == 0 ? 1 : 0)
  build_columns(sum_left, max_column, 0, max_column[0], [], memo) do |column, rest|
    total += count_tails(rest, column, memo)
  end

  memo[key] = total
end

def build_columns(sum_left, max_column, index, prev_value, column, memo, &block)
  if index == max_column.length
    yield column.freeze, sum_left
    return
  end

  remaining = max_column.length - index - 1
  upper = [max_column[index], prev_value, sum_left - remaining].min
  return if upper < 1

  1.upto(upper) do |value|
    next_sum = sum_left - value
    break if next_sum < remaining
    build_columns(next_sum, max_column, index + 1, value, column + [value], memo, &block)
  end
end

def t(n, k)
  return 0 if k < 1 || n < k

  memo = {}
  count_tails(n, Array.new(k, n), memo)
end

def triangle(rows)
  1.upto(rows).flat_map do |n|
    1.upto(n).map { |k| t(n, k) }
  end
end

if __FILE__ == $PROGRAM_NAME
  rows = (ARGV[0] || 12).to_i
  p triangle(rows)
end
