require 'prime'

# Multiplicative with a(17^e) = 17^e, and a(p^e) = (p-1)*p^(e-1) if p != 17.
def A(n)
  n.prime_division.inject(1){|s, (p, e)| s * (p == 17 ? p ** e : (p - 1) * p ** (e - 1))}
end

p (1..78).map{|i| A(i)}
