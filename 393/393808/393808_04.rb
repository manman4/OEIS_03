# Verify the prime-power formulas for
# a_{s,t}(n) = Sum_{k=1..n} gcd(k^s,n^t).
#
# Checked formulas:
# a_{s,t}(p^e) = p^min(s*e,t*e) + Sum_{k=0..e-1}
#   (p^(e-k) - p^(e-k-1)) * p^min(s*k,t*e).
# For t <= s,
# a_{s,t}(p^e) = p^e * ( 1 + ((p-1)/p)
#   * Sum_{k=1..t*e} p^floor(((s-1)*k)/s) ).

def primes_upto(limit)
  return [] if limit < 2

  sieve = Array.new(limit + 1, true)
  sieve[0] = sieve[1] = false
  p = 2
  while p * p <= limit
    if sieve[p]
      step = p * p
      while step <= limit
        sieve[step] = false
        step += p
      end
    end
    p += 1
  end
  (2..limit).select{|n| sieve[n]}
end

def direct_prime_power_value(p, e, s, t)
  n = p**e
  nt = n**t
  total = 0
  1.upto(n){|k|
    total += (k**s).gcd(nt)
  }
  total
end

def prime_power_value_padic(p, e, s, t)
  total = p**([s * e, t * e].min)
  0.upto(e - 1){|k|
    total += (p**(e - k) - p**(e - k - 1)) * p**([s * k, t * e].min)
  }
  total
end

def prime_power_value_floor(p, e, s, t)
  sum = 0
  1.upto(t * e){|k|
    sum += p**(((s - 1) * k) / s)
  }
  p**e + (p - 1) * p**(e - 1) * sum
end

def verify!(prime_limit, e_max, s_max, t_max)
  primes_upto(prime_limit).each{|p|
    1.upto(e_max){|e|
      1.upto(s_max){|s|
        1.upto(t_max){|t|
          direct = direct_prime_power_value(p, e, s, t)
          padic = prime_power_value_padic(p, e, s, t)
          if direct != padic
            raise "padic mismatch: p=#{p}, e=#{e}, s=#{s}, t=#{t}, direct=#{direct}, formula=#{padic}"
          end

          next if t > s

          floor_value = prime_power_value_floor(p, e, s, t)
          if direct != floor_value
            raise "floor mismatch: p=#{p}, e=#{e}, s=#{s}, t=#{t}, direct=#{direct}, formula=#{floor_value}"
          end
        }
      }
    }
  }
end

if __FILE__ == $PROGRAM_NAME
  prime_limit = (ARGV[0] || 7).to_i
  e_max = (ARGV[1] || 4).to_i
  s_max = (ARGV[2] || 5).to_i
  t_max = (ARGV[3] || 5).to_i

  verify!(prime_limit, e_max, s_max, t_max)
  puts "ok"
end
