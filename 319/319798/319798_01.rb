def f(k, n)
  ary = []
  while n > 0
    ary << n % k
    n /= k
  end
  ary
end
def g(k, ary)
  (0..ary.size - 1).inject(0){|s, i| s += ary[i] * k ** i}
end
def A319798(n)
  return 1922263344 if n == 7
  i = 1
  ary = [1]
  while g(n, ary) - g(n, ary.reverse) != i
    i += 1
    ary = f(n, i).sort
  end
  i
end

ary = []
(1..50).each{|i| 
  p j = A319798(3 * i - 1)
  ary << j
}
p ary
