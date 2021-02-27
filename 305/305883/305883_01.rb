require 'prime'

def A(n)
  ary = []
  (1..n / 2).each{|i|
    ary << [i, n - i] if (i.prime?) && ((n - i).prime?) && (i < (n - i))
  }
  ary
end

ary = (4..374).map{|i| A(2 * i)}.flatten
(1..ary.size).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i + 3
  print ' '
  puts j
}
