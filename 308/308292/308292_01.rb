def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(k, n)
  return 1 if k == 0
  s = 0
  (0..n).to_a.repeated_permutation(k){|c|
    s += f(c.inject(:+)) / (1..k).inject(1){|s, i| s * f(c[i - 1])}
  }
  s
end

(0..4).each{|i|
p [i, (0..5).map{|j| A(j, i)}]
}
(0..4).each{|i|
p [i, (0..5).map{|j| A(i, j)}]
}

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
(1..ary.size).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
