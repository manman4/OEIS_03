require 'prime'

def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

p [0, 3, 8, 13, 19, 423, 585].map{|i| f(2*i) + f(i) + 1}
