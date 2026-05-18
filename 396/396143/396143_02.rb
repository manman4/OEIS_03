# Triangle read by rows where T(n, k) is the number of partitions of
# n + 2*k^2 into 2*k distinct parts not containing the part 2*k,
# except when n = k = 0, in which case T(0, 0) = 1.

def column_sequence(max_n, column_k)
  return [1] + [0] * max_n if column_k == 0

  parts = 2 * column_k
  max_total = max_n + 2 * column_k * column_k
  dp = Array.new(parts + 1){Array.new(max_total + 1, 0)}
  dp[0][0] = 1

  (1..max_total).each{|part|
    next if part == parts

    upper = [parts, part].min
    upper.downto(1){|used|
      part.upto(max_total).reverse_each{|sum|
        dp[used][sum] += dp[used - 1][sum - part]
      }
    }
  }

  (0..max_n).map{|n| dp[parts][n + 2 * column_k * column_k]}
end

def build_table(max_n)
  columns = (0..max_n).map{|k| column_sequence(max_n, k)}
  (0..max_n).map{|n|
    (0..n).map{|k| columns[k][n]}
  }
end

p ary = build_table(20).flatten
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
