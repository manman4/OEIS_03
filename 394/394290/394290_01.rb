def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i).map{|rest| [i, *rest]}}
end
# Number of partitions of n such that the multiplicity m of the greatest part g satisfies m < g and m is also a part.
def A(n)
  cnt = 0
  partition(n, 1, n).each{|ary|
    g = ary.max
    m = ary.count(g)
    if m < g && ary.include?(m)
      # p ary.join
      cnt += 1
    end
  }
  cnt
end
def A394290(n)
  [0] + (1..n).map{|i| A(i)}
end
n = 30
ary = A394290(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
