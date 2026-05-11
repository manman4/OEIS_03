#!/usr/bin/env ruby
# Triangle T(n,k), n >= 1, 1 <= k <= kmax(n), read by rows,
# where T(n,k) is the number of rectangular plane partitions of n
# with exactly k rows and with rows and columns strictly decreasing.
#
# This implementation uses the generating function of column k:
#   Sum_{l>=1} q^(k*l*(k+l)/2)
#     * Product_{i=1..k} Product_{j=1..l} 1/(1 - q^(i+j-1)).

def shift_series(series, shift, max_n)
  shifted = Array.new(max_n + 1, 0)
  0.upto(max_n - shift){|i|
    shifted[i + shift] = series[i]
  }
  shifted
end

def multiply_by_geometric(series, power, max_n)
  power.upto(max_n){|i|
    series[i] += series[i - power]
  }
end

def row_kmax(n)
  k = 0
  k += 1 while (k + 1) * (k + 2) / 2 <= n
  k
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
    1.upto(k){|i|
      multiply_by_geometric(rect, l + i - 1, max_n)
    }
    min_sum.upto(max_n){|n|
      coeffs[n] += rect[n]
    }
    l += 1
  end

  coeffs
end

def triangle(rows)
  kmax = row_kmax(rows)
  columns = 1.upto(kmax).map{|k| strict_column_gf(rows, k)}

  1.upto(rows).map{|n|
    kmax = row_kmax(n)
    1.upto(kmax).map{|k| columns[k - 1][n]}.inject(:+)
  }
end

n = 5000
ary = [1] + triangle(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
