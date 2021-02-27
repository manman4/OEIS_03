require 'prime'

def A(n)
  ary = []
  (1..n).each{|i|
    ary << i if (i ** 2 + 1).prime? && ((i + 6) ** 2 + 1).prime?
  }
  ary
end

p ary = A(10 ** 4)
(1..200).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}
p ary.map{|i| i / 2}
