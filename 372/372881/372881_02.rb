require 'prime'

def A(n)
  s = 0
  (1..n).each{|i|
    (1..n).each{|j|
      s += n.gcd(i * j)
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
