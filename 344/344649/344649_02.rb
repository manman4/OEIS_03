# Triangle read by rows where T(n, k) is the number of strict
# integer partitions of 2*n with reverse-alternating sum 2*k.

def build_table(max_n)
  max_sum = 2 * max_n
  dp = Array.new(max_sum + 1){Hash.new(0)}
  dp[0][0] = 1

  max_sum.downto(1){|part|
    next_dp = dp.map(&:dup)
    (0..(max_sum - part)).each{|sum|
      dp[sum].each{|alt_sum, count|
        next_dp[sum + part][part - alt_sum] += count
      }
    }
    dp = next_dp
  }

  (0..max_n).map{|n|
    total = 2 * n
    (0..n).map{|k| dp[total][2 * k]}
  }
end

M=139
ary = build_table(M).flatten
(0...ary.size).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
