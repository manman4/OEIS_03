def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# A(n,k) = Sum_{j=0..k} binomial(k,j) * binomial(n+(k-1)*j-1,n-j).
def A(n, k)
  return 1 if n == 0
  return 0 ** n if k == 0
  (0..[n, k].min).inject(0){|s, j| s + ncr(k, j) * ncr(n + (k - 1) * j - 1, n - j)}
end

n = 139
ary = []
(0..n).each{|i|
  (0..i).each{|j|
    ary << A(j, i - j)
  }
}
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
