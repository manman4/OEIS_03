def count_partitions_with_constraint(n)
  return 1 if n == 0
  counts = count_partitions_with_constraint_up_to(n)
  counts[n]
end

def count_partitions_with_constraint_up_to(n_max)
  counts = Array.new(n_max + 1, 0)
  counts[0] = 0

  k = 1
  loop do
    min_sum = k * k * (k + 1) / 2
    break if min_sum > n_max

    dp = count_exact_k_distinct_with_min_mult_up_to(n_max, k)
    (0..n_max).each{|s|
      counts[s] += dp[s]
    }
    k += 1
  end

  counts
end

def count_exact_k_distinct_with_min_mult_up_to(n, k)
  # dp[j][s]: using processed sizes, select exactly j distinct sizes,
  # sum to s, where each selected size has multiplicity >= k.
  dp = Array.new(k + 1) { Array.new(n + 1, 0) }
  dp[0][0] = 1

  (1..n).each{|size|
    dp2 = dp.map(&:dup)

    (0...k).each{|j|
      # temp[s] = ways to pick current size with multiplicity >= k
      temp = Array.new(n + 1, 0)

      base = k * size
      (base..n).each{|s|
        temp[s] = dp[j][s - base]
      }
      (base + size..n).each{|s|
        temp[s] += temp[s - size]
      }

      (0..n).each{|s|
        dp2[j + 1][s] += temp[s]
      }
    }

    dp = dp2
  }

  dp[k]
end

n = 1000
counts = count_partitions_with_constraint_up_to(n)
counts.each_with_index{|val, n|
  if n > 0
    print n
    print " " 
    puts val
  end
}
