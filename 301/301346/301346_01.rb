require 'prime'

def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n)
  ary = [0, 1, 2, 3, 4, 5, 9, 10, 11, 13, 24, 65, 76]
  return f(n) ** 2 + 1 if ary.include?(n)
  (f(n) ** 2 + 1).prime_division[-1][0]
end

n = 20
(0..n).each{|i| p A(i)}
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
