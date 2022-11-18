def A(k, n)
  a = []
  ary = [1]
  n.times{
    a << ary.count(k)
    ary += ary.uniq.sort.map{|i| ary.count(i)}
  }
  a
end

n = 100
ary = A(2, n)
b = [ary[0]]
(1..n - 1).each{|i|
  # print i
  # print ' '
  # puts ary[i] - ary[i-1]
  b << ary[i] - ary[i - 1]
}
p b