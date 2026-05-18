# Sequence for fixed k where a(n) is the number of strict integer
# partitions of 2*n with reverse-alternating sum -2*k.
# Equivalently, for k > 0, a(n) is the number of partitions of
# n + 2*k^2 into 2*k distinct parts not containing the part 2*k.

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

max_n = 60
column_k = 4
p ary = column_sequence(max_n, column_k)

(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
