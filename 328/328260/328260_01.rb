require 'prime'

# Number of distinct primes dividing n
def A001221(n)
  n.prime_division.size
end

n = 10000
(1..n).each{|i|
  j = i * A001221(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}