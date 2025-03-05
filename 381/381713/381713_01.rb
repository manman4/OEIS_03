require 'prime'

# a(n) = n^6 * Product_{distinct primes p dividing n} (1 + 1/p^3 + 1/p^6).
def A(n)
  return 1 if n == 1
  n.prime_division.inject(n ** 6){|s, i| s * (1 + 1 / i[0] ** 3r + 1 / i[0] ** 6r)}.to_i
end

n = 10000
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
