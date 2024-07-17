def A(n, k)
  ary = []
  (0..n).each{|i|
    (i..n).each{|j|
      ary << i ** k + j ** k
    }
  }
  ary.uniq.size
end

n = 1000
(0..n).each{|i|
  j = A(i, 3)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
