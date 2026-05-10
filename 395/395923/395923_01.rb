#!/usr/bin/env ruby
# Computes Tr(n, k):
# number of rectangular plane partitions of n with exactly k rows.

def multiply_by_geometric(series, power, max_n)
  result = series.dup
  power.upto(max_n){|i|
    result[i] += result[i - power]
  }
  result
end

def T(n, k)
  total = 0
  1.upto(n / k){|s|
    coeffs = Array.new(n + 1, 0)
    coeffs[k * s] = 1
    1.upto(k){|i|
      1.upto(s){|j|
        coeffs = multiply_by_geometric(coeffs, i + j - 1, n)
      }
    }
    total += coeffs[n]
  }
  total
end

def a(n)
  (1..n).map{|i| (1..i).map{|j| T(i, j)}}.flatten
end

rows = 140
ary = a(rows)
(1..ary.size).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
