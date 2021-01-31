def pl(n)
  n.to_s.reverse.to_i == n
end

ary = []
(1..10 ** 7).each{|i| 
  j = i * i * i
  if !pl(j)
    ary << j
  end
  break if ary.size >= 1000
}
(1..1000).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}
