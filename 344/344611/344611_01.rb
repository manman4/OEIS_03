# Row sums of A344610:
# sum_k T(n, k), where T(n, k) is the number of integer partitions of 2*n
# with reverse-alternating sum 2*k.

def partition_numbers(max_n)
  ary = Array.new(max_n + 1, 0)
  ary[0] = 1
  (1..max_n).each{|part|
    (part..max_n).each{|sum|
      ary[sum] += ary[sum - part]
    }
  }
  ary
end

def strict_row_sums(max_n)
  max_sum = 2 * max_n
  offset = max_sum
  width = 2 * max_sum + 1
  dp = Array.new(max_sum + 1){Array.new(width, 0)}
  dp[0][offset] = 1

  max_sum.downto(1){|part|
    next_dp = dp.map(&:dup)
    (0..(max_sum - part)).each{|sum|
      row = dp[sum]
      (-sum..sum).each{|alt_sum|
        count = row[alt_sum + offset]
        next if count == 0
        next_dp[sum + part][part - alt_sum + offset] += count
      }
    }
    dp = next_dp
  }

  (0..max_n).map{|n|
    total = 2 * n
    row = dp[total]
    sum = 0
    (0..total).step(2){|alt_sum|
      sum += row[alt_sum + offset]
    }
    sum
  }
end

def row_sums(max_n)
  partitions = partition_numbers(max_n)
  strict = strict_row_sums(max_n)
  (0..max_n).map{|n|
    (0..n).sum{|j| strict[j] * partitions[n - j]}
  }
end

max_n = 1000
ary = row_sums(max_n)

(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
