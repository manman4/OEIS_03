require 'prime'

def is_square(n)
  return true if n < 2
  n.prime_division.all?{|i| i[1] % 2 == 0}
end

def A359967(n)
  a = []
  (1..n).each{|i|
    s = 0
    (1..i).each{|j|
      s += j if i % j == 0 && is_square(j + 1)
    }
    a << s
  }
  a
end

n = 10100
m = 10000
ary = A359967(n)
(1..m).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}
