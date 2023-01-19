require 'prime'

def is_cube(n)
  return true if n < 2
  n.prime_division.all?{|i| i[1] % 3 == 0}
end

def A359943(n)
  a = []
  (1..n).each{|i|
    s = 0
    (1..i).each{|j|
      s += j if i % j == 0 && is_cube(j - 1)
    }
    a << s
  }
  a
end

def C(n)
  ary = [1]
  a = [0] + A359943(n)
  (1..n).each{|i| ary << (1..i).inject(0){|s, j| s - a[j] * ary[-j]} / i}
  ary
end

n = 10100
m = 10000
ary = C(n)
(0..m).each{|i|
  print i
  print ' '
  puts ary[i]
}
