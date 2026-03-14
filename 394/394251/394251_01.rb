def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i).map{|rest| [i, *rest]}}
end
# Number of partitions p of n such that (maximal multiplicity of the parts of p) <= k * (number of distinct parts of p).
def A(n, k)
  cnt = 0
  partition(n, 1, n).each{|ary|
    if ary.group_by{|x| x}.values.map(&:size).max <= k * ary.group_by{|x| x}.size
      # p ary
      cnt += 1
    end
  }
  cnt
end
def A_array(n, k)
  [1] + (1..n).map{|i| A(i, k)}
end
n = 40
ary = A_array(n, 3)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

