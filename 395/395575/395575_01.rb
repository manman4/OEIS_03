def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(0) = 1; a(n) = Sum_{k=1..floor((2*n+1)/3)} (-1)^(k-1) * (n-k+1)^k * binomial(2*n-2*k+1,k) * a(n-k).
def a(n)
  ary = [1]
  (1..n).each{|i|
    ary << (1..(2 * i + 1) / 3).inject(0){|s, k| s + (-1) ** (k - 1) * (i - k + 1) ** k * ncr(2 * i - 2 * k + 1, k) * ary[i - k]}
  }
  ary
end

n = 500
ary = a(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
