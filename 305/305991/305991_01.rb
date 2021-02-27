def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

n = 20
ary = [1] + (1..n).map{|i| 3 ** i * (0..i - 1).inject(1){|s, j| s * (9 * j - 1)} / f(i)}
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
