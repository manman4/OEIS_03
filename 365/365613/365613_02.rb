# number of partitions p of n such that m * (the greatest multiplicity of the parts of p) is a part of p.

def pentagonal_pairs(limit)
  list = []
  k = 1
  while true
    g1 = k * (3 * k - 1) / 2
    g2 = k * (3 * k + 1) / 2
    break if g1 > limit && g2 > limit
    list << [k, g1] if g1 <= limit
    list << [k, g2] if g2 <= limit
    k += 1
  end
  list
end

def partition_numbers(n, pentas)
  p = Array.new(n + 1, 0)
  p[0] = 1
  i = 1
  while i <= n
    sum = 0
    pentas.each{|k, g|
      v = i - g
      break if v < 0
      s = (k.odd? ? 1 : -1)
      sum += s * p[v]
    }
    p[i] = sum
    i += 1
  end
  p
end

# dp_max_le[s] = partitions of s with multiplicity <= k
# computed as p(s) * product_{j>=1} (1 - x^{(k+1)j})
def max_le_all(n, k, p, pentas)
  dp = Array.new(n + 1, 0)
  i = 0
  while i <= n
    sum = p[i]
    pentas.each{|kk, g|
      t = (k + 1) * g
      v = i - t
      break if v < 0
      s = (kk.odd? ? -1 : 1)
      sum += s * p[v]
    }
    dp[i] = sum
    i += 1
  end
  dp
end

# Given dp_max_le for multiplicity <= k, remove target_part to get dp_without_target,
# then dp_with_target = dp_max_le - dp_without_target.
def with_target_from_max_le(dp_max_le, n, k, target_part)
  dp_no = Array.new(n + 1, 0)
  t = target_part
  r = 0
  while r < t
    window = 0
    j = 0
    while (sum = r + j * t) <= n
      val = dp_max_le[sum]
      dp_no[sum] = val - window
      window += dp_no[sum]
      if j >= k
        window -= dp_no[r + (j - k) * t]
      end
      j += 1
    end
    r += 1
  end
  dp_with = Array.new(n + 1, 0)
  i = 0
  while i <= n
    dp_with[i] = dp_max_le[i] - dp_no[i]
    i += 1
  end
  dp_with
end

def A_sequence(n, m)
  max_k1 = Math.sqrt(n / m.to_f).floor
  max_k2 = n / (m + 1)
  max_k = (max_k1 > max_k2 ? max_k1 : max_k2)
  return Array.new(n + 1, 0) if max_k == 0

  pentas = pentagonal_pairs(n)
  p = partition_numbers(n, pentas)

  ans = Array.new(n + 1, 0)
  k = 1
  while k <= max_k
    target_part = m * k
    dp_k = max_le_all(n, k, p, pentas)
    with_k = with_target_from_max_le(dp_k, n, k, target_part)
    dp_km1 = max_le_all(n, k - 1, p, pentas)
    with_km1 = with_target_from_max_le(dp_km1, n, k - 1, target_part)
    i = 0
    while i <= n
      ans[i] += with_k[i] - with_km1[i]
      i += 1
    end
    k += 1
  end

  ans
end

n = 10000
ary = A_sequence(n, 1)
(0..n).each{|i| 
  print i
  print ' '
  puts ary[i]
}
