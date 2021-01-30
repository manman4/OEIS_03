def A(n)
  ary = [0, 1]
  (2..n).each{|i|
    s = 0
    (1..i - 1).each{|j|
      if (i - 1) % j == 0
        s -= j * ary[j]
      end
    }
    ary << s
  }
  ary
end

n = 500
ary = A(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
