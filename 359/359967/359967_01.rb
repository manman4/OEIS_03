def is_square(n)
  Math.sqrt(n).to_i ** 2 == n
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
