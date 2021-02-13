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

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  a = bernoulli(n + k + 1)
  ary = [1]
  (1..n).each{|i|
    ary << (-1) ** (k % 2) / i.to_r * (0..i - 1).inject(0){|s, j| s + a[i - j + k + 1] * ary[j] / (ncr(i - j + k, k) * (i - j + k + 1r))}
  }
  ary
end

n = 300
ary = A(2, n).map{|i| i.denominator}
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
