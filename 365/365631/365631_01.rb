# 和因子はmin以上max以下
def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i).map{|rest| [i, *rest]}}
end

# k*(k+1)/2から
def A(k, n)
  (k * (k + 1) / 2..n).map{|i| partition(i, 1, i).select{|j| j.uniq.size == k}.size}
end

p A(5, 59)