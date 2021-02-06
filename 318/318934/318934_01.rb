def A(n)
  ary = [3]
  i = 1
  while ary.size < n
    j = i.to_s(2)
    ary << i if j[0] == "1" && j[1] == "1" && j[2] == "0"
    i += 1
  end
  ary
end

n = 10000
ary = A(n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
