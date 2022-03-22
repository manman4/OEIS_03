def I(ary, n)
  a = [1]
  i = 0
  while i < n
    a << -(0..i).inject(0){|s, j| s + ary[1 + i - j] * a[j]}
    i += 1
  end
  a
end

def A(n)
  ary = [1] + [0] * n
  (1..n).each{|i|
    j = i * i * i
    break if j > n
    ary[j] = 1
  }
  I(ary, n)
end

m = 11000
n = 10000
ary = A(m)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
