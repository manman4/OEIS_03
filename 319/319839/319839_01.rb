def f(k, n)
  ary = []
  while n > 0
    ary << n % k
    n /= k
  end
  ary
end
def g(k, ary)
  (0..ary.size - 1).inject(0){|s, i| s + ary[i] * k ** i}
end
def A319798(n)
  i = 1
  ary = [1]
  while g(n, ary) - g(n, ary.reverse) != i
    i += 1
    ary = f(n, i).sort
  end
  i
end
def A319839(n)
  (1..n).map{|i| A319798(2 * i)}
end
p A319839(20)
