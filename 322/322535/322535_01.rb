n = 10000
(0..n).each{|i|
  j = (2 * i + 1) * (9664*i**6+28992*i**5+37360*i**4+26400*i**3+10936*i**2+2568*i+315)/315
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
