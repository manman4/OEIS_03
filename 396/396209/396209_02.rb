def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = Sum_{i=1..n} Sum_{j=1..i} binomial(j+1,2) * binomial(n+2*i-1,3*i-1) * binomial(i+2*j-1,3*j-1).
def a(n)
  (1..n).inject(0){|s, i|
    s + (1..i).inject(0){|t, j|
      t + ncr(j + 1, 2) * ncr(n + 2 * i - 1, 3 * i - 1) * ncr(i + 2 * j - 1, 3 * j - 1)
    }
  }
end

n = 100
(0..n).each{|i|
  j = a(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}