def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i).map{|rest| [i, *rest]}}
end

def A(n)
  return 1 if n == 0
  s = 0
  partition(n, 1, n).each{|ary|
    s += ary.uniq.inject(1){|s, i| s * i * i}
  }
  s
end

p (0..35).map{|i| A(i)}
