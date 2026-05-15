# Number of partitions of n into k distinct parts containing the part m.
def A(n, k, m)
  helper = lambda{|remaining, parts_left, min_val, found_m|
    if parts_left == 1
      return 0 if remaining < min_val
      return (found_m || remaining == m) ? 1 : 0
    end

    count = 0
    (min_val..((remaining - parts_left*(parts_left-1)/2) / parts_left)).each{|a|
      count += helper.call(remaining - a, parts_left - 1, a + 1, found_m || a == m)
    }
    count
  }

  return 0 if k == 0 || n < k*(k+1)/2
  helper.call(n, k, 1, false)
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

# p (0..20).map{|i| A(2*i, i, m)}

# p (1..20).map{|i| (1..i).map{|j| A(i, j, m)}}

# p ary = (1..20).map{|i| (1..i).map{|j| A(i, j, m)}}.flatten
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