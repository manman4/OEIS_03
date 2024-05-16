require 'prime'

def A(n)
  (1..n).inject(0){|s, i| s + (n * n).gcd(i * i * i)}
end

# multiplicative function
def B(n)
  n.prime_division.inject(1){|s, (p, e)| s * A(p ** e)}
end

n = 10000
(1..n).each{|i|
  j = B(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

