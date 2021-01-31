def A(k, n)
(0..n).map{|i| 
  i.to_s(k).split('').sort.join.to_i(k) + i.to_s(k).split('').sort.reverse.join.to_i(k)
}
end

n = 4095
ary = A(4, n)
(0..n).each{|i|
  print i
  print ' '
  puts ary[i]
}
