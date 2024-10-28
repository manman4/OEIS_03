def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(0) = 1; a(n) = Sum_{k=1..n} (4 * k/n - 1) * (k-1)! * binomial(n,k) * a(n-k).
def A(n)
  ary = [1]
  (1..n).each{|i| ary << (1..i).inject(0){|s, j| s + (4r * j / i - 1) * f(j - 1) * ncr(i, j) * ary[i - j]}.to_i}
  ary
end

n = 500
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
