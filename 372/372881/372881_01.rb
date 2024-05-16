require 'prime'

def A(n)
  s = 0
  (1..n).each{|i|
    (1..n).each{|j|
      s += n.gcd(i * j)
    }
  }
  s
end

# multiplicative function
def B(n)
  n.prime_division.inject(1){|s, (p, e)| s * A(p ** e)}
end

p (1..1000).map{|i| A(i) - B(i)}
p (1..48).map{|i| A(i)}

