require 'prime'

# a[0]=0
def phi(n)
  phi_ary = [0]
  (1..n).each{|i| phi_ary << i}
  (2..n).each{|p|
    if phi_ary[p] == p
      p.step(n, p){|d| phi_ary[d] -= phi_ary[d] / p}
    end
  }
  phi_ary
end

def power0(a, n)
  return 1 if n == 0
  k = power0(a, n >> 1)
  k *= k
  return k if n & 1 == 0
  return k * a
end

# x > 0
def sigma(x, i)
  sum = 1
  pq = i.prime_division
  pq.each{|a, n| sum *= (power0(a, (n + 1) * x) - 1) / (power0(a, x) - 1)}
  sum
end

# k > 0
def A(k, m, n)
  phi_ary = phi(n)
  ary = [0, 1]
  (2..n).each{|i|
    p_ary = i.prime_division.to_a
    # n = p^e
    if p_ary.size == 1
      s = 0
      (1..i).each{|j|
        s += phi_ary[j] * sigma(k, j ** m) if i % j == 0
      }
      ary << s
    else
      # multiplicative function
      ary << p_ary.inject(1){|s, (p, e)| s * ary[p ** e]}
    end
  }
  ary
end

n = 10000
ary = A(2, 2, n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}