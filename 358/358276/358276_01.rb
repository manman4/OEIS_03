# a[0] = 0
def A(n)
  ary = [0, 1]
  (2..n).each{|i|
    s = 0
    (1..i - 1).each{|j|
      if i % j == 0
        s += (i / j) * (-1) ** (i / j - 1) * ary[j]
      end
    }
    ary << s
  }
  ary
end

n = 10000
ary = A(n)
(1..n).each{|i|
  print i
  print ' '
  puts ary[i]
}
