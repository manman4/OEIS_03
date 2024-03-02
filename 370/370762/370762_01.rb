def A(n)
  ary = [[1]]
  a = [1]
  n.times{
    b = []
    a.each{|i|
      b << 2 * i + 1
      b << 4 * i + 1
    }
    ary << b
    a = b
  }
  ary.flatten
end

n = 12
ary = A(n)
(0..ary.size - 1).each{|i|
  print i
  print ' '
  puts ary[i]
}