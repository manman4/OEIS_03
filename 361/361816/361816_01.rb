def A(n)
  ary = [1, 2, 0, -10]
  (4..n).each{|i|
    ary << 2 * ( (2*i-1)*ary[-1] - 3*(2*i-2)*ary[-2] + 3*(2*i-3)*ary[-3] - (2*i-4)*ary[-4] ) / i
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