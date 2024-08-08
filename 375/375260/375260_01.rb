# a(n) = ((n+3)/n^2) * ((2*n-1)*a(n-1) + 3*(n+2)*a(n-2)).
def A(n)
  ary = [1, 4]
  (2..n).each{|i|
    ary << ((i + 3r) / i ** 2) * ((2 * i - 1) * ary[-1] + 3 * (i + 2) * ary[-2])
  }
  ary
end

n = 1000
ary = A(n)
(0..n).each{|i|
  j = ary[i].to_i
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
