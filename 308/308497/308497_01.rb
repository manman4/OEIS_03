def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end
def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end
def A(k, n)
  m = f(k)
  ary = [0]
  (1..n).each{|i| ary << (f(i + k - 1) - (1..i - 1).inject(0){|s, j| s + ncr(i - 1, j) * f(j + k - 1) * ary[-j]}) / m}
  ary
end

def A308497(n)
  a = []
  (0..n).each{|i| a << A(i, n - i + 1)}
  ary = []
  (1..n).each{|i|
    (1..i).each{|j|
      ary << a[i - j][j]
    }
  }
  ary
end

n = 140
ary = A308497(n)
(1..ary.size).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
