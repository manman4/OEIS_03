def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n, k)
  m = f(n)
  (0..n / 3).inject(0){|s, i| s + m * (k / 6r) ** i * (i + 1) ** (n - 2 * i - 1) / (f(i) * f(n - 3 * i))}.to_i
end

def B(n)
  ary = []
  (0..n).each{|i|
    (0..i).each{|j|
      ary << A(j, i - j)
    }
  }
  ary
end

n = 139
ary = B(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
