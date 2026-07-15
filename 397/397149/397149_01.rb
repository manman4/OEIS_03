def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(1) = 1; a(n) = Sum_{i, j, k, l>=1 and i+j+k+l=n} a(i)*a(j)*a(k)*a(l) + Sum_{r=1..floor(n/3)} (-1)^(r+1) * binomial(n-2*r,r) * a(n-2*r).
def a(n)
  ary = [0, 1]
  

