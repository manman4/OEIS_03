def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def C(k, n)
  ary = []
  s = 0
  (1..n).each{|i|
    s += ncr(k * i, k)
    ary << s
  }
  ary
end

def A322967(n)
  a = []
  (1..n).each{|i| a << C(i, n - i + 1)}
  ary = []
  (0..n - 1).each{|i|
    (0..i).each{|j|
      ary << a[i - j][j]
    }
  }
  ary
end

n = 140
ary = A322967(n)
(1..ary.size).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
