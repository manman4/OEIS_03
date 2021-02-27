require 'prime'

def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

n = 16
ary = Array.new(10 ** 4 + 1, 0)
a = (1..n).map{|i| f(i)}
(1..n).each{|i|
  a.combination(i){|c|
    m = c.inject(:+)
    ary[m] = i if m <= 10 ** 4
  }
}

(1..10 ** 4).each{|i|
  print i
  print ' '
  puts ary[i]
}
