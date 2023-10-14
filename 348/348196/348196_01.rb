require 'prime'

def A(k, n)
  s = 0
  ((n - 1) ** 2..n ** 2 - 1).each{|i|
    if i.prime?
      s += 1 if i % 4 == k
    end
  }
  s
end

n = 10000
s = 0
(1..n).each{|i| 
  s += A(1, i)
  print i
  print ' '
  puts s
}
