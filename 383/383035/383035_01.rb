def A(k, n)
  ary = [0, 1]
  (2..n).each{|i|
    s = k ** i - (k - 1) ** i
    (1..i - 1).each{|j|
      s += (-1) ** (i / j) * j * ary[j] if i % j == 0
    }
    ary << s / i
  }
  ary[1..-1]
end

n = 2000
ary = A(3, n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
