require 'prime'

# Multiplicative with a(13^e) = 13^e, and a(p^e) = (p-1)*p^(e-1) if p != 13.
def A(n)
  n.prime_division.inject(1){|s, (p, e)| s * (p == 13 ? p ** e : (p - 1) * p ** (e - 1))}
end

p (1..76).map{|i| A(i)}
