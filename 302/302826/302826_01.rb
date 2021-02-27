require 'prime'

def A(n)
  ary = []
  (2..n).each{|i|
    cnt = 0
    (1..i).each{|j|
      k = j * j + (i - j)
      cnt += 1 if k.prime?
    }
    ary << cnt
  }
  ary
end

n = 10000
ary = A(n)
(2..n).each{|i|
  print i
  print ' '
  puts ary[i - 2]
}
