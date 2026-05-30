# A(n,k) = Sum_{0 = x_0 <= x_1 <= ... <= x_{k-1} <= x_k = n} Product_{j=0..k-1} (x_j + 1) * (4*x_{j+1} - 3*x_j + 1)^(x_{j+1} - x_j - 1) * binomial(x_{j+1}, x_j).

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def weight(x, n)
  (x + 1r) * ncr(n, x) * (4 * n - 3 * x + 1) ** (n - x - 1)
end

def a(n, k, memo = Hash.new{|h, key| h[key] = {}})
  return n.zero? ? 1 : 0 if k.zero?
  return memo[k][n] if memo[k].key?(n)
  sum = 0
  0.upto(n){|x|
    sum += a(x, k - 1, memo) * weight(x, n)
  }
  memo[k][n] = sum.to_i
end

n = 9
(0..n).each{|i|
  (0..i).each{|j|
    print a(j, i - j)
    print ', '
  }
}