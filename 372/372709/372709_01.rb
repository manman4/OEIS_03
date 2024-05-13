require 'prime'

def tau(n)
  n.prime_division.inject(1){|s, i| s * (i[1] + 1)}
end

# a(n) = Sum_{k=1..n} tau(n*k)
def A(n)
  (1..n).inject(0){|s, i| s + tau(n * i)}
end

n = 10000
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}