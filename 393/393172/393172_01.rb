def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# aa(1) = 1; a(n) = Sum_{k=1..n-1} a(k)*a(n-k) + Sum_{r=1..floor(n/2)} (-1)^(r+1) * binomial(n-r,r) * a(n-r).
def a(n)
  ary = [0, 1]
  (2..n).each{|m|
    sum = (1..m - 1).inject(0){|s, k| s + ary[k] * ary[m - k]}
    ary << (1..m / 2).inject(sum){|s, r| s + (-1) ** (r + 1) * ncr(m - r, r) * ary[m - r]}
  }
  ary
end

n = 520
ary = a(n)
(1..n).each{|i| 
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
