#  a(n) = (n+1)*a(n-1) - (n-2)*a(n-2) - (n-5)*a(n-3) + (n-3)*a(n-4).
def A(n)
  a, b, c, d = 1, 1, 0, 0
  ary = [1]
  (4..n + 3).each{|i|
    a, b, c, d = b, c, d, (i + 1) * d - (i - 2) * c - (i - 5) * b + (i - 3) * a
    ary << a
  }
  ary
end

# a(n) = (n-3)*a(n-1) + 3*(n-1)*a(n-2) + (2*n-5)*a(n-3) - (n-3)*a(n-4) - (2*n-13)*a(n-5) - (n-8)*a(n-6) + (n-6)*a(n-7).
def B(n)
  a, b, c, d, e, f, g = 1, 1, 2, 4, 18, 92, 570
  ary = [1]
  (7..n + 6).each{|i|
    a, b, c, d, e, f, g = b, c, d, e, f, g, (i - 3) * g + 3 * (i - 1) * f + (2 * i - 5) * e - (i - 3) * d - (2 * i - 13) * c - (i - 8) * b + (i - 6) * a
    ary << a
  }
  ary
end

def A370485(n)
  a = A(n)
  b = B(n)
  (0..n).map{|i| b[i] - a[i]}
end

n = 500
ary = A370485(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}