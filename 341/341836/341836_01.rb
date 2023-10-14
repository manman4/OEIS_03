require 'prime'

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  n.prime_division.inject(1){|s, i| s * (-1) ** (i[1] % 2) * ncr(k, i[1])}
end

n = 10000
(1..n).each{|i|
  j = A(10, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
