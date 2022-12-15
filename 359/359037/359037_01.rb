require 'prime'

def d(n)
  sum = 1
  pq = n.prime_division
  pq.each{|a| sum *= (1 + a[1])}
  sum
end

def A(n)
  d(n) * d(n ** 3)
end

n = 10000
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}