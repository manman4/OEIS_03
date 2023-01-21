def is_square(n)
  return false if n < 0
  Math.sqrt(n).to_i ** 2 == n
end

def A(n)
  a = []
  (1..n).each{|i|
    s = 0
    (1..i).each{|j|
      s += j if i % j == 0 && is_square(j - 10)
    }
    a << s
  }
  a
end

def C(n)
  ary = [1]
  a = [0] + A(n)
  (1..n).each{|i| ary << (1..i).inject(0){|s, j| s - a[j] * ary[-j]} / i}
  ary
end

def take_logarithm(ary)
  a = []
  ary.map{|i|
    if i > 0
      a <<  Math.log(1 + i)
    else
      a << -Math.log(1 - i)
    end
  }
  a
end

n = 10010
m = 10000
ary = take_logarithm(C(n))
print 'No,Value'
(1..m).each{|i|
  print i
  print ','
  puts ary[i - 1]
}
