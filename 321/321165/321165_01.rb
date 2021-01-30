def B(k, n)
  ary = []
  (1..n).each{|i|
    (i..n).each{|j|
      ary << i * j
    }
  }
  ary.uniq!
  ary.inject(:+)
end

def C(k, n)
  (1..n).map{|i| B(k, i)}
end

n = 1000
ary = C(2, n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
