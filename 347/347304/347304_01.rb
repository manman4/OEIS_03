def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

n = 2000
(0..n).each{|i|
  j = f(i) / (f(i / 2) * f(i / 3) * f(i / 6))
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}