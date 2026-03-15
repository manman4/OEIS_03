def partition_counts_up_to(n)
  # part[L][s] = number of partitions of s using parts <= L
  part = Array.new(n + 1) { Array.new(n + 1, 0) }
  part[0][0] = 1

  l = 1
  while l <= n
    prev = part[l - 1]
    cur = prev.dup
    s = l
    while s <= n
      cur[s] += cur[s - l]
      s += 1
    end
    part[l] = cur
    l += 1
  end

  part
end

# Number of partitions of n such that the multiplicity m of the greatest part g
# satisfies m < g and m is also a part.
def A394290_array(n)
  counts = Array.new(n + 1, 0)
  part = partition_counts_up_to(n)

  g = 2
  while g <= n
    max_m = g - 1
    m = 1
    while m <= max_m
      base = m * (g + 1)
      break if base > n
      t = 0
      while base + t <= n
        counts[base + t] += part[g - 1][t]
        t += 1
      end
      m += 1
    end
    g += 1
  end

  counts
end

n = 1000
ary = A394290_array(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
