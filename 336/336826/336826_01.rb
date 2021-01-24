def A(n)
  n.to_s.split('').map(&:to_i).inject(:*)
end

def B(n)
  ary = [0]
  (1..n).each{|i|
    j = i * A(i)
    ary << j if j > 0
  }
  ary.uniq.sort
end

n = 3273375
# n = 50000000
ary = B(n)
b = []
(1..10000).each{|i|
  j = ary[i - 1]
  b << j if j + 1 == ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
p b
