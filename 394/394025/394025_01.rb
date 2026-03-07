def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i - 1).map{|rest| [i, *rest]}}
end
def A(n, m)
  cnt = 0
  partition(n, 1, n).each{|ary|
    # max(ary) = m * min(ary) + 1
    if ary.max == m * ary.min + 1
      cnt += 1
    end
  }
  cnt
end
def A394025(n)
  (1..n).map{|i| A(i, 3)}
end
p A394025(50)
