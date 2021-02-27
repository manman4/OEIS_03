require 'prime'

def A(n)
  return 0 if n < 7
  (1..n / 2).each{|i|
    return n - 2 * i if (i.prime?) && ((n - i).prime?) && (i < (n - i))
  }
end

ary = (1..10000).map{|i| A(2 * i)}
(1..ary.size).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
