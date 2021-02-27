def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end
def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end
def S(x, y, n)
  (0..n / 2).inject(0){|s, i| s + x ** (n - 2 * i) * y ** i * ncr(n - i, i) * f(n - i) / f(i)}
end
def A305460(n)
  (0..n).map{|i| S(3, 2, i)}
end
p A305460(25)

n = 15
(0..5).each{|m|
  p [m, (0..n).map{|i| S(m, 2, i)}]
}
