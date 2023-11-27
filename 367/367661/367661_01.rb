def A(k, n)
  ary = [1]
  (1..n).each{|i|
    ary << (0..(i - 1) / k).inject(ary[-1]){|s, j| s + ary[j] * ary[i - 1 - k * j]}
  }
  ary
end

n = 1000
ary = A(5, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}