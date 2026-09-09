def max_f(n)
  subs = (1...(1 << n)).map { |m| (1..n).select { |v| m[v - 1] == 1 } }
  best = [0, [], []]
  subs.each_with_index do |a, i|
    subs[i..].each do |b|                    
      c = Hash.new(0)
      a.each { |x| b.each { |y| c[x * y] += 1 } }
      f = c.count { |_, v| v == 1 }
      best = [f, a, b] if f > best[0]
    end
  end
  best
end

def A399711(n)
  (1..n).map{|k| max_f(k)[0]}
end
p A399711(10)
