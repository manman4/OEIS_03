def A(n)
  k = 13
  s = 0
  (1..n).each{|i|
    s += i if n % i == 0 && (i % k == 1 || i % k == 3 || i % k == 4 || i % k == 9 || i % k == 10 || i % k == 12)
  }
  s
end

def B(n)
  (1..n).map{|i| A(i)}
end

def C(n)
  ary = [1]
  a = [0] + B(n)
  (1..n).each{|i| ary << (1..i).inject(0){|s, j| s + a[j] * ary[-j]} / i}
  ary
end

n = 10100
m = 10000
ary = C(n)
(0..m).each{|i|
  print i
  print ' '
  puts ary[i]
}
