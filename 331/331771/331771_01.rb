def A(n)
  s = 0
  (-n + 1).step(n - 1){|i|
    (-n + 1).step(n - 1){|j|
      s += (n - i.abs) * (n - j.abs) if i.gcd(j) == 1
    }
  }
  s
end

n = 1000
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}