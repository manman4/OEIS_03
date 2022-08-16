def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(k, n)
  s = 0
  (1..n).each{|i|
    s += (-1) ** (n / i) * i ** k if n % i == 0
  }
  -s
end

def B(k, n)
  ary = [0] + (1..n).map{|i| A(k, i)}
  a = [0]
  (1..n).each{|i|
    m = f(i)
    a << (1..i).inject(0){|s, j| s + ary[j] * m / j}
  }
  a
end

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def C(k, n)
  ary = B(k, n)
  a = [1]
  (1..n).each{|i|
    a << (1..i).inject(0){|s, j| s + ary[j] * ncr(i - 1, j - 1) * a[-j]}
  }
  a
end

n = 500
ary = C(2, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}