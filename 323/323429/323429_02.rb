#!/usr/bin/env ruby
# Computes a(n):
# number of rectangular plane partitions of n.

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

def a(max_n)
  raise ArgumentError, "max_n must be a nonnegative Integer" unless max_n.is_a?(Integer) && max_n >= 0

  totals = Array.new(max_n + 1, 0)
  totals[0] = 1

  1.upto(max_n){|rows|
    coeffs = [1] + Array.new(max_n, 0)
    1.upto(max_n / rows){|cols|
      coeffs = shift_series(coeffs, rows, max_n)
      1.upto(rows){|i|
        multiply_by_geometric(coeffs, cols + i - 1, max_n)
      }
      (rows * cols).upto(max_n){|n|
        totals[n] += coeffs[n]
      }
    }
  }

  totals
end

rows = 1000
ary = a(rows)
(0..rows).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
