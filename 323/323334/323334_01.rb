def B(k, n)
  ary = []
  (1..n).each{|i|
    (i..n).each{|j|
      (j..n).each{|k|
        ary << i * j * k
      }
    }
  }
  ary.uniq!
  ary.inject(:+)
end

n = 500
(1..n).each{|i|
  j = B(3, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
