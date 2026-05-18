# Triangle read by rows where U(n, k) is the number of strict integer
# partitions of 2*n with reverse-alternating sum -2*k.

def build_table(max_n)
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
    (0..n).map{|k| dp[total][offset - 2 * k]}
  }
end

p ary = build_table(20)
p ary = build_table(20).flatten
(0...ary.size).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
