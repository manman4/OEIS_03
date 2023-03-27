def A(n)
  ary = [1, 3, 12]
  (3..n).each{|i|
    ary << 3 * ( (3*i-2)*ary[-1] - 2*(3*i-4)*ary[-2] + (3*i-6)*ary[-3] ) / i
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