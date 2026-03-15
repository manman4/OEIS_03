def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i).map{|rest| [i, *rest]}}
end
# Number of partitions of n such that the multiplicity m of the least part s is a part of the partition and m > s
def A(n)
  cnt = 0
  partition(n, 1, n).each{|ary|
    s = ary.min
    m = ary.count(s)
    if m > s && ary.include?(m)
      # p ary.join
      cnt += 1
    end
  }
  cnt
end
def A394291(n)
  [0] + (1..n).map{|i| A(i)}
end
n = 30
ary = A394291(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
