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

def A(k, n)
  (0..n).map{|i| S(k, 1, i)}
end

def A305401(n)
  a = []
  (0..n).each{|i| a << A(i, n - i)}
  ary = []
  (0..n).each{|i|
    (0..i).each{|j|
      ary << a[i - j][j]
    }
  }
  ary
end

n = 10
ary = A305401(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
