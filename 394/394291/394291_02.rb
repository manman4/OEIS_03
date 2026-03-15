def partition_counts_min_part(n)
  # part_at_least[s][t] = number of partitions of t using parts >= s
  part_at_least = Array.new(n + 2) { Array.new(n + 1, 0) }
  t = 0
  while t <= n
    part_at_least[n + 1][t] = (t == 0 ? 1 : 0)
    t += 1
  end

  s = n
  while s >= 1
    t = 0
    while t <= n
      part_at_least[s][t] = part_at_least[s + 1][t]
      t += 1
    end
    t = s
    while t <= n
      part_at_least[s][t] += part_at_least[s][t - s]
      t += 1
    end
    s -= 1
  end

  part_at_least
end

# Number of partitions of n such that the multiplicity m of the least part s
# is a part of the partition and m > s.
def A394291_array(n)
  counts = Array.new(n + 1, 0)
  part_at_least = partition_counts_min_part(n)

  s = 1
  while s <= n
    m = s + 1
    while m <= n
      base = m * (s + 1)
      break if base > n
      t = 0
      while base + t <= n
        counts[base + t] += part_at_least[s + 1][t]
        t += 1
      end
      m += 1
    end
    s += 1
  end

  counts
end

n = 1000
ary = A394291_array(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
