require 'prime'

def A(n)
  ary = []
  Prime.each(10 ** 7){|i|
    j = i.to_s.reverse.to_i
    if (i * i + j * j).prime?
      ary << i
      break if ary.size == n
    end
  }
  ary
end

n = 5000
ary = A(n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
