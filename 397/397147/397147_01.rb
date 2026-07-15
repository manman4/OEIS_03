def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(1) = 1; a(n) = Sum_{k=1..n-1} a(k)*a(n-k) + Sum_{r=1..floor(n/3)} (-1)^(r+1) * binomial(n-2*r,r) * a(n-2*r).
def a(n)
  ary = [0, 1]
  (2..n).each{|m|
    sum = (1..m - 1).inject(0){|s, k| s + ary[k] * ary[m - k]}
    ary << (1..m / 3).inject(sum){|s, r| s + (-1) ** (r + 1) * ncr(m - 2 * r, r) * ary[m - 2 * r]}
  }
  ary
end

n = 40
p ary = a(n)
(1..n).each{|i| 
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
