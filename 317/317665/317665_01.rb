def I(ary, n)
  a = [1]
  i = 0
  while i < n
    a << -(0..i).inject(0){|s, j| s += ary[1 + i - j] * a[j]}
    i += 1
  end
  a
end

def A(n)
  ary = [1] + [0] * n
  (1..Math.sqrt(n).to_i).each{|i| ary[i * i] = 1}
  I(ary, n)
end

n = 11000
ary = A(n)
(0..10000).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
