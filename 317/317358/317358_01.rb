def A(n)
  a = (0..n).to_a
  k = 2
  while (a.inject(:+) - n) % k != 0
    k += 1
    (1..n).each{|i| a[i] *= i}
  end
  k
end

n = 1000
ary = [0] + (1..n).map{|i| A(i)}
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
