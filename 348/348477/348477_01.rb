require 'prime'

def A348477(n)
  ary = (2..n).map{|i| i.prime_division}.flatten
  ary.delete(1)
  [1] + ary
end

n = 10000
ary = A348477(n)
(1..n).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}
