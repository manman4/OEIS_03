require 'prime'

def A(n)
  ary = [0, 1]
  (2..n).each{|i|
    p_ary = i.prime_division.to_a
    # n = p^e
    if p_ary.size == 1
      s = 0
      # a(n) = Sum_{1 <= x_1, x_2 <= n} gcd(x_1, n)/gcd(x_1, x_2, n).
      (1..i).each{|j|
        (1..i).each{|k|
          m = i.gcd(j)
          s += m / m.gcd(k)
        }
      }
      ary << s
    else
      # multiplicative function
      ary << p_ary.inject(1){|s, (p, e)| s * ary[p ** e]}
    end
  }
  ary
end

n = 1000
ary = A(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}