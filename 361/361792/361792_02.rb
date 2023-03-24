def A(n)
  ary = [1, 2, -6, -10, 66, 60, -750]
  (7..n).each{|i|
    ary << -( (3*i-5)*ary[-1] + (17*i-24)*ary[-2] + 35*(i-3)*ary[-3] + 35*(i-4)*ary[-4] + 21*(i-5)*ary[-5] + 7*(i-6)*ary[-6] + (i-7)*ary[-7] ) / i
  }
  ary
end

n = 1000
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}