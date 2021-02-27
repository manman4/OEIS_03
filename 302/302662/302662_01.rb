def A(n)
  a = 1 + n * 1i
  (2..n).each{|i|
    a *= i + (n - i + 1) * 1i
  }
  a
end

n = 400
(0..n).each{|i|
  j = A(i).imaginary
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
