def s(n)
  s = 0
  (1..n).each{|i| s += i if n % i == 0}
  s
end

def A(m, n)
  ary = [1]
  a = [0] + (1..n).map{|i| s(i)}
  (1..n).each{|i| ary << (1..i).inject(0){|s, j| s - m * a[j] * ary[-j]} / i}
  ary
end

def B(k, n)
  a = A(k, n)
  (0..n).select{|i| a[i] == 0}
end

m = 50000
n = 10000
a = B(26, m)
(1..n).each{|i|
  print i
  print ' '
  puts a[i - 1]
}
