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

n = 10100
m = 10000
ary = A([[2, -2], [1, 1], [3, 1]], n)
(0..m).each{|i|
  j = ary[i].abs
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
