# Triangle read by rows where T(n, k) is the number of partitions of
# n+2*k^2 into 2*k distinct parts containing the part 2*k,
# except when n = k = 0, in which case T(0, 0) = 1.

@memo = {}

def count_ways(total, parts, min_part, forbidden)
  return total == 0 ? 1 : 0 if parts == 0
  return 0 if total <= 0

  key = [total, parts, min_part, forbidden]
  return @memo[key] if @memo.key?(key)

  min_sum = parts * min_part + parts * (parts - 1) / 2
  return @memo[key] = 0 if min_sum > total

  count = 0
  min_part.upto(total) do |part|
    next if part == forbidden
    count += count_ways(total - part, parts - 1, part + 1, forbidden)
  end
  @memo[key] = count
end

def t(n, k)
  return 1 if n == 0 && k == 0
  return 0 if k == 0

  fixed = 2 * k
  remaining = n + 2 * k * k - fixed

  return 0 if remaining < 0

  count_ways(remaining, 2 * k - 1, 1, fixed)
end

p (0..50).map{|n| t(n, 2)}
