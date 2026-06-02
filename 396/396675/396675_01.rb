def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def each_tuple(remaining, len, current, &block)
  if len == 0
    block.call(current.dup)
  else
    (0..remaining).each{|xi|
      current << xi
      each_tuple(remaining - xi, len - 1, current, &block)
      current.pop
    }
  end
end

# A(n,k,m) = (n-1)! * Sum_{x_1, x_2, ..., x_{k+1} >= 0 and x_1 + x_2 + ... + x_{k+1} = n-1} m^(x_{k+1}) * Product_{i=1..k} ((n - Sum_{j=1..i-1} x_j)^(x_i) / x_i!).
def A(n, k, m)
  total = Rational(0)

  each_tuple(n - 1, k, []){|xs|
    x_last = (n - 1) - xs.sum
    sign = m**x_last

    s = 0
    prod = Rational(1)
    xs.each{|xi|
      base = n - s
      prod *= Rational(base**xi, f(xi))
      s += xi
    }

    total += sign * prod
  }

  (f(n - 1) * total).to_i
end

# A(n,k) = (n-1)! * Sum_{x_1, x_2, ..., x_{k+1} >= 0 and x_1 + x_2 + ... + x_{k+1} = n-1} (-1)^(x_{k+1}) * Product_{i=1..k} ((n - Sum_{j=1..i-1} x_j)^(x_i) / x_i!).
def A396676(n, k)
  A(n, k, -1)
end

n = 10
p (1..n).map{|i| (1..i).map{|j| A396676(j, i - j)}}.flatten
