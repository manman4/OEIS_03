def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def C(k, n)
  s = 0
  (1..n).each{|i|
    s += ncr(n * i, n)
  }
  s * f(n)
end

n = 500
(1..n).each{|i|
  j = C(i, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
