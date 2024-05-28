require 'prime'

def A(n, prime)
  return 1 if n == 1
  n.prime_division.inject(1){|s, (p, e)| s * (p == prime ? e + 1 : 1)}
end

n = 10000
(1..n).each{|i|
  j = A(i, 7)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

