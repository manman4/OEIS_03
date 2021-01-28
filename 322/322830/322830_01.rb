def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end
def A(k, n)
  (0..n).map{|i| (0..k).inject(0){|s, j| s + ncr(2 * k, 2 * j) * (i + 1) ** (k - j) * i ** j}}
end

p A(3, 20)
