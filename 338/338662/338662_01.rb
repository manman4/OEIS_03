def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n)
  s = 0
  (1..n).each{|i|
    s += (n / i) ** i * ncr(i + n / i - 1, i) if n % i == 0
  }
  s
end

n = 5000
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}