require 'prime'

def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n)
  m = f(n) ** 2 + 1
  cnt = 1
  while (m + cnt).prime_division.size > 1
#p [m + cnt, (m + cnt).prime_division]
    cnt += 1
  end
#p [n, cnt, m + cnt, (m + cnt).prime_division]
  cnt
end

n = 10
p ary = (1..n).map{|i| A(i)}
p (1..n).map{|i| ary[i - 1] - i}
# p (1..n).map{|i| ary[i - 1] - i + 1}
# (1..n).each{|i|
#   j = (i ** i + f(i)) / i
#   break if j.to_s.size > 1000
#   print i
#   print ' '
#   puts j
# }
