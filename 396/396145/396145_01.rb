# Sequence for fixed k where U(n, k) is the number of strict integer
# partitions of 2*n with reverse-alternating sum -2*k.

def column_sequence(max_n, column_k)
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

  target = -2 * column_k
  (0..max_n).map{|n| dp[2 * n][target + offset]}
end

max_n = 50

column_k = 1
p ary = column_sequence(max_n, column_k)

column_k = 2
p ary = column_sequence(max_n, column_k)

column_k = 3
p ary = column_sequence(max_n, column_k)

column_k = 4
p ary = column_sequence(max_n, column_k)

p (0..15).map{|n| (0..n).map{|k| column_sequence(n, k)[-1]}}

(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
