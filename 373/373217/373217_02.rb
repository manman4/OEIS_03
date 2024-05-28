require 'prime'

def A(n, prime)
  return 1 if n == 1
  n.prime_division.inject(1){|s, (p, e)| p == prime ? s * (e + 1) : s}
end

n = 10000
(1..n).each{|i|
  j = A(i, 7)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

