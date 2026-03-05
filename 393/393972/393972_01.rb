def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i).map{|rest| [i, *rest]}}
end
def A(n, k)
  cnt = 0
  partition(n, 1, n).each{|ary|
    # max(ary) < k * min(ary)
    cnt += 1 if ary.max < k * ary.min
  }
  cnt
end
def A393972(n)
  (1..n).map{|i| A(i, 3)}
end
p A393972(40)