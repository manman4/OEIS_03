def max_f(n)
  subs = (1..(1 << n) - 1).map{|a| (1..n).select{|v| a[v - 1] == 1}}
  best = [0, [], []]
  subs.each_with_index{|s, i|
    subs[i..-1].each{|t|
      r = Hash.new(0)
      s.each{|x| t.each{|y| r[x * y] += 1}}
      f = r.values.count(1)
      best = [f, s, t] if f > best[0]
    }
  }
  best
end
def A399711(n)
  (1..n).map{|i| max_f(i)[0]}
end
p A399711(10)
