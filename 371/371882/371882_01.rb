def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def H(n)
  s = 0
  a = [0]
  (1..n).each{|i|
    s += 1r / i
    a << s
  }
  a
end

def A(n, k)
  h = H(n)
  ary = [1]
  (1..n).each{|i|
    ary <<(0..i).inject(0){|s, j| s + ncr(i, j) ** k * (1 - 7 * j * (h[j] - h[i - j]))}.to_i
  }
  ary
end

n = 650
m = 600
ary = A(n, 7)
(0..m).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}