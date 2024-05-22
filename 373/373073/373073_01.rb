# 和因子はmin以上max以下
def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i).map{|rest| [i, *rest]}}
end

def A(n, k)
  return 1 if n == 0
  cnt = 0
  partition(n, 1, n).each{|ary|
    m = ary.size
    cnt += 1 if ary[-1] > k * m
  }
  cnt
end 

# p (0..50).map{|i| A(i, 1)}
p (0..50).map{|i| A(i, 2)}
# p (1..55).map{|i| A(i, 3)}
# p (1..70).map{|i| A(i, 4)}
# p (1..75).map{|i| A(i, 5)}

