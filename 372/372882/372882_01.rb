require 'prime'

def A(n)
  (1..n).inject(0){|s, i| s + n.gcd(i * i * i)}
end

# multiplicative function
def B(n)
  n.prime_division.inject(1){|s, (p, e)| s * A(p ** e)}
end

p (1..10000).map{|i| A(i) - B(i)}
p (1..60).map{|i| A(i)}




