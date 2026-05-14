# Number of m's in the partitions of n into exactly k parts.
def A(n, k, m)
  # Returns [total_ms, partition_count] for partitions of `remaining`
  # into `parts_left` parts each >= min_val (non-decreasing order).
  # When a==m, each valid completion gains +1 m's from this part,
  # so we must multiply by sub_count rather than adding 1 once.
  helper = lambda{|remaining, parts_left, min_val|
    if parts_left == 1
      return [0, 0] if remaining < min_val
      return [remaining == m ? 1 : 0, 1]
    end

    total_ms = 0
    total_count = 0
    (min_val..(remaining / parts_left)).each{|a|
      sub_ms, sub_count = helper.call(remaining - a, parts_left - 1, a)
      total_ms += (a == m ? sub_count : 0) + sub_ms
      total_count += sub_count
    }
    [total_ms, total_count]
  }

  return 0 if k == 0 || n < k
  helper.call(n, k, 1)[0]
end

m = 1
n = 62
# p (0..n).map{|i| A(i, 0, m)}
# p (0..n).map{|i| A(i, 1, m)}
# p (0..n).map{|i| A(i, 2, m)}
# p (0..n).map{|i| A(i, 3, m)}
# p (0..n).map{|i| A(i, 4, m)}
# p (0..n).map{|i| A(i, 5, m)}
# p (0..n).map{|i| A(i, 6, m)}

p (0..20).map{|i| A(2*i, i, m)}

# p (1..20).map{|i| (1..i).map{|j| A(i, j, m)}}

p ary = (1..20).map{|i| (1..i).map{|j| A(i, j, m)}}.flatten
# (1..ary.size).each{|i|
#   print i
#   print ' '
#   puts ary[i - 1]
# }

p (0..20).map{|i| (0..20).map{|j| A(i, j, m)}.sum}


puts("k=3")

p (0..n).map{|i| A(i, 3, 1)}
p (0..n).map{|i| A(i, 3, 2)}
p (0..n).map{|i| A(i, 3, 3)}

puts("k=4")

p (0..n).map{|i| A(i, 4, 1)}
p (0..n).map{|i| A(i, 4, 2)}
p (0..n).map{|i| A(i, 4, 3)}
p (0..n).map{|i| A(i, 4, 4)}

puts("k=5")

p (0..n).map{|i| A(i, 5, 1)}
p (0..n).map{|i| A(i, 5, 2)}
p (0..n).map{|i| A(i, 5, 3)}
p (0..n).map{|i| A(i, 5, 4)}
p (0..n).map{|i| A(i, 5, 5)}