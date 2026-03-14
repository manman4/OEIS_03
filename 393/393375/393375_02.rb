def count_exact_d_distinct_with_min_mult_up_to(n, d, min_mult)
  # dp[j][s]: using processed sizes, select exactly j distinct sizes,
  # sum to s, where each selected size has multiplicity >= min_mult.
  dp = Array.new(d + 1) { Array.new(n + 1, 0) }
  dp[0][0] = 1

  (1..n).each do |size|
    dp2 = dp.map(&:dup)

    (0...d).each do |j|
      base = min_mult * size
      next if base > n

      # temp[s] = ways to select current size with multiplicity >= min_mult
      temp = Array.new(n + 1, 0)
      (base..n).each do |s|
        temp[s] = dp[j][s - base]
      end
      (base + size..n).each do |s|
        temp[s] += temp[s - size]
      end

      (0..n).each do |s|
        dp2[j + 1][s] += temp[s]
      end
    end

    dp = dp2
  end

  dp[d]
end

# Number of partitions of n such that k * (number of distinct parts) <= minimal multiplicity of the parts.
def A_array(n, k)
  counts = Array.new(n + 1, 0)
  counts[0] = 1

  d = 1
  loop do
    min_sum = k * d * d * (d + 1) / 2
    break if min_sum > n

    min_mult = k * d
    le = count_exact_d_distinct_with_min_mult_up_to(n, d, min_mult)

    (0..n).each do |s|
      counts[s] += le[s]
    end
    d += 1
  end

  counts
end

n = 1000
k = 3
ary = A_array(n, k)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
