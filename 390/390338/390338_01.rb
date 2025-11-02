def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = Sum_{k=0..n} binomial(4*n+k-1,n-k).
def A(n)
  ary = [1]
  (1..n).each{|i|
    ary << (0..i).inject(0){|s,k| s + ncr(4*i+k-1, i-k)}
  }
  ary
end

n = 1100
m = 1000
ary = A(n)
(0..m).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
