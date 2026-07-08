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
  total = p**e
  1.upto(t * e){|i|
    total += (p - 1) * p**(e - 1 + ((s - 1) * i) / s)
  }
  total
end

def a_number(n, s = 3, t = 2)
  return 0 if n <= 0
  raise ArgumentError, "multiplicativity requires t <= s" if t > s

  factorize(n).inject(1){|prod, (p, e)|
    prod * prime_power_value_floor(p, e, s, t)
  }
end

def sequence(limit, s = 3, t = 2)
  (0..limit).map{|n| a_number(n, s, t)}
end

if __FILE__ == $PROGRAM_NAME
  limit = (ARGV[0] || 20).to_i
  s = (ARGV[1] || 3).to_i
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
