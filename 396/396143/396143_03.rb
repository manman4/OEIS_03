# Triangle read by rows where T(n, k) is the number of strict integer
# partitions of 2*n with reverse-alternating sum -2*k.
#
# For k > 0, the column generating function is
#   q^k / Product_{j=1..2*k} (1-q^j)
#   - Sum_{i=1..2*k} (-1)^(i-1) * q^(i*(i-1)/2 + k)
#     / Product_{j=1..2*k-i} (1-q^j).
# The case k = 0 is exceptional: T(0, 0) = 1 and T(n, 0) = 0 for n > 0.

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

def column_sequence(max_n, column_k, bounded)
  return [1] + [0] * max_n if column_k == 0

  ary = Array.new(max_n + 1, 0)
  shift = column_k
  exact = bounded[2 * column_k]
  (0..(max_n - shift)).each{|n|
    ary[n + shift] += exact[n]
  }

  (1..(2 * column_k)).each{|i|
    shift = i * (i - 1) / 2 + column_k
    next if shift > max_n

    coeffs = bounded[2 * column_k - i]
    sign = i.odd? ? -1 : 1
    (0..(max_n - shift)).each{|n|
      ary[n + shift] += sign * coeffs[n]
    }
  }
  ary
end

def build_table(max_n)
  bounded = bounded_partition_series(2 * max_n, max_n)
  columns = (0..max_n).map{|k| column_sequence(max_n, k, bounded)}
  (0..max_n).map{|n|
    (0..n).map{|k| columns[k][n]}
  }
end

ary = build_table(139).flatten
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
