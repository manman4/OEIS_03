def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i).map{|rest| [i, *rest]}}
end
# number of partitions p of n such that m * (the least multiplicity of the parts of p) is a part of p.
def A(n, m)
  cnt = 0
  partition(n, 1, n).each{|ary|
    min_multiplicity = ary.tally.values.min
    if ary.include?(m * min_multiplicity)
      # p ary.join
      cnt += 1
    end
  }
  cnt
end
def A394460(n)
  [0] + (1..n).map{|i| A(i, 2)}
end
n = 50
p ary = A394460(n)
# (1..n).each{|i|
#   j = ary[i]
#   break if j.to_s.size > 1000
#   print i
#   print ' '
#   puts j
# }

# p (1..n).map{|i| A(i, 1)}
# p (1..n).map{|i| A(i, 2)}
# p (1..n).map{|i| A(i, 3)}
# p (1..n).map{|i| A(i, 4)}
