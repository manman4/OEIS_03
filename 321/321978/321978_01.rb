def f(k, m)
  return 1 if k * m == 0
  s =(1..k * m).inject(:*)
  (1..k).each{|i|
    (1..m).each{|j|
      s /= i + j - 1
    }
  }
  s
end

n = 200
(0..n).each{|i|
  j = f(9, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
