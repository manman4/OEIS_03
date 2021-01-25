def A325912(n)
  a = [2]
  (1..n).each{|i| a << -a[-1] + 2 ** 2 ** i}
  a
end

n = 20
ary = A325912(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
