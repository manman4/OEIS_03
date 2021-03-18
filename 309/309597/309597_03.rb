def A(n)
  ary = [3]
  (2..n).each{|i|
    j = 10 ** (2 ** (i - 2))
    ary << (j + 3) * (j - 1) / 3 - ary[-1]
  }
  ary
end

n = 10
ary = A(n)
p ary.map{|i| i * 3}
b=[]
(1..n).each{|i|
  j = ary[i - 1]
  j = j * (j + 1) / 2
  #break if j.to_s.size > 1000
  print i
  print ' '
  puts j
b<<j
}
p b
p b.map{|i| i * 9}
c = b.map{|i| i.to_s.split('').map(&:to_i)}
p c.map{|i| i.count(5)}
p c.map{|i| i.count(6)}

c.each{|i|
  i = i.reverse
  a = []
  (0..i.size - 1).each{|j|
    a << j if i[j] == 6
  }
  p a
}
