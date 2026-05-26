def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# A(n,k) = Sum_{0 = x_0 <= x_1 <= ... <= x_{k-1} <= x_k = n} Product_{j=0..k-1} (2*x_j + 1) * binomial(3*x_{j+1} - x_j + 1,x_{j+1} - x_j)/(3*x_{j+1} - x_j + 1).
def a(n, k)
  return n == 0 ? 1 : 0 if k == 0
  sum = 0
  (0..n).each{|x|
    sum += a(x, k-1) * (2*x + 1) * ncr(3*n - x + 1, n - x) / (3*n - x + 1)
  }
  sum.to_i
end

n = 10
(0..n).each{|i|
  (0..i).each{|j|
    print a(j, i - j)
    print ', '
  }
}
