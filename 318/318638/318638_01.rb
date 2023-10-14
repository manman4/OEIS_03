def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n)
  s = 0
  (1..n).each{|i|
    s += 3 ** (i - n / i) * ncr(i, n / i) if (n % i == 0) && (i >= n / i)
  }
  s
end

n = 1024
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}