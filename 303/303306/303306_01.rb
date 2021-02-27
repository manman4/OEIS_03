def s(n)
  s = 0
  (1..n).each{|i| s += i if n % i == 0}
  s
end
def A303306(n)
  ary = [1]
  a = (0..n).map{|i| 2 ** (i - 1) * (s(2 * i) - s(i))}
  (1..n).each{|i| ary << -(1..i).inject(0){|s, j| s + a[j] * ary[-j]} / i}
  ary
end
p A303306(100)
