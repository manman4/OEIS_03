def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i).map{|rest| [i, *rest]}}
end
def A(n, k)
  cnt = 0
  partition(n, 1, n).each{|ary|
    cnt += 1 if k * ary.count(ary.min) == ary.size
  }
  cnt
end
def A386361(n)
  (1..n).map{|i| A(i, 4)}
end
p A386361(55)