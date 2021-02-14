def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def I(ary, n)
  a = [1]
  i = 0
  while i < n
    a << -(0..i).inject(0){|s, j| s += ary[1 + i - j] * a[j]}
    i += 1
  end
  a
end

def A(k, n)
  I((0..n).map{|i| f(i) ** k}, n)
end

n = 250
ary = A(3, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
