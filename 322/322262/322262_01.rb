def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def I(ary, n)
  a = [1]
  i = 0
  while i < n
    a << -(0..i).inject(0){|s, j| s + ary[1 + i - j] * a[j]}
    i += 1
  end
  a
end

def A(k, n)
  a = (0..2 * k - 1).map{|i| (-1) ** (i % 2) * 1r / f(i)} + [0] * (n - 2 * k + 1)
  b = I(a, n)
  (0..n).map{|i| (b[i] * f(i)).to_i}
end

n = 800
ary = A(3, n)
(0..n).each{|i|
   j = ary[i]
   break if j.to_s.size > 1000
   print i
   print ' '
   puts j
}

