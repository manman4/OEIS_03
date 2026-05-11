#!/usr/bin/env ruby
# Triangle T(n,k), n >= 1, 1 <= k <= floor((sqrt(8*n+1)-1)/2), read by rows,
# where T(n,k) is the number of rectangular plane partitions of n
# with exactly k rows and with rows and columns strictly decreasing.
#
# This implementation uses the generating function of column k:
#   Sum_{l>=1} q^(k*l*(k+l)/2)
#     * Product_{i=1..k} Product_{j=1..l} 1/(1 - q^(i+j-1)).

def shift_series(series, shift, max_n)
  shifted = Array.new(max_n + 1, 0)
  0.upto(max_n - shift) do |i|
    shifted[i + shift] = series[i]
  end
  shifted
end

def multiply_by_geometric(series, power, max_n)
  power.upto(max_n) do |i|
    series[i] += series[i - power]
  end
end

def strict_column_gf(max_n, k)
  coeffs = Array.new(max_n + 1, 0)
  rect = [1] + Array.new(max_n, 0)
  l = 1

  loop do
    min_sum = k * l * (k + l) / 2
    break if min_sum > max_n

    shift = k * (k + 2 * l - 1) / 2
    rect = shift_series(rect, shift, max_n)
    1.upto(k) do |i|
      multiply_by_geometric(rect, l + i - 1, max_n)
    end
    min_sum.upto(max_n) do |n|
      coeffs[n] += rect[n]
    end
    l += 1
  end

  coeffs
end

def triangle(rows)
  kmax = ((Math.sqrt(8 * rows + 1) - 1) / 2).floor
  columns = 1.upto(kmax).map { |k| strict_column_gf(rows, k) }

  1.upto(rows).flat_map do |n|
    row_kmax = ((Math.sqrt(8 * n + 1) - 1) / 2).floor
    1.upto(row_kmax).map { |k| columns[k - 1][n] }
  end
end

if __FILE__ == $PROGRAM_NAME
  rows = (ARGV[0] || 50).to_i
  p triangle(rows)
end
