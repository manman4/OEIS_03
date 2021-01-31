def pl(n)
  n.to_s.reverse.to_i == n
end

ary = []
(1..10 ** 7).each{|i| 
  j = i * i
  if !pl(i)
    ary << j
  end
  break if ary.size >= 10000
}
(1..10000).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}
