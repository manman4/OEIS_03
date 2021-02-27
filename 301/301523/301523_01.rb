require 'prime'

def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

n = 500
ary = [0]
(1..n).each{|i|
  (1..i).each{|j|
    ary << f(i + 1) + f(j)
  }
}

(1..10000).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
