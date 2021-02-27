require 'prime'

def A(n)
  ary = []
  (2..n).each{|i|
    (1..i).each{|j|
      k = j * j + (i - j)
      ary << k if k.prime?
    }
  }
  ary
end

n = 421
ary = A(n)
(2..ary.size + 1).each{|i|
  print i
  print ' '
  puts ary[i - 2]
}
