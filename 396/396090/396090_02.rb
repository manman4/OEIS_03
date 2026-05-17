# Triangle read by rows where T(n, k) is the number of integer
# partitions of 2*n with reverse-alternating sum 2*k.

def partition_numbers(max_n)
  dp = Array.new(max_n + 1, 0)
  dp[0] = 1
  (1..max_n).each{|part|
    (part..max_n).each{|sum|
      dp[sum] += dp[sum - part]
    }
  }
  dp
end

def strict_column(max_n, column_k)
  return [1] + [0] * max_n if column_k == 0

  max_total = max_n + 2 * column_k * column_k
  choose = 2 * column_k - 1
  target_offset = 2 * column_k * column_k - 2 * column_k
  dp = Array.new(choose + 1){Array.new(max_total + 1, 0)}
  dp[0][0] = 1

  (1..max_total).each{|part|
    next if part == 2 * column_k

    upper = [choose, part].min
    upper.downto(1){|used|
      (part..max_total).reverse_each{|sum|
        dp[used][sum] += dp[used - 1][sum - part]
      }
    }
  }

  (0..max_n).map{|n| dp[choose][n + target_offset]}
end

def column_sequence(max_n, column_k)
  return partition_numbers(max_n) if column_k == 0

  partitions = partition_numbers(max_n)
  strict = strict_column(max_n, column_k)
  (0..max_n).map{|n|
    (0..n).sum{|m| partitions[n - m] * strict[m]}
  }
end

max_n = 10000

column_k = 3
seq = column_sequence(max_n, column_k)

(0..seq.size - 1).each{|i|
  j = seq[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
