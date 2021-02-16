def A(k, n)
  n.to_s(k).split('').map(&:to_i).inject(:*)
end

(0..10000).each{|i|
  j = A(8, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
