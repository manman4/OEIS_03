# number of partitions p of n such that m * (the least multiplicity of the parts of p) is a part of p.

def partitions_all_and_excluding(n, skip_part)
  dp_all = Array.new(n + 1, 0)
  dp_no = Array.new(n + 1, 0)
  dp_all[0] = 1
  dp_no[0] = 1
  part = 1
  while part <= n
    sum = part
    while sum <= n
      dp_all[sum] += dp_all[sum - part]
      if part != skip_part
        dp_no[sum] += dp_no[sum - part]
      end
      sum += 1
    end
    part += 1
  end
  [dp_all, dp_no]
end

# Return dp[0..n] for partitions where:
# - Every present part has multiplicity >= min_k (or 0 if absent)
# - target_part is present with multiplicity >= target_min_mult
def count_min_ge_with_target_all(n, min_k, target_part, target_min_mult)
  return Array.new(n + 1, 0) if target_part > n
  max_part = n / min_k
  return Array.new(n + 1, 0) if target_part > max_part

  dp = Array.new(n + 1, 0)
  dp[0] = 1

  part = 1
  while part <= max_part
    new_dp = Array.new(n + 1, 0)
    is_target = (part == target_part)
    min_t = is_target ? target_min_mult : min_k

    r = 0
    while r < part
      prefix = []
      j = 0
      while (sum = r + j * part) <= n
        prefix[j] = (j == 0 ? dp[sum] : prefix[j - 1] + dp[sum])
        j += 1
      end

      j = 0
      while (sum = r + j * part) <= n
        if j >= min_t
          add = prefix[j - min_t]
          if is_target
            new_dp[sum] = add
          else
            new_dp[sum] = dp[sum] + add
          end
        else
          new_dp[sum] = is_target ? 0 : dp[sum]
        end
        j += 1
      end
      r += 1
    end

    dp = new_dp
    part += 1
  end

  dp
end

def A_sequence(n, m)
  max_k = Math.sqrt(n / m.to_f).floor
  return Array.new(n + 1, 0) if max_k == 0

  ans = Array.new(n + 1, 0)

  # k = 1
  dp_all, dp_no = partitions_all_and_excluding(n, m)
  present = Array.new(n + 1, 0)
  i = 0
  while i <= n
    present[i] = dp_all[i] - dp_no[i]
    i += 1
  end
  b = count_min_ge_with_target_all(n, 2, m, 2)
  i = 0
  while i <= n
    ans[i] += present[i] - b[i]
    i += 1
  end

  k = 2
  while k <= max_k
    target_part = m * k
    a = count_min_ge_with_target_all(n, k, target_part, k)
    b = count_min_ge_with_target_all(n, k + 1, target_part, k + 1)
    i = 0
    while i <= n
      ans[i] += a[i] - b[i]
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


