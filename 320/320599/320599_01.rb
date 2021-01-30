require 'prime'

def A(n)
  ary = [9]
  i = 10
  while ary.size < n
    ary << i if (4 * i + 1).prime? && (8 * i + 1).prime?
    i += 1
  end
  ary
end

n = 10000
ary = A(n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
