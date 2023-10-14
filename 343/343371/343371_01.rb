def A(n)
  ary = [1]
  (1..n).each{|i|
    s = 1
    (1..i - 1).each{|j|
      s += ary[j - 1] if i % j == 0
    }
    ary << s
  }
  ary
end

n = 10000
ary = A(n)
(0..n).each{|i|
  print i
  print ' '
  puts ary[i]
}