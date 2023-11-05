# a(0) = a(1) = 1; a(n) = 2 * Sum_{k=0..n-2} a(k) * a(n-k-2).
def A(n)
  ary = [1, 1]
  (2..n).each{|i|
    ary << (0..i - 2).inject(0){|s, j| s + 2 * ary[j] * ary[i - j - 2]}
  }
  ary
end

n = 1100
m = 1000
ary = A(n)
(0..m).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}