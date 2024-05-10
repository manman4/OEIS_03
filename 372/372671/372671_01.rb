require 'prime'

# Multiplicative with a(p^e) = p^e if p = 2 or 3, and (p-1)*p^(e-1) otherwise.
def A(n)
  n.prime_division.inject(1){|s, (p, e)| s * (p == 2 || p == 3 ? p ** e : (p - 1) * p ** (e - 1))}
end

p (1..75).map{|i| A(i)}
