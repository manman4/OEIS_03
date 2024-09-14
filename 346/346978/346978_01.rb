def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(k, n)
  a = [1]
  (1..n).each{|i|
    a << (1..i).inject(0){|s, j| s + (f(i) / f(i - j)) * ((k + 1).to_r / j - k.to_r / i) * a[-j]}.to_i
  }
  a
end

n = 500
ary = A(1, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
