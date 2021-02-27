require 'prime'

def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n)
  m = f(n) - n
  cnt = 0
  while !(m + cnt).prime?
    cnt -= 1
  end
#p [n, cnt, m + cnt]
  -cnt
end

n = 15
p ary = (3..n).map{|i| A(i) - 1}
p ary = (3..n).map{|i| A(i)}
# p ary.map{|i| i + 1}
# p (3..n).map{|i| A(i) + i}
# p (3..n).map{|i| A(i) + i + 1}
# (1..n).each{|i|
#   j = (i ** i + f(i)) / i
#   break if j.to_s.size > 1000
#   print i
#   print ' '
#   puts j
# }
