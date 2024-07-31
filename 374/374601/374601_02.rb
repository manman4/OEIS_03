# a(n) = n^(n-1) - Sum_{i=1..n-1} n^(n-1-i)*i*a(i))
def A(n)
  ary = [0]
  (1..n).each{|i| i ** (i - 1) - (1..i - 1).inject(0){|s, j| s + i ** (i - 1 - j) * j * ary[j]} == 0 ? ary << 0 : ary << i ** (i - 1) - (1..i - 1).inject(0){|s, j| s + i ** (i - 1 - j) * j * ary[j]}}
  ary
end

n = 500
ary = A(n)
(1..n).each{|i|
  j = ary[i] 
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
