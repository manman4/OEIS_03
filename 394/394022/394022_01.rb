def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i).map{|rest| [i, *rest]}}
end
# Number of partitions of n with multiplicity of each part at most k, satisfying max(p) <= m * min(p).
def A(n, m, k)
  cnt = 0
  partition(n, 1, n).each{|ary|
    # max(ary) <= m * min(ary)
    if ary.max <= m * ary.min
      # multiplicity of each part at most k
      if ary.group_by{|x| x}.values.all?{|v| v.size <= k}
        # p ary
        cnt += 1
      end
    end
  }
  cnt
end
def A394022(n)
  (1..n).map{|i| A(i, 2, 3)}
end
p A394022(40)

