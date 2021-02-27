def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i - 1).map{|rest| [i, *rest]}}
end
def f(n)
  a = []
  partition(n, 2, n).each{|ary|
    sum = ary.inject(0){|s, i| s + 1 / i.to_r}
    a << ary.reverse if sum.denominator == 1 && sum.numerator > 1
  }
  a
end
n = 1
a = []
while a == []
  n += 1
  a = f(n)
end
p a
