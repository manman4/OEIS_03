def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def bernoulli(n)
  ary = []
  a = []
  (0..n).each{|i|
    a << 1r / (i + 1)
    i.downto(1){|j| a[j - 1] = j * (a[j - 1] - a[j])}
    ary << a[0]
  }
  ary
end

def A330906(n)
  ary = bernoulli(4 * n + 4)
  (0..n).inject([]){|s, i|
    s << (0..2 * i + 2).inject(0){|t, j| t + (-1) ** j * (1r - 2 ** (2 * j - 1)) * (1r -2  ** (4 * i + 3 - 2 * j)) * ary[2 * j] * ary[4 * i + 4 - 2 * j] / (f(2 * j) * f(4 * i + 4 - 2 * j) + 0r)}.denominator
  }
end

m = 150
n = 100
ary = A330906(m)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
