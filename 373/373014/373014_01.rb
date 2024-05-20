def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i).map{|rest| [i, *rest]}}
end

def A(n, k, m)
  cnt = 0
  partition(n, 1, n).each{|i|
    cnt += 1 if i[0] % k == m
  }
  cnt
end

# p (1..30).map{|i| A(i, 2, 0)}
# p (1..30).map{|i| A(i, 2, 1)}

# p (1..60).map{|i| A(i, 3, 0)}
p [0] + (1..50).map{|i| A(i, 3, 1)}
# p (1..60).map{|i| A(i, 3, 2)}