# a(0) = a(1) = a(2) = a(3) = 1; a(n) = a(n-1) + (n-3)*a(n-4).
def A(n)
  a = [1, 1, 1, 1]
  (4..n).each{|i|
    a << a[-1] + (i - 3) * a[-4]
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