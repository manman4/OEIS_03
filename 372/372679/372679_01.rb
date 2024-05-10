require 'prime'

# Multiplicative with a(p^e) = p^e if p = 3 or 5, and (p-1)*p^(e-1) otherwise.
def A(n)
  n.prime_division.inject(1){|s, (p, e)| s * (p == 3 || p == 5 ? p ** e : (p - 1) * p ** (e - 1))}
end

p (1..78).map{|i| A(i)}
