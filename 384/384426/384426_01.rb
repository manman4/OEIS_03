def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i).map{|rest| [i, *rest]}}
end
# Number of partitions p of n with max(p) <= 2*min(p), where the multiplicity of min(p) is 1 or 2 and all other parts are distinct. 
def A(n)
  cnt = 0
  partition(n, 1, n).each{|ary|
    m = ary.min
    if ary.all?{|i| i <= 2 * m} && ary.count(m) <= 2 && ary.select{|i| i > m}.uniq.size == ary.count{|i| i > m}
      # p ary
      cnt += 1
    end
  }
  cnt
end
def A384426(n)
  [0] + (1..n).map{|i| A(i)}
end
p A384426(40)

