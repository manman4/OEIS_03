# Triangle read by rows where T(n, k) is the number of integer
# partitions of 2*n with reverse-alternating sum 2*k.

def build_table(max_n)
  max_sum = 2 * max_n
  dp = Array.new(max_sum + 1){Hash.new(0)}
  dp[0][0] = 1

  max_sum.downto(1){|part|
    next_dp = Array.new(max_sum + 1){Hash.new(0)}
    (0..max_sum).each{|sum|
      dp[sum].each{|alt_sum, count|
        copies = 0
        new_sum = sum
        while new_sum <= max_sum
          new_alt_sum = copies.even? ? alt_sum : part - alt_sum
          next_dp[new_sum][new_alt_sum] += count
          copies += 1
          new_sum += part
        end
      }
    }
    dp = next_dp
  }

  (0..max_n).map{|n|
    total = 2 * n
    (0..n).map{|k| dp[total][2 * k]}
  }
end

max_n = 50

ary = build_table(max_n)

# column_k = 0
# p seq = (0..max_n).map{|n| column_k <= n ? ary[n][column_k] : 0}

# column_k = 1
# p seq = (0..max_n).map{|n| column_k <= n ? ary[n][column_k] : 0}

# column_k = 2
# p seq = (0..max_n).map{|n| column_k <= n ? ary[n][column_k] : 0}

column_k = 3
p seq = (0..max_n).map{|n| column_k <= n ? ary[n][column_k] : 0}


# (0...seq.size).each{|i|
#   j = seq[i]
#   break if j.to_s.size > 1000
#   print i
#   print ' '
#   puts j
# }
