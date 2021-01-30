def f(k, m)
  return 1 if k * m == 0
  s =(1..k * m).inject(:*)
  (1..k).each{|i|
    (1..m).each{|j|
      s /= i + j - 1
    }
  }
  s
end
def A(n)
  ary = []
  (0..n).each{|i|
    (0..i).each{|j|
      ary << f(i, j)
    }
  }
  ary
end
n = 30
ary = A(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
