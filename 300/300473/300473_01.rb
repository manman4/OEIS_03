require 'prime'

def A(n)
  i = 0
  cnt = 0
  ary = []
  while cnt < n
    i += 1
    if (i * i + 21 * i + 1).prime?
      cnt += 1
      ary << i
    end
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
