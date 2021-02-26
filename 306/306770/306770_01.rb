require 'bigdecimal'

def f(n)
  return 1 if n == 0
  (1..n).inject(:*)
end

def g(n)
  f(n - 1) + f(n) + f(n + 1)
end

def h(n)
  (1..n).inject(0){|s, i| s += 1r / g(i)}
end

n = 100
str = BigDecimal(h(n), n + 10).to_s("F")
ary = str.split('').map(&:to_i)
p ary = ary[2..-1]
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
