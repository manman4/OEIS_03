def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

# a(n) = n^2 * (3*n^4-6*n^3+7*n^2-4*n+1) * a(n-1) - n^2 * (n-1)^8 * (3*n^2-6*n+5) * a(n-2) + n^2 * (n-1)^8 * (n-2)^8 * a(n-3) + ((n-1)!)^6 for n > 2.
def a(n)
  ary = [0, 1, 85]
  (3..n).each{|i|
    ary << i**2 * (3*i**4-6*i**3+7*i**2-4*i+1) * ary[-1] - i**2 * (i-1)**8 * (3*i**2-6*i+5) * ary[-2] + i**2 * (i-1)**8 * (i-2)**8 * ary[-3] + f(i-1)**6
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
