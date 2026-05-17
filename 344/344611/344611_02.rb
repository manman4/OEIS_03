# Row sums of A344610 computed from generating functions.
#
# For k > 0:
#   Sum_{n >= 0} A344649(n, k) * q^n
#     = Sum_{i=1..2*k} (-1)^(i-1) * q^(i*(i-1)/2 + k) / Product_{j=1..2*k-i} (1-q^j)
#
# and
#   Sum_{n >= 0} A344610(n, k) * q^n
#     = (Product_{j > 0} 1 / (1-q^j)) * (Sum_{n >= 0} A344649(n, k) * q^n).
#
# Therefore the row sums are obtained by summing the strict-column series over k
# and multiplying by the partition generating function.

def partition_series(max_n)
  ary = Array.new(max_n + 1, 0)
  ary[0] = 1
  (1..max_n).each{|part|
    (part..max_n).each{|sum|
      ary[sum] += ary[sum - part]
    }
  }
  ary
end

def bounded_partition_series(max_part, max_n)
  series = Array.new(max_part + 1){Array.new(max_n + 1, 0)}
  series[0][0] = 1
  (1..max_part).each{|part|
    ary = series[part - 1].dup
    (part..max_n).each{|sum|
      ary[sum] += ary[sum - part]
    }
    series[part] = ary
  }
  series
end

def strict_row_sum_series(max_n)
  bounded = bounded_partition_series(2 * max_n, max_n)
  ary = Array.new(max_n + 1, 0)
  ary[0] = 1

  (1..max_n).each{|column_k|
    (1..(2 * column_k)).each{|i|
      shift = i * (i - 1) / 2 + column_k
      next if shift > max_n

      coeffs = bounded[2 * column_k - i]
      sign = i.odd? ? 1 : -1
      (0..(max_n - shift)).each{|n|
        ary[n + shift] += sign * coeffs[n]
      }
    }
  }
  ary
end

def ordinary_row_sum_series(max_n)
  partitions = partition_series(max_n)
  strict = strict_row_sum_series(max_n)
  ary = Array.new(max_n + 1, 0)
  (0..max_n).each{|n|
    (0..n).each{|j|
      ary[n] += strict[j] * partitions[n - j]
    }
  }
  ary
end

max_n = 1000
ary = ordinary_row_sum_series(max_n)

(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
