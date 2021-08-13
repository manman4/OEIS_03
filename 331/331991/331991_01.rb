def A(n)
  ary = [0, 1, 1]
  (3..n).each{|i|
    s = 0
    (1..i - 2).each{|j|
      s += ary[j] if (i - 1) % j == 0
    }
    ary << s
  }
  ary
end

n = 50
ary = A(n)
(1..n).each{|i|
  print i
  print ' '
  puts ary[i]
}
