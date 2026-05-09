def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

# a(n) = (n^4 + (n-1)^4) * a(n-1) - (n-1)^8 * a(n-2) + ((n-2)!)^4 for n > 1.
def a(n)
  ary = [0, 0]
  (2..n).each{|i|
    ary << (i**4 + (i-1)**4) * ary[-1] - (i-1)**8 * ary[-2] + f(i-2)**4
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
