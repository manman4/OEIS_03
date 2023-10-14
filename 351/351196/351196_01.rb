require 'prime'

def A(k, n)
  s = 0
  (1..n).each{|i|
    s += i ** k if i.prime? && n % i == 0
  }
  s
end

n = 10000
(1..n).each{|i|
  print i
  print ' '
  puts A(8, i)
}