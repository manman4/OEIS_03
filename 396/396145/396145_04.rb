# a(n) = number of partitions of n + 32 into 8 distinct parts
# not containing the part 8.

def sequence(max_n)
  parts = 8
  forbidden_part = 8
  shift = 32
  max_total = max_n + shift
  dp = Array.new(parts + 1){Array.new(max_total + 1, 0)}
  dp[0][0] = 1

  (1..max_total).each{|part|
    next if part == forbidden_part

    upper = [parts, part].min
    upper.downto(1){|used|
      max_total.downto(part){|sum|
        dp[used][sum] += dp[used - 1][sum - part]
      }
    }
  }

  (0..max_n).map{|n| dp[parts][n + shift]}
end

max_n = 10000
ary = sequence(max_n)

(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
