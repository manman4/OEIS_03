require 'prime'

def A305762(n)
  return 24 if n == 0
  s = 1
  s *= 3 if n % 9 == 0
  n.prime_division.each{|i|
    s *= 2 ** [3, (i[1] - 1)].min if i[0] == 2
  }
  s
end

n = 10000
(0..n).each{|i|
  print i
  print ' '
  puts A305762(i)
}
