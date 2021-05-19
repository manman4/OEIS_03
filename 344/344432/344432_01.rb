require 'prime'

def A008683(n)
  ary = n.prime_division
  return (-1) ** (ary.size % 2) if ary.all?{|i| i[1] == 1}
  0
end

n = 1000
s = 0
print 0
print ' '
puts s
(1..n).each{|i|
  s += s + A008683(i)
  break if s.to_s.size > 1000
  print i
  print ' '
  puts s
}
