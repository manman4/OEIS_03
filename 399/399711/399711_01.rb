def max_f(n)
  subs = (1..(1 << n) - 1).map{|m| (1..n).select{|v| m[v - 1] == 1}}
  best = [0, [], []]
  subs.each_with_index{|s, i|
    subs[i..-1].each{|t|
      u = Hash.new(0)
      s.each{|x| t.each{|y| u[x * y] += 1}}
      f = u.values.count(1)
      best = [f, s, t] if f > best[0]
    }
  }
  best
end
def A399711(n)
  (1..n).map{|k| max_f(k)[0]}
end
p A399711(10)
