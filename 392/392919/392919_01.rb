def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i).map{|rest| [i, *rest]}}
end
# Number of partitions p of n such that (maximal multiplicity of the parts of p) <= m * (minimal part of p).
def A(n, m)
  cnt = 0
  partition(n, 1, n).each{|ary|
    s = ary.min
    max_multiplicity = ary.map{|x| ary.count(x)}.max
    if max_multiplicity <= m * s
      # p ary.join
      cnt += 1
    end
  }
  cnt
end
def A392919(n)
  [0] + (1..n).map{|i| A(i, 1)}
end
n = 50
p ary = A392919(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
