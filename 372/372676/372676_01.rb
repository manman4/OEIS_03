require 'prime'

# Multiplicative with a(11^e) = 11^e, and a(p^e) = (p-1)*p^(e-1) if p != 11.
def A(n)
  n.prime_division.inject(1){|s, (p, e)| s * (p == 11 ? 11 ** e : (p - 1) * p ** (e - 1))}
end

p (1..76).map{|i| A(i)}
