require 'prime'

def A(n)
  s = 0
  (1..n).each{|i|
    (1..n).each{|j|
      s += n.gcd(i) * n.gcd(j) / n.gcd(i * j)
    }
  }
  s
end

# multiplicative function
def B(n)
  n.prime_division.inject(1){|s, (p, e)| s * A(p ** e)}
end

n = 1000
(1..n).each{|i|
  j = B(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

