def B(k, n)
  ary = []
  (1..n).to_a.repeated_combination(k){|c|
    ary << c.inject(:*)
  }
  ary.uniq!
  ary.inject(:+)
end

def C(k, n)
  (1..n).map{|i| B(k, i)}
end

def A321163(n)
  a = []
  (1..n).each{|i| a << C(i, n - i + 1)}
  ary = []
  (0..n - 1).each{|i|
    (0..i).each{|j|
      ary << a[i - j][j]
    }
  }
  ary
end

n = 25
ary = A321163(n)
(1..ary.size).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
