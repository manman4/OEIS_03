# a(0) = a(1) = a(2) = 1; a(n) = a(n-1) + (n-2)*a(n-3).
def A(n)
  a = [1, 1, 1]
  (3..n).each{|i|
    a << a[-1] + (i - 2) * a[-3]
  }
  a
end

n = 1000
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}