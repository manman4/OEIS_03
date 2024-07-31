# a(n) = n^n - Sum_{i=1..n-1} n^(n-i)*a(i).
def A(n)
  ary = [0]
  (1..n).each{|i| ary << i ** i - (1..i - 1).inject(0){|s, j| s + i ** (i - j) * ary[j]}}
  ary
end

n = 500
ary = A(n)
(1..n).each{|i|
  j = ary[i] / i
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
