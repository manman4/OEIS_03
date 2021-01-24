require 'prime'

def mu(n)
  return 1 if n == 1
  return -1 if n.prime?
  ary = n.prime_division.map{|i| i[1]}
  return 0 if ary.any?{|i| i > 1}
  (-1) ** (ary.size % 2)
end

def mu_ary(n)
  (1..n).map{|i| mu(i)}
end

m = 11000
n = 10000
ary = [0] + mu_ary(m)
(1..n).each{|i|
  j = (1..i).inject(0){|s, j| s + ary[j] * j * j * j * j * j}
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

