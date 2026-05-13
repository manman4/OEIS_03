# Number of 1's in the partitions of n into k parts.
def A(n, k)
  # Returns [total_ones, partition_count] for partitions of `remaining`
  # into `parts_left` parts each >= min_val (non-decreasing order).
  # When a==1, each valid completion gains +1 ones from this part,
  # so we must multiply by sub_count rather than adding 1 once.
  helper = lambda{|remaining, parts_left, min_val|
    if parts_left == 1
      return [0, 0] if remaining < min_val
      return [remaining == 1 ? 1 : 0, 1]
    end

    total_ones = 0
    total_count = 0
    (min_val..(remaining / parts_left)).each{|a|
      sub_ones, sub_count = helper.call(remaining - a, parts_left - 1, a)
      total_ones += (a == 1 ? sub_count : 0) + sub_ones
      total_count += sub_count
    }
    [total_ones, total_count]
  }

  return 0 if k == 0 || n < k
  helper.call(n, k, 1)[0]
end

n = 62
p (0..n).map{|i| A(i, 0)}
p (0..n).map{|i| A(i, 1)}
p (0..n).map{|i| A(i, 2)}
p (0..n).map{|i| A(i, 3)}
p (0..n).map{|i| A(i, 4)}