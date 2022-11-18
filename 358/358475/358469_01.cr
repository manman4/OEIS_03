def a(k, n)
  a = Array(Int32).new
  ary = [1]
  n.times{
    a << ary.count(k)
    ary += ary.uniq.sort.map{|i| ary.count(i)}
  }
  a
end

p a(7, 200)