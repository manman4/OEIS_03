def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

# # a(n) = n^2 * (2*n^2-2*n+1) * a(n-1) - n^2 * (n-1)^6 * a(n-2) + ((n-1)!)^4 for n > 1.
def a(n)
  ary = [0, 1]
  (2..n).each{|i|
    ary << i**2 * (2*i**2-2*i+1) * ary[-1] - i**2 * (i-1)**6 * ary[-2] + f(i-1)**4
  }
  ary
end

n = 300
ary = a(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
