def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# T(n,k) = Sum_{j=0..k} (-1)^(k-j) * binomial(k,j) * (3+j)^n.
def T(n, k)
  (0..k).inject(0){|sum, j| sum + (-1)**(k - j) * ncr(k, j) * (3 + j)**n}
end

def A(n)
  (0..n).map{|i| (0..i).map{|j| T(i, j)}}.flatten
end

n = 139
ary = A(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}