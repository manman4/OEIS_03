def A(n)
  a = [0]
  (1..n).each{|i|
    s = 0
    (1..i).each{|j|
      s += 1 if i % j == 0
    }
    a << s
  }
  ary = [0]
  (1..n).each{|i|
    s = 0
    (1..i).each{|j|
      s += a[j] if i % j == 0
    }
  ary << s
  }
  ary
end

n = 10000
ary = A(n)
s = 0
(1..n).each{|i|
  s += ary[i] ** 2
  break if s.to_s.size > 1000
  print i
  print ' '
  puts s
}
