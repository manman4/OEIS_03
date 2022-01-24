def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i).map{|rest| [i, *rest]}}
end

def A(n)
  a = Array.new(n, 0)
  partition(n, 1, n).each{|ary|
    (1..n).each{|i|
      a[i - 1] += 1 if i * ary[-1] == ary.size
    }
  }
  a
end

def A350889(n)
  (1..n).map{|i| A(i)}.flatten
end

p A350889(14)
