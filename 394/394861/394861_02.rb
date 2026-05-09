def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

# a(n) = (n^3 + (n-1)^3 + (n-2)^3) * a(n-1) - ((n-1)^6 + (n-1)^3 * (n-2)^3 + (n-2)^6) * a(n-2) + (n-2)^9 * a(n-3) + ((n-3)!)^3 for n > 2.
def a(n)
  ary = [0, 0, 0]
  (3..n).each{|i|
    ary << (i**3 + (i-1)**3 + (i-2)**3) * ary[-1] - ((i-1)**6 + (i-1)**3 * (i-2)**3 + (i-2)**6) * ary[-2] + (i-2)**9 * ary[-3] + f(i-3)**3
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
