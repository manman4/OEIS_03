def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(0) = 1; a(n) = Sum_{k=0..n-1} (-1)^(n-k-1) * binomial(n,k) * (k+2)^(2*(n-k)) * a(k) 
def A(n)
  ary = [1]
  (1..n).each{|i| ary << (0..i-1).inject(0){|s, j| s + (-1)**(i-j-1) * ncr(i, j) * (j+2)**(2*(i-j)) * ary[j]}}
  ary
end

n = 400
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}