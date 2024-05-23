def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i - 1).map{|rest| [i, *rest]}}
end

def A(n, k)
  return 1 if n == 0
  cnt = 0
  partition(n, 1, n).each{|ary|
    cnt += 1 if ary.size % k == 0
  }
  cnt
end 

# p (1..50).map{|i| A(i, 1)}
# p (1..50).map{|i| A(i, 2)}
# p (0..63).map{|i| A(i, 3)}
# p (0..65).map{|i| A(i, 4)}
p (0..65).map{|i| A(i, 5)}


