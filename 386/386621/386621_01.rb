def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# A(n,k) = Sum_{j=0..floor(n/2)} k^(n-2*j) * binomial(2*(n-j),n-j) * binomial(n-j,j)
def A(n, k)
  (0..n / 2).inject(0){|sum, j| sum + k**(n - 2 * j) * ncr(2 * (n - j), n - j) * ncr(n - j, j)}
end

def B(n)
  (0..n).map{|i| (0..i).map{|j| A(j, i - j)}}.flatten
end

n = 139
ary = B(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print " "
  puts j
}