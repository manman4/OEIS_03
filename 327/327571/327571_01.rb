def T(k, n)
  s = 0
  (0..n - 2).each{|i|
    s += k * n ** i
    return i if s % (i + 1) > 0
  }
  n - 1
end

def A327571(n)
  (2..n).map{|i| (1..i - 1).map{|j| T(j, i)}}.flatten
end

n = 10
ary = A327571(n)
(1..ary.size).each{|i|
  print i + 1
  print ' '
  puts ary[i - 1]
}

(2..30).each{|i|
  p (1..i - 1).map{|j| T(j, i)}
}
