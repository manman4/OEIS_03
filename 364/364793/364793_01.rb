# 和因子はmin以上max以下
def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i).map{|rest| [i, *rest]}}
end

def A(k, n)
  (0..n).map{|i| partition(i, 1, i).select{|j| j.uniq.size <= k}.size}
end

p A(4, 52)