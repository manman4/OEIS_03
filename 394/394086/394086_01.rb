def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i).map{|rest| [i, *rest]}}
end
# Number of partitions of n where the smallest part divides all others, has multiplicity at most 2, and all larger parts are distinct.
def A(n)
  cnt = 0
  partition(n, 1, n).each{|ary|
    m = ary.min
    if ary.all?{|i| i % m == 0} && ary.count(m) <= 2 && ary.select{|i| i > m}.uniq.size == ary.count{|i| i > m}
      # p ary
      cnt += 1
    end
  }
  cnt
end
def A394086(n)
  (1..n).map{|i| A(i)}
end
p A394086(40)

