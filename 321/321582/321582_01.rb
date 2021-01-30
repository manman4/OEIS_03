require 'prime'

def A(n)
  ary = []
  Prime.each(10 ** 7){|i|
    j = 4 * i + 1
    if j % 5 == 0
      j /= 5
      ary << i if j.prime?
      break if ary.size == n
    end
  }
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
