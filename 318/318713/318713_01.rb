def bernoulli(n)
  ary = []
  a = []
  (0..n).each{|i|
    a << 1r / (i + 1)
    i.downto(1){|j| a[j - 1] = j * (a[j - 1] - a[j])}
    ary << a[0] # Bn = a[0]
  }
  ary
end

def A(n)
  a = bernoulli(2 * n + 4)
  ary = [1]
  (1..n).each{|i|
    ary << -3r / i * (0..i - 1).inject(0){|s, j| s += a[2 * i - 2 * j + 4] * ary[j] / ((2 * i - 2 * j + 1r) * (2 * i - 2 * j + 2) * (2 * i - 2 * j + 3) * (2 * i - 2 * j + 4))}
  }
  ary
end

n = 200
ary = A(n).map{|i| i.numerator}
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
