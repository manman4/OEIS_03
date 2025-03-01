def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# A(n,0) = 0^n; A(n,k) = k * Sum_{j=0..n} binomial(2*n+j+k,j)/(2*n+j+k) * A(n-j,2*j).
def A(n, k)
  return 0 ** n if k == 0
  (k * (0..n).inject(0){|s, i| s + ncr(2*n + i + k, i) / (2*n + i + k).to_r * A(n - i, 2*i)}).to_i
end

def B(n)
  (0..n).map{|i| (0..i).map{|j| A(j, i - j)}}.flatten
end

n = 10
p B(n)

