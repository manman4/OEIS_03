# 和因子はmin以上max以下
def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i).map{|rest| [i, *rest]}}
end

def A(k, n)
  return 1 if n == 0
  cnt = 0
  partition(n, 1, n).each{|i|
    cnt += 1 if i[-1] % k == 0
  }
  cnt
end

n = 50
b = []
(0..n).each{|i|
  j = A(5, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
  b << j
}
p b