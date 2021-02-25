def A(k, n)
  ary = [0]
  (1..n).each{|i|
    s = 1
    (2..i).each{|j|
      s += j ** k * ary[i / j] if i % j == 0
    }
    ary << s
  }
  ary[1..-1]
end

n = 10000
ary = A(2, n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
