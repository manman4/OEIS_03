def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n, k)
  m = f(n)
  (0..n / 2).inject(0){|s, i| s + m * (-k / 2r) ** i / (f(i) * f(n - 2 * i))}.to_i
end

n = 139
ary = []
(0..n).each{|i|
  (0..i).each{|j|
    ary << A(j, i - j)
  }
}
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
