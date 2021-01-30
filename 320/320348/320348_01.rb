def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i - 1).map{|rest| [i, *rest]}}
end

def f(n)
  a = []
  partition(n, 1, n).each{|ary|
    ary << 0
    s = ary.size
    ary1 = ary
    ary2 = (1..s - 1).map{|i| ary1[i - 1] - ary1[i]}
#p [ary, ary2]
    t = ary2.size
    ary2.uniq!
    a << ary if ary2.size == t
  }
#p ''
  a
end

(1..100).each{|i| 
  print i
  print ' '
  puts f(i).size
}
