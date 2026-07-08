# a(n) = Sum_{k=1..n} gcd(k^3,n^2).
# Multiplicative with
# a(p^e) = p^e * ( 1 + ((p-1)/p) * Sum_{k=1..2*e} p^floor(2*k/3) ).
#
# More generally, let s and t be positive integers, and let
# a_{s,t}(n) = Sum_{k=1..n} gcd(k^s,n^t).
# a_{s,t}(p^e) = p^min(s*e,t*e) + Sum_{k=0..e-1}
#   (p^(e-k) - p^(e-k-1)) * p^min(s*k,t*e).
# a_{s,t}(n) is multiplicative if and only if t <= s.
# For 1 <= t <= s, a_{s,t}(p^e) = p^e * ( 1 + ((p-1)/p)
#   * Sum_{k=1..t*e} p^floor(((s-1)*k)/s) ).

def factorize(n)
  factors = []
  m = n
  p = 2

  while p * p <= m
    e = 0
    while (m % p).zero?
      m /= p
      e += 1
    end
    factors << [p, e] if e > 0
    p = (p == 2 ? 3 : p + 2)
  end

  factors << [m, 1] if m > 1
  factors
end

def prime_power_value_floor(p, e, s, t)
  sum = 0
  1.upto(t * e){|k|
    sum += p**(((s - 1) * k) / s)
  }
  p**e + (p - 1) * p**(e - 1) * sum
end

def a_number(n, s, t)
  return 0 if n <= 0
  raise ArgumentError, "multiplicativity requires t <= s" if t > s

  factorize(n).inject(1){|prod, (p, e)|
    prod * prime_power_value_floor(p, e, s, t)
  }
end

def sequence(limit, s, t)
  (0..limit).map{|n| a_number(n, s, t)}
end

if __FILE__ == $PROGRAM_NAME
  limit = (ARGV[0] || 10000).to_i
  s = (ARGV[1] || 4).to_i
  t = (ARGV[2] || 2).to_i

  ary = sequence(limit, s, t)
  (1..limit).each{|n|
    j = ary[n]
    break if j.to_s.size > 1000
    print n
    print ' '
    puts j
  }
end
