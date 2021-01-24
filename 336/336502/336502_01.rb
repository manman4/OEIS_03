def f(n)
  return 1 if n == 1
  (n * (n - 1) / 2 + 1..(n + 1) * n / 2).inject(:*)
end


n = 500
s = 0
(1..n).each{|i|
  s += f(i)
  break if s.to_s.size > 1000
  print i
  print ' '
  puts s
}
