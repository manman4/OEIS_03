def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i).map{|rest| [i, *rest]}}
end
# Number of partitions of n into parts where each part divides the largest part, and each part occurs at most twice if it is the largest part, and at most once otherwise.
def A(n)
  cnt = 0
  partition(n, 1, n).each{|ary|
    m = ary.max
    if ary.all?{|x| m % x == 0} && ary.group_by{|x| x}.all?{|k, v| v.size <= (k == m ? 2 : 1)}
      # p ary
      cnt += 1
    end
  }
  cnt
end
def A389719(n)
  (1..n).map{|i| A(i)}
end
p A389719(55)

