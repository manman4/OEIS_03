# A(n,k) = Sum_{0 = x_0 <= x_1 <= ... <= x_{k-1} <= x_k = n} Product_{j=0..k-1} 2 * (x_j + 1) * (4*x_{j+1} - 2*x_j + 2)^(x_{j+1} - x_j - 1) * binomial(x_{j+1}, x_j).

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def weight(x, n)
  base = 4 * n - 2 * x + 2
  Rational(2 * (x + 1) * ncr(n, x), base) * base**(n - x)
end

def a(n, k, memo = Hash.new { |h, key| h[key] = {} })
  return n.zero? ? 1 : 0 if k.zero?
  return memo[k][n] if memo[k].key?(n)

  sum = 0
  0.upto(n) do |x|
    sum += a(x, k - 1, memo) * weight(x, n)
  end

  memo[k][n] = sum.to_i
end

n = 10
(0..n).each{|i|
  (0..i).each{|j|
    print a(j, i - j)
    print ', '
  }
}