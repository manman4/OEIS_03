def s(k, n)
  s = 0
  (1..n).each{|i| s += i if n % i == 0 && i % k == 0}
  s
end

def A(ary, n)
  a_ary = [1]
  a = [0] + (1..n).map{|i| ary.inject(0){|s, j| s + j[1] * s(j[0], i)}}
  (1..n).each{|i| a_ary << (1..i).inject(0){|s, j| s - a[j] * a_ary[-j]} / i}
  a_ary
end

def A331484(n)
  a = A([[1, 1]], n)
  ary = [1]
  (1..n).each{|i| ary << -(1..i).inject(0){|s, j| s + a[j - 1] * ary[-j]}}
  ary
end

m = 5050
n = 5000
ary = A331484(m)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
