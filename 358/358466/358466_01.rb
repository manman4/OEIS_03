def A(k, n)
  a = []
  ary = [1]
  n.times{
    a << ary.count(k)
    ary += ary.uniq.sort.map{|i| ary.count(i)}
  }
  a
end

p A(1, 100)