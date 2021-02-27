def s(n, m)
  s = 0
  (1..[n, m].min).each{|i| s += i if n % i == 0}
  s
end

def A(n, m)
  ary = [1]
  a = [0] + (1..n).map{|i| s(i, m)}
  (1..n).each{|i| ary << (1..i).inject(0){|s, j| s + a[j] * ary[-j]} / i}
  ary
end

(0..30).each{|n|
  print n
  print ' '
  n0 = n ** 3 - n * n
  puts A(n0, n * n)[-1]
}
