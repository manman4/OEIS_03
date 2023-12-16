def I(ary, n)
  a = [1]
  i = 0
  while i < n
    a << -(0..i).inject(0){|s, j| s + ary[1 + i - j] * a[j]}
    i += 1
  end
  a
end

def A(n, k)
  a = [1] + [0] * n
  (1..n).each{|i|
    j = i ** k
    break if j > n
    a[j] = 1
  }
  I(a, n)
end

n = 10100
m = 10000
ary = A(n, 5)
(0..m).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

