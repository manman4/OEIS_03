def A(k, n)
  ary = [0]
  (1..n).each{|i|
    j = k ** (i - 1)
    j += ary[i / k] if i % k == 0
    ary << j
  }
  ary[1..-1]
end

n = 1100
m = 1000
ary = A(3, n)
(1..m).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}