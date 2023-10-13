require 'prime'

n = 10000
primes = Prime.take(n + 1)
(1..n).each{|i|
  print i
  print ' '
  puts (i * primes[i]) % primes[i - 1]
}