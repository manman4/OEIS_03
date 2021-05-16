require 'prime'

def tau(n)
  n.prime_division.inject(1){|s, i| s * (i[1] + 1)}
end

n = 10000
(1..n).each{|i|
  j = tau(i ** 6)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
