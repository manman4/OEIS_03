def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(k, n)
  (0..n).inject(0){|s, i| s + (-1) ** (n-i) * f(n + (k - 1) * i) / (f(n - i) * f(i) ** k)}.to_i
end

def B(n)
  ary = []
  (0..n).each{|i|
    (0..i).each{|j|
      ary << A(i - j, j)
    }
  }
  ary
end

n = 10
p ary = B(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

# (0..5).each{|i| p [i, (0..10).map{|j| A(i, j)}]}
# (0..5).each{|i| p (0..10).map{|j| A(j, i)}}
# p (0..10).map{|i| A(i, i)}
