require 'prime'

def A(n, prime)
  n.prime_division.inject(1){|s, (p, e)| s * (p == prime ? p ** e : (p - 1) * p ** (e - 1))}
end

p (1..100).map{|i| A(i, 7)}

