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

def C(n)
  ary = [1]
  a = [0] + A359967(n)
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
